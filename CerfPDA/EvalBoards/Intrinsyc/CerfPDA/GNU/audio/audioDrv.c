/**
 *********************************************************************
 *	@file       erAudio.c
 *	@version    0.1
 *	@author     ELR
 *	@date       22 Feb 2006 
 *
 *	Project:    Sip Stack
 *
 *  @brief      CerfPDA's audio driver
 *
 *
 *  @todo
 *
 *	Revision History:
 * 
 *
 *	COPYRIGHT (C) 2006 Eduardo Luis Romero. Université de Sherbrooke.
 *  Québec, Canada.
 *
 *********************************************************************/


/* //////////////////////////////////////////////////////////////////////////// */
/* include files                           //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */


#include <os_cpu.h>
#include <cpld.h>
#include <gpio.h>
#include <ppc.h>
#include <ucb1200.h>
#include <mcp_sa.h>
#include <ssp.h>
#include <dma.h>
#include <uda1341.h>
#include <bsp.h>
#include <audioDrv.h>

#include <debugELR2.h>

/* //////////////////////////////////////////////////////////////////////////// */
/* define cosntants & macros               //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

#define     preg_t      INT32U
#define     NULL        00

/* //////////////////////////////////////////////////////////////////////////// */
/* local variables                         //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

static const unsigned char udaReset[1]  = {0x40};                               /* reset -> STATUS = 01000000B                  */
static const unsigned char udaConfig[2] = {0x22, 0xE3};                         /* 0x22 = system clock frqcy = 256 fs           */
                                                                                /*        Data input mode: LSB-justified 16 bit */
                                                                                /* 0xE3 = OGS on   (output +6 dB)               */
                                                                                /*        IGS on,  (input +6dB)                 */
                                                                                /*        PAD off, (polarity ADC: non-inverted) */
                                                                                /*        PDA off, (polarity DAC: non-inverted) */
                                                                                /*        DS  off, (single speed playback)      */
                                                                                /*        PC  11   (ADC on, DAC on)             */
static const unsigned char udaConfig2[4] = {0xC2, 0xE2, 0xC4, 0xF0};

static struct {                                                                 /* input channel control structure              */
    INT8U           ready;                                                      /* flag channel ready (1) / not ready (0)       */
    INT8U           inUse;                                                      /* flag channel stopped (0) / in use (1)        */
    INT8U           currentBuff;                                                /* current buffer indic: 0: buf A / 1: buf B    */
    INT16U          bufSize;                                                    /* buffer size                                  */
    void            (*onBufDone)(void);                                         /* callback fucntion: called when the transfert */
                                                                                /*  to the buffer has been completed.           */
                                                                                /*  used for signal the event to the system     */
    }inChnnlCtl;

static struct {                                                                 /* output channel control structure             */
    INT8U           ready;                                                      /* flag channel ready (1) / not ready (0)       */
    INT8U           inUse;                                                      /* flag: 0/1 -> channel stopped/in use          */
    INT8U           currentBuff;                                                /* current buffer indic: 0: buf A / 1: buf B    */
    INT16U          bufSize;                                                    /* buffer size                                  */
    void            (*onBufDone)(void);                                         /* callback fucntion: called when the transfert */
                                                                                /*  from the buffer has been completed.         */
                                                                                /*  used for signal the event to the system     */
    }outChnnlCtl;

INT16U  inBuffA[2*SOUNDDEVBUFF_MAXSIZE];                                        /* input buffer A                               */
INT16U  inBuffB[2*SOUNDDEVBUFF_MAXSIZE];                                        /* input buffer B                               */
INT16U  outBuffA[2*SOUNDDEVBUFF_MAXSIZE];                                       /* output buffer A                              */
INT16U  outBuffB[2*SOUNDDEVBUFF_MAXSIZE];                                       /* output buffer B                              */

/* //////////////////////////////////////////////////////////////////////////// */
/* local function prototypes               //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */
void L3SendBit (int val);
void L3SendByte (unsigned char data, int mode);
void L3WrCommd(unsigned char addr, unsigned char *dataP, unsigned int dataLen);
void sndUda1341Cmmd (const unsigned char *dataP,unsigned int len);
void sndUda1341val(const unsigned char *dataP, unsigned int len);
void inChnnlDMA_IRQ(void);
void outChnnlDMA_IRQ(void);
void inChnnlDMAini(void);
void outChnnlDMAini(void);


/* //////////////////////////////////////////////////////////////////////////// */
/* functions                               //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

/** 
 * @fn      void L3Init(void)
 * @brief   L3 bus initialization
 *
 * The codec UDA1341TS is configured using the L3 bus that is controlled from
 * the UCB1200 I/O pins.
 * This functions initializates the three Ucb1200 I/O pins as outputs and sets
 * their initial values to level '1'
 *
 * @return  void
 * @see
 *
 */
void L3Init (void)
{
    /* set the output level to '1' using a read-modify-write access */
	mcp_write_codec_register(UCB1200_REG_IO_DATA,                                   /* write the USB_IO_DATA_REG with...    */
                            mcp_read_codec_register(UCB1200_REG_IO_DATA)            /*   read the actual value of the reg   */
                            | (L3_MODE | L3_CLK | L3_DATA));                        /*   doing the .OR. with the mask       */
    /* configure the I/O pins as outputs    */
	mcp_write_codec_register(UCB1200_REG_IO_DIRECTION,
                            mcp_read_codec_register(UCB1200_REG_IO_DIRECTION)
                            | (L3_MODE | L3_CLK | L3_DATA));
}

/** 
 * @fn      void L3SendBit(int val)
 * @brief   Send val's LSB to the L3 bus
 *
 * @param   val     data
 * @return  void
 * @see
 *
 */

void L3SendBit (int val)
{
	mcp_write_codec_register(UCB1200_REG_IO_DATA,                                   /* set the Ck to '0'                    */
            mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(L3_CLK));
                                                                                    /* set the bit value                    */
	if (val & 0x01)
        mcp_write_codec_register(UCB1200_REG_IO_DATA,                               /*    LSB is '1'                        */
                mcp_read_codec_register(UCB1200_REG_IO_DATA) | (L3_DATA));
	else
		mcp_write_codec_register(UCB1200_REG_IO_DATA,                               /*    LSB is '0'                        */
                mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(L3_DATA));

	udelay(L3_DELAY);                                                               /*  delay                               */
	mcp_write_codec_register(UCB1200_REG_IO_DATA,                                   /* set the Ck to '1'                    */
            mcp_read_codec_register(UCB1200_REG_IO_DATA) | (L3_CLK));
	udelay(L3_DELAY);                                                               /*  delay                               */
    mcp_write_codec_register(UCB1200_REG_IO_DATA,                                   /*  restore L3_DATA                     */
                mcp_read_codec_register(UCB1200_REG_IO_DATA) | (L3_DATA));

}

/** 
 * @fn      void L3SendByte(unsigned char data, int mode)
 * @brief   Send data byte to the L3 bus
 *
 * This function allows multibyte transfers controlled by
 * the "mode" parameter. When mode is set to ADDR_MODE,
 * L3_MODE is set to low ('0') during the transfer. If mode
 * is FIRST_BYTE, L3_MODE is set to high ('1') during the
 * transfer. If mode is neither ADDR_MODE nor FIRST_BYTE,
 * then FOLLOW_BYTES is assumed: the L3_MODE is pulsed to
 * low and then set to high during the transfert.
 *
 * @param   data    data byte to send
 * @param   mode    defines the L3_MODE behavior. See the description.
 * @return  void
 * @see
 *
 */
void L3SendByte (unsigned char data, int mode)
{
	int i;
    
	switch(mode){
	case ADDR_MODE:
		mcp_write_codec_register(UCB1200_REG_IO_DATA,                                   /* address mode: set L3_MODE to low     */
                mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(L3_MODE));
		break;
	case FIRST_BYTE:                                                                    /* first data byte: L3_MODE remains high */
		break;
	default:                                                                            /* following data bytes: pulse L3_MODE  */
		mcp_write_codec_register(UCB1200_REG_IO_DATA,
                mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(L3_MODE));             /*    ... set L3_MODE to low            */
		udelay(L3_DELAY);                                                               /*    ... wait...                       */
		mcp_write_codec_register(UCB1200_REG_IO_DATA,
                mcp_read_codec_register(UCB1200_REG_IO_DATA) | (L3_MODE));              /*    ... and return L3_MODE to high    */
		break;
	}
	udelay(L3_DELAY);                                                                   /* delay                                */
	for (i = 0; i < 8; i++)                                                             /* send the 8 bits                      */
		L3SendBit(data >> i);

	if (mode == ADDR_MODE)
		mcp_write_codec_register(UCB1200_REG_IO_DATA,                                   /* if address mode: return L3_MODE to high */
                mcp_read_codec_register(UCB1200_REG_IO_DATA) | (L3_MODE));
	udelay(L3_DELAY);                                                                   /* delay                                */
}

/** 
 * @fn      void L3WrCommd(unsigned char addr, unsigned char *dataP, unsigned int dataLen)
 * @brief   Send data byte to the L3 bus
 *
 * This function performs a multibyte transfer over the L3 bus.
 *
 * @param   addr    destination register address
 * @param   dataP   pointer to the data array to send
 * @param   dataLen number of data bytes to send
 * @return  void
 * @see
 *
 */
void L3WrCommd(unsigned char addr, unsigned char *dataP, unsigned int dataLen)
{
	int mode;
    
    mode = 0;
	L3SendByte(addr, mode++);                                                           /* send the address byte                */
	while (dataLen--)
		L3SendByte(*dataP++, mode++);                                                   /* and then the followins data bytes    */
}

/** 
 * @fn      void sndUda1341Cmmd (const unsigned char *dataP, unsigned int len)
 * @brief   Send data array to the UDA1341 in STATUS mode
 *
 * This function performs a multibyte transfer to UDA1341's status
 * register. It allows to control the reset, set the system clock
 * frecuency, data input format, etc.
 *
 * @param   dataP   pointer to the data array
 * @param   len     number of data bytes to send
 * @return  void
 * @see     UDA1341TS data sheet
 *
 */
void sndUda1341Cmmd (const unsigned char *dataP, unsigned int len)
{
	int mode = 0;
   
    L3SendByte(0x16, mode++);                                                           /* address = 00010110B  (STATUS Mode)    */
	while (len--)
		L3SendByte(*dataP++, mode++);
}

/** 
 * @fn      void sndUda1341val(const unsigned char *dataP, unsigned int len)
 * @brief   Send the data array to the UDA1341 in DATA0 mode
 *
 * This function performs a multibyte transfer to UDA1341's direct addressing
 * registers. It allows to control the volume, bass boost, treble, mute, etc.
 *
 * @param   dataP   pointer to the data array
 * @param   len     number of data byte to send
 * @return  void
 * @see     UDA1341TS data sheet
 *
 */
void sndUda1341val(const unsigned char *dataP, unsigned int len)
{
    unsigned int    mode;

    mode = 0;
	L3SendByte(0x14, mode++);                                                           /* address = 00010100B                  */
	while (len--)
		L3SendByte(*dataP++, mode++);                                                   /* send the data bytes                  */
}

/** 
 * @fn      void sndUda1341Xval(const unsigned char dataP)
 * @brief   Send the data byte to the UDA1341 in DATA0 mode
 *
 * This function performs a byte transfer to UDA1341's direct addressing
 * registers. It allows to control the volume, bass boost, treble, mute, etc.
 *
 * @param   dataP   data byte
 * @return  void
 * @see     UDA1341TS data sheet
 *
 */
void sndUda1341Xval(const unsigned char dataP)
{
    unsigned int    mode;

    mode = 0;
	L3SendByte(0x14,  mode++);                                                          /* address = 00010100B                  */
    L3SendByte(dataP, mode);                                                            /* send the data bytes                  */
}

/** 
 * @fn      void iniUda1341(void)
 * @brief   Initilizate UDA1341TS
 *
 * @return  void
 * @see     UDA1341TS data sheet
 *
 */
void iniUda1341(void)
{
    sndUda1341Cmmd((const unsigned char *)&udaReset,sizeof(udaReset));         /* reset the UDA                                */
    udelay(1000);
    sndUda1341Cmmd((const unsigned char *)&udaConfig,sizeof(udaConfig));       /* and send the default config                  */
    udelay(1000);
    sndUda1341val((const unsigned char *)&udaConfig2,sizeof(udaConfig2));      /* and send the default config                  */

}


/** 
 * @fn      void cerfSamplerate(cPDAaudiofSamp fSmplng)
 * @brief   Set UDA1341TS sample rate.
 *
 * The UDA1341 sampling rate is set writing the CPLD at the register
 * CERF_PDA_CPLD_AUDIO_FSMP (offset 0x04)
 *
 * @param   fSmplng     sampling rate.
 * @return  void
 * @see     CerfPDA's documentation 
 *
 */
void cerfSamplerate(cPDAfrSampl_e fSmplng)
{
    CERF_PDA_CPLD_AUDIO_FSMP = fSmplng;
}

/** 
 * @fn      void iniSSP(void)
 * @brief   Initializate SSP port.
 *
 * @return  void
 * @see     CerfPDA's documentation 
 *
 */
void iniSSP(void)
{
    mcp_disable();                                                                      /* disable MCP before configure the SSP */
    CERF_PDA_SSP_CNTRL0  = 0x00;                                                        /* make sure than the SSP is disable    */
    CERF_PDA_SSP_STATUS |= 0x40;                                                        /* clear ROR bit (receiver buf overrun  */
    GPIO_GAFR |= (GPIO_SSP_TXD | GPIO_SSP_RXD | GPIO_SSP_SCLK |                         /* SSP uses the pin alternate functions */
                  GPIO_SSP_SFRM | GPIO_SSP_CLK);
   	GPIO_GPDR |= (GPIO_SSP_TXD | GPIO_SSP_SCLK | GPIO_SSP_SFRM);                        /* define TXD, SCLK & SFRM as outputs   */
    GPIO_GPDR &= ~(GPIO_SSP_RXD | GPIO_SSP_CLK);                                        /* define RXD & CLK as inputs           */
    GPIO_GPCR = 0x02000000;                                                             /* clear output pin 25                  */
    PPC_PPAR  |= 0x00040000;                                                            /* SSP pin reassignment                 */
    CERF_PDA_SSP_CNTRL0 = 0x0000001F;                                                   /* TI frame format                      */
                                                                                        /*      16 bits de data size            */
    CERF_PDA_SSP_CNTRL1 = 0x00000020;                                                   /* External clock used on GPIO 19       */
    CERF_PDA_SSP_CNTRL0 |=0x00000080;                                                   /* start up SSP using pin reassignment  */
    mcp_enable();                                                                       /* re-enable MCP                        */
}

/** 
 * @fn      void iniAudioDvce(void)
 * @brief   Initializate CerfPDA's audio device
 *
 * @return  void
 * @see     CerfPDA's documentation 
 *
 */
void iniAudioDvce(void)
{
	mcp_enable();
    ucb1200_init();
    L3Init();
    udelay(5);
    iniSSP();
	cerfSamplerate(DEFAULT_AUDIO_FSAMP);

    CERF_PDA_CPLD_AUDIO_LRSYNC = 1;                                                     /* sound reset                          */
    CERF_PDA_CPLD_AUDIO_LRSYNC = 0;                                                     /* sound reset                          */
    CERF_PDA_CPLD_AUDIO_ENA = 1;                                                        /* enable sound system                  */ 
    iniUda1341();                                                                       /* uda1341 initialisation               */
}	

/*
*  |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
*  |||||  CerfPDA audio control functions
*  |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
*  |||||    Input channel
*  ||||||||||||||||||||||||||||||||||||||| 
*/

/** 
 * @fn      void inChnnlDMAini(void)
 *
 * @brief   Initializate DMA 0 as an input channel
 *
 * @return  void
 * @see
 *
 */
void inChnnlDMAini(void)
{
    DMA_DCSR0_W0 = 0x7F;                                                            /* reset the DMA channel                    */
    DMA_DDAR0    = 0x81C01BF9;                                                      /* see the manual, section 11.6.1.1         */
    DMA_DBSB0    = (preg_t)inBuffB;                                                 /* input buffer A address                   */
    DMA_DBTA0    = 0;                                                               /* transf count buffer A                    */
    DMA_DBSB0    = (preg_t)inBuffB;                                                 /* input buffer B address                   */
    DMA_DBTB0    = 0;                                                               /* transf count buffer B                    */
    request_irq(20,inChnnlDMA_IRQ);
}


/** 
 * @fn      void inChnnlDMAend(void)
 *
 * @brief   close DMA 1 channel
 *
 * @return  void
 * @see
 *
 */
void inChnnlDMAend(void)
{
    DMA_DCSR0_W0 = 0x7F;                                                            /* reset the DMA channel                    */
    free_irq(20);                                                                   /* set up the interrupt handler             */
}

/** 
 * @fn      void inChnnlInit(cPDAfrSampl_e samplRate,INT16U bufSize, void (*onBufDone)(void))
 *
 * @brief   Initializate audio input channel
 *
 * @param   samplRate   sampling rate, see the cPDAfrSampl_e enumeration
 * @param   bufSize     buffer size exprssed in samples
 * @param   onBufDone   callback fucntion "on buffer done"
 * @return  void
 * @see
 *
 */
void inChnnlInit(cPDAfrSampl_e samplRate,INT16U bufSize, void (*onBufDone)(void))
{
    inChnnlCtl.ready = 0;
    inChnnlCtl.inUse = 0;                                                       /* set the inUse flag to 0 (stopped)            */
    inChnnlCtl.currentBuff = 0;                                                 /* current buff = 0 (buffer A)                  */
    inChnnlCtl.bufSize= 4*bufSize;                                              /* 2 interleave channels (Left & Right)         */   

    inChnnlCtl.onBufDone = onBufDone;                                           /* set the callback fucntion                    */
    inChnnlClrBuf();
    inChnnlDMAini();
    inChnnlCtl.ready = 1;                                                       /* set the "channel ready" indicator            */

}

/** 
 * @fn      void inChnnlEnd(void)
 *
 * @brief   End audio input channel operation
 *
 * @return  void
 * @see
 *
 */
void inChnnlEnd(void)
{
    OS_CPU_SR   cpu_sr;

    OS_ENTER_CRITICAL();
    inChnnlCtl.ready = 0;
    inChnnlCtl.onBufDone = NULL;
    inChnnlDMAend();
    OS_EXIT_CRITICAL();
}


/** 
 * @fn      void inChnnlClrBuf(void)
 *
 * @brief   Initializate audio input buffers with 0
 *
 * @return  void
 * @see
 *
 */
void inChnnlClrBuf(void)
{
    memset((void*)inBuffA,0,sizeof(inBuffA));
    memset((void*)inBuffB,0,sizeof(inBuffB));
}

/** 
 * @fn      INT16S inChnnlStart(void)
 *
 * @brief   Start input channel operation.
 *
 * This fucntion starts the channel operation enabling the DMA
 * transferts. Both STARTA and STARTB bits are set, the starting
 * buffer is determined by the status of the BIU flag of
 * the DMA channel. The function inChnnlNxtBuf() signals the
 * buffer to use first.
 *
 * @param   void
 * @return  0 on success 
 * @see
 *
 */
INT16S inChnnlStart(void)
{
    if( !inChnnlCtl.ready || (DMA_DCSR0_RO & 0x01))                             /* return -1 if channel not ready or the DMA    */
        return -1;                                                              /*   is already running                         */

    inChnnlClrBuf();
    DMA_DCSR0_W0 = 0x7F;
    DMA_DBTA0    = inChnnlCtl.bufSize;                                          /* set up buffer A                              */
    DMA_DBSA0    = (preg_t)inBuffA;
    DMA_DBTB0    = inChnnlCtl.bufSize;                                          /* set up buffer B                              */
    DMA_DBSB0    = (preg_t)inBuffB;
    DMA_DCSR0_W1 = 0x53;                                                        /*   RUN    = 1                                 */
                                                                                /*   IE     = 1                                 */
                                                                                /*   STARTA = 1                                 */
                                                                                /*   STARTB = 1                                 */
    return 0;
}

/** 
 * @fn      INT16S inChnnlStop(void)
 *
 * @brief   Stop input channel operation.
 *
 * @param   void
 * @return  0 on success 
 * @see
 *
 */
INT16S inChnnlStop(void)
{
    if( !inChnnlCtl.ready || !(DMA_DCSR0_RO & 0x01))                            /* return -1 if channel not ready or the DMA    */
        return -1;                                                              /*   is not running                             */
    DMA_DCSR0_W0 = 0x01;                                                        /* stop the DMA channel:                        */
                                                                                /*   RUN    = 0                                 */
    return 0;
}

/** 
 * @fn      void inChnnlReset(void)
 *
 * @brief   Reset input channel operation.
 *
 * @param   void
 * @return  0 on success 
 * @see
 *
 */
void inChnnlReset(void)
{
    DMA_DCSR0_W0 = 0x7F;                                                        /* reset the DMA channel                        */
    DMA_DBTA0    = 0;                                                           /* transf count buffer A = 0                    */
    DMA_DBTB0    = 0;                                                           /* transf count buffer B = 0                    */
    inChnnlCtl.ready = 0;                                                       /* clear the "channel ready" indicator          */
}

/** 
 * @fn      void* inChnnlNxtBuf(void)
 *
 * @brief   Get next buffer to fill in
 *
 * @param   void
 * @return  pointer to the buffer
 * @see
 *
 */
void* inChnnlNxtBuf(void)
{
    INT16U  currntStat;

    currntStat = DMA_DCSR0_RO;
    if((currntStat & 0x11) || (currntStat & 0x41)){                             /* is there a transfer in progress??            */
        return (currntStat & 0x80)? (void*)inBuffA : (void*)inBuffB;            /*   yes, if BIU = 0 --> Next: Buffer A         */
                                                                                /*           BIU = 1 --> Next: Buffer B         */
    } else {
        return (currntStat & 0x80)? (void*)inBuffB : (void*)inBuffA;            /*   no,  if BIU = 0 --> Next: Buffer B         */
                                                                                /*           BIU = 1 --> Next: Buffer A         */
    }
}

/** 
 * @fn      void* inChnnlBufInUse(void)
 *
 * @brief   Get input buffer in use
 *
 *
 * @param   void
 * @return  pointer to the buffer in use
 * @see
 *
 */
void* inChnnlBufInUse(void)
{
    INT16U  currntStat;

    currntStat = DMA_DCSR0_RO;
    if((currntStat & 0x11) || (currntStat & 0x41)){                             /* is there a transfer in progress??            */
        return (currntStat & 0x80)? (void*)inBuffB : (void*)inBuffA;            /*   yes, if BIU = 0 --> Buffer A is in use     */
                                                                                /*           BIU = 1 --> Buffer B is in use     */
    } else {
        return (currntStat & 0x80)? (void*)inBuffA : (void*)inBuffB;            /*   no,  if BIU = 0 --> Buffer B is in use     */
                                                                                /*           BIU = 1 --> Buffer A is in use     */
    }
}


/** 
 * @fn     void inChnnlDMA_IRQ(void)
 *
 * @brief   In_channel (DMA 0) interrupt handler
 *
 * @param   void
 * @return  pointer to the buffer in use
 * @see
 *
 */
void inChnnlDMA_IRQ(void)
{
    register INT16U   dmaStat;

    dmaStat = DMA_DCSR0_RO;

    switch (dmaStat & 0xf8){
        case 0x40:
        case 0xC8:
            DMA_DBTA0    = inChnnlCtl.bufSize;                                  /* buf B in use: set STARTA                     */
            DMA_DBSA0    = (preg_t)inBuffA;
            DMA_DCSR0_W1 = 0x10;
            break;
        case 0x30:
        case 0x88:
            DMA_DBTB0    = inChnnlCtl.bufSize;                                  /* buf A in use: set STARTB                     */
            DMA_DBSB0    = (preg_t)inBuffB;
            DMA_DCSR0_W1 = 0x40;
            break;
//        default:
    }
    if( inChnnlCtl.onBufDone)                                                  /* if defined, call the callback function       */
        inChnnlCtl.onBufDone();
    return;
}


/*
*  |||||    Output channel
*  ||||||||||||||||||||||||||||||||||||||| 
*/

/** 
 * @fn      void outChnnlDMAini(void)
 *
 * @brief   Initializate DMA 0 as an input channel
 *
 * @return  void
 * @see
 *
 */
void outChnnlDMAini(void)
{
    DMA_DCSR1_W0 = 0x7F;                                                            /* reset the DMA channel                    */
    DMA_DDAR1    = 0x81C01BE8;                                                      /* see the manual, section 11.6.1.1         */
    DMA_DBSA1    = (preg_t)outBuffA;                                                /* output buffer A address                  */
    DMA_DBTA1    = 0;                                                               /* transf count buffer A                    */
    DMA_DBSB1    = (preg_t)outBuffB;                                                /* output buffer B address                  */
    DMA_DBTB1    = 0;                                                               /* transf count buffer B                    */
    request_irq(21,outChnnlDMA_IRQ    );                                            /* set up the interrupt handler             */
}

/** 
 * @fn      void outChnnlDMAend(void)
 *
 * @brief   close DMA 0 channel
 *
 * @return  void
 * @see
 *
 */
void outChnnlDMAend(void)
{
    DMA_DCSR1_W0 = 0x7F;                                                            /* reset the DMA channel                    */
    free_irq(21);                                                                   /* set up the interrupt handler             */
}

/** 
 * @fn      void outChnnlInit(cPDAfrSampl_e samplRate,INT16U bufSize, void (*onBufDone)(void))
 *
 * @brief   Initializate audio output channel
 *
 * @param   samplRate   sampling rate, see the cPDAfrSampl_e enumeration
 * @param   bufSize     buffer size exprssed in samples
 * @param   onBufDone   callback fucntion "on buffer done"
 *
 * @return  void
 * @see
 *
 */
void outChnnlInit(cPDAfrSampl_e samplRate,INT16U bufSize, void (*onBufDone)(void))
{
    outChnnlCtl.ready = 0;
    outChnnlCtl.inUse = 0;                                                       /* set the inUse flag to 0 (stopped)            */
    outChnnlCtl.currentBuff = 0;                                                 /* current buff = 0 (buffer A)                  */
    outChnnlCtl.bufSize= 4*bufSize;                                              /* 2 interleave channels (Left & Right)         */

    outChnnlCtl.onBufDone = onBufDone;                                           /* set the callback fucntion                    */
    outChnnlClrBuf();
    outChnnlDMAini();
    outChnnlCtl.ready = 1;                                                       /* set the "channel ready" indicator            */
}


/** 
 * @fn      void outChnnlEnd(void)
 *
 * @brief   End audio output channel operation
 *
 * @return  void
 * @see
 *
 */
void outChnnlEnd(void)
{
    OS_CPU_SR   cpu_sr;

    OS_ENTER_CRITICAL();
    outChnnlCtl.ready = 0;
    outChnnlCtl.onBufDone = NULL;
    outChnnlDMAend();
    OS_EXIT_CRITICAL();
}

/** 
 * @fn      void outChnnlClrBuf(void)
 *
 * @brief   Initializate audio output buffers with 0
 *
 * @return  void
 * @see
 *
 */
void outChnnlClrBuf(void)
{
    memset((void*)outBuffA,0,sizeof(outBuffA));
    memset((void*)outBuffB,0,sizeof(outBuffB));
}

/** 
 * @fn      INT16S outChnnlStart(void)
 *
 * @brief   Start output channel operation.
 *
 * This fucntion starts the channel operation enabling the DMA
 * transfert. Both STARTA and STARTB bits are set, the
 * starting buffer is determined by the status of the BIU flag of
 * the DMA channel. The function inChnnlNxtBuf() signals the
 * buffer to use first.
 *
 * @param   void
 * @return  0 on success 
 * @see
 *
 */
INT16S outChnnlStart(void)
{
    if( !outChnnlCtl.ready || (DMA_DCSR1_RO & 0x01))                            /* return -1 if channel not ready or the DMA    */
        return -1;                                                              /*   is already running                         */
    DMA_DCSR1_W0 = 0x7F;

    outChnnlClrBuf();
    DMA_DCSR1_W0 = 0x7F;
    DMA_DBTA1    = outChnnlCtl.bufSize;                                          /* set up buffer A                              */
    DMA_DBSA1    = (preg_t)outBuffA;
    DMA_DBTB1    = outChnnlCtl.bufSize;                                          /* set up buffer B                              */
    DMA_DBSB1    = (preg_t)outBuffB;
    DMA_DCSR1_W1 = 0x53;                                                         /*   RUN    = 1                                 */
                                                                                 /*   IE     = 1                                 */
                                                                                 /*   STARTA = 1                                 */
                                                                                 /*   STARTB = 1                                 */
    return 0;
}

/** 
 * @fn      INT16S outChnnlStop(void)
 *
 * @brief   Stop output channel operation.
 *
 * @param   void
 * @return  0 on success 
 * @see
 *
 */
INT16S outChnnlStop(void)
{
    if( !outChnnlCtl.ready || !(DMA_DCSR1_RO & 0x01))                           /* return -1 if channel not ready or the DMA    */
        return -1;                                                              /*   is not running                             */
    DMA_DCSR1_W0 = 0x01;                                                        /* stop the DMA channel:                        */
                                                                                /*   RUN    = 0                                 */
    return 0;
}

/** 
 * @fn      void outChnnlReset(void)
 *
 * @brief   Reset output channel operation.
 *
 * @param   void
 * @return  0 on success 
 * @see
 *
 */
void outChnnlReset(void)
{
    DMA_DCSR1_W0 = 0x7F;                                                        /* reset the DMA channel                        */
    DMA_DBTA1    = 0;                                                           /* transf count buffer A = 0                    */
    DMA_DBTB1    = 0;                                                           /* transf count buffer B = 0                    */
    outChnnlCtl.ready = 0;                                                      /* clear the "channel ready" indicator          */
}

/** 
 * @fn      void* outChnnlNxtBuf(void)
 *
 * @brief   Get next output buffer to fill in
 *
 * @param   void
 * @return  pointer to the buffer
 * @see
 *
 */
void* outChnnlNxtBuf(void)
{
    INT16U  currntStat;

    currntStat = DMA_DCSR1_RO;
    if((currntStat & 0x11) || (currntStat & 0x41)){                             /* is there a transfer in progress??            */
        return (currntStat & 0x80)? (void*)outBuffA : (void*)outBuffB;          /*   yes, if BIU = 0 --> Next: Buffer A         */
                                                                                /*           BIU = 1 --> Next: Buffer B         */
    } else {
        return (currntStat & 0x80)? (void*)outBuffB : (void*)outBuffA;          /*   no,  if BIU = 0 --> Next: Buffer B         */
                                                                                /*           BIU = 1 --> Next: Buffer A         */
    }
}

/** 
 * @fn      void* outChnnlBufInUse(void)
 *
 * @brief   Get output buffer in use
 *
 * @param   void
 * @return  pointer to the buffer in use
 * @see
 *
 */
void* outChnnlBufInUse(void)
{
    INT16U  currntStat;

    currntStat = DMA_DCSR1_RO;
    if((currntStat & 0x11) || (currntStat & 0x41)){                             /* is there a transfer in progress??            */
        return (currntStat & 0x80)? (void*)outBuffB : (void*)outBuffA;          /*   yes, if BIU = 0 --> Buffer A is in use     */
                                                                                /*           BIU = 1 --> Buffer B is in use     */
    } else {
        return (currntStat & 0x80)? (void*)outBuffA : (void*)outBuffB;          /*   no,  if BIU = 0 --> Buffer B is in use     */
                                                                                /*           BIU = 1 --> Buffer A is in use     */
    }
}


/** 
 * @fn     void inChnnlDMA_IRQ(void)
 *
 * @brief   In_channel (DMA 0) interrupt handler
 *
 * @param   void
 * @return  pointer to the buffer in use
 * @see
 *
 */
void outChnnlDMA_IRQ(void)
{
    register INT16U   dmaStat;

    dmaStat = DMA_DCSR1_RO;

    switch (dmaStat & 0xf8){
        case 0x40:
        case 0xC8:
            DMA_DBTA1    = outChnnlCtl.bufSize;                                 /* buf B in use: set STARTA                     */
            DMA_DBSA1    = (preg_t)outBuffA;
            DMA_DCSR1_W1 = 0x10;
            break;
        case 0x30:
        case 0x88:
            DMA_DBTB1    = outChnnlCtl.bufSize;                                 /* buf A in use: set STARTB                     */
            DMA_DBSB1    = (preg_t)outBuffB;
            DMA_DCSR1_W1 = 0x40;
            break;
    //    default:
    }
    if( outChnnlCtl.onBufDone)                                                  /* if defined, call the callback function       */
        outChnnlCtl.onBufDone();
    return;
}

/*
*  |||||    Start/Stop & IRQ functions
*  ||||||||||||||||||||||||||||||||||||||| 
*/

/** 
 * @fn      INT16S inChnnlStart(void)
 *
 * @brief   Starts input channel operation.
 *
 * This fucntion starts the channel operation enabling the DMA
 * transferts. Both STARTA and STARTB bits are set, the starting
 * actual buffer is determined by the status of the BIU flag of
 * the DMA channel. The function inChnnlNxtBuf() signals the
 * buffer to use first.
 *
 * @param   void
 * @return  0 on success 
 * @see
 *
 */
INT16S audioIoChnnlStart(void)
{
   inChnnlStart();
   outChnnlStart();
   return 0;
}

/** 
 * @fn      INT16S inChnnlStart(void)
 *
 * @brief   Stop input & output channels operation.
 *
 * @param   void
 * @return  0 on success 
 * @see
 *
 */
INT16S audioIoChnnlStop(void)
{
   outChnnlStop();
   inChnnlStop();
   return 0;
}


