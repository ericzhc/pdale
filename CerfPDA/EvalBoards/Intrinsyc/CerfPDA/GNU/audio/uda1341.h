/*
 * Philips UDA1341 mixer device driver
 *
 * Copyright (c) 2000 Nicolas Pitre <nico@cam.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 */

/*
 * UDA1341 L3 address and command types
 */
#define UDA1341_L3Addr		5
#define UDA1341_DATA0		0
#define UDA1341_DATA1		1
#define UDA1341_STATUS		2


/*
 * UDA1341 internal state variables.
 * The default values are defined to sane initial operating values.
 */

/* UDA1341 status settings */

#define UDA_STATUS0_IF_I2S	0
#define UDA_STATUS0_IF_LSB16	1
#define UDA_STATUS0_IF_LSB18	2
#define UDA_STATUS0_IF_LSB20	3
#define UDA_STATUS0_IF_MSB	4
#define UDA_STATUS0_IF_MSB16	5
#define UDA_STATUS0_IF_MSB18	6
#define UDA_STATUS0_IF_MSB20	7

#define UDA_STATUS0_SC_512FS	0
#define UDA_STATUS0_SC_384FS	1
#define UDA_STATUS0_SC_256FS	2

typedef struct {
	u16 DC_filter:1;	/* DC filter */
	u16 input_fmt:3;	/* data input format */
	u16 system_clk:2;	/* system clock frequency */
	u16 reset:1;		/* reset */
	const u16 select:1;	/* must be set to 0 */
} UDA_STATUS_0;

#define UDA_STATUS_0_DFLT \
	(UDA_STATUS_0){0, UDA_STATUS0_IF_LSB16, UDA_STATUS0_SC_256FS, 0, 0}

typedef struct {
	u16 DAC_on:1;		/* DAC powered */
	u16 ADC_on:1;		/* ADC powered */
	u16 double_speed:1;	/* double speed playback */
	u16 DAC_pol:1;	/* polarity of DAC */
	u16 ADC_pol:1;	/* polarity of ADC */
	u16 ADC_gain:1;	/* gain of ADC */
	u16 DAC_gain:1;	/* gain of DAC */
	const u16 select:1;	/* must be set to 1 */
} UDA_STATUS_1;

#define UDA_STATUS_1_DFLT	(UDA_STATUS_1){1, 1, 0, 0, 0, 1, 1, 1}

/* UDA1341 direct control settings */

typedef struct {
	u16 volume:6;		/* volume control */
	const u16 select:2;	/* must be set to 0 */
} UDA_DATA0_0;

#define UDA_DATA0_0_DFLT	(UDA_DATA0_0){21, 0}

typedef struct {
	u16 treble:2;
	u16 bass:4;
	const u16 select:2;	/* must be set to 1 */
} UDA_DATA0_1;

#define UDA_DATA0_1_DFLT	(UDA_DATA0_1){1, 5, 1}

typedef struct {
	u16 mode:2;		/* mode switch */
	u16 mute:1;
	u16 deemphasis:2;
	u16 peak_detect:1;
	const u16 select:2;	/* must be set to 2 */
} UDA_DATA0_2;

#define UDA_DATA0_2_DFLT	(UDA_DATA0_2){3, 0, 0, 1, 2}

/* DATA0 extended programming registers */

typedef struct {
	const u16 ext_addr:3;	/* must be set to 0 */
	const u16 select1:5;	/* must be set to 24 */
	u16 ch1_gain:5;	/* mixer gain channel 1 */
	const u16 select2:3;	/* must be set to 7 */
} UDA_DATA0_ext0;

#define UDA_DATA0_ext0_DFLT	(UDA_DATA0_ext0){0, 24, 4, 7}

typedef struct {
	const u16 ext_addr:3;	/* must be set to 1 */
	const u16 select1:5;	/* must be set to 24 */
	u16 ch2_gain:5;	/* mixer gain channel 2 */
	const u16 select2:3;	/* must be set to 7 */
} UDA_DATA0_ext1;

#define UDA_DATA0_ext1_DFLT	(UDA_DATA0_ext1){1, 24, 4, 7}

typedef struct {
	const u16 ext_addr:3;	/* must be set to 2 */
	const u16 select1:5;	/* must be set to 24 */
	u16 mixer_mode:2;
	u16 mic_level:3;	/* MIC sensitivity level */
	const u16 select2:3;	/* must be set to 7 */
} UDA_DATA0_ext2;

#define UDA_DATA0_ext2_DFLT	(UDA_DATA0_ext2){2, 24, 2, 4, 7}

typedef struct {
	const u16 ext_addr:3;	/* must be set to 4 */
	const u16 select1:5;	/* must be set to 24 */
	u16 ch2_igain_l:2;	/* input amplifier gain channel 2 (bits 1-0) */
	const u16 reserved:2;	/* must be set to 0 */
	u16 AGC_ctrl:1;	/* AGC control */
	const u16 select2:3;	/* must be set to 7 */
} UDA_DATA0_ext4;

#define UDA_DATA0_ext4_DFLT	(UDA_DATA0_ext4){4, 24, 0 & 3, 0, 1, 7}

typedef struct {
	const u16 ext_addr:3;	/* must be set to 5 */
	const u16 select1:5;	/* must be set to 24 */
	u16 ch2_igain_h:5;	/* input amplifier gain channel 2 (bits 6-2) */
	const u16 select2:3;	/* must be set to 7 */
} UDA_DATA0_ext5;

#define UDA_DATA0_ext5_DFLT	(UDA_DATA0_ext5){5, 24, 0 >> 2, 7}

typedef struct {
	const u16 ext_addr:3;	/* must be set to 6 */
	const u16 select1:5;	/* must be set to 24 */
	u16 AGC_level:2;	/* AGC output level */
	u16 AGC_const:3;	/* AGC time constant */
	const u16 select2:3;	/* must be set to 7 */
} UDA_DATA0_ext6;

#define UDA_DATA0_ext6_DFLT	(UDA_DATA0_ext6){6, 24, 3, 0, 7}

typedef struct {
	u16 peak:6;		/* peak level value */
} UDA_DATA1;

#define UDA_DATA1_DFLT		(UDA_DATA1){0}

/*
 * All registers
 */
typedef struct {
	UDA_STATUS_0 status_0;
	UDA_STATUS_1 status_1;
	UDA_DATA0_0 data0_0;
	UDA_DATA0_1 data0_1;
	UDA_DATA0_2 data0_2;
	UDA_DATA0_ext0 data0_ext0;
	UDA_DATA0_ext1 data0_ext1;
	UDA_DATA0_ext2 data0_ext2;
	UDA_DATA0_ext4 data0_ext4;
	UDA_DATA0_ext5 data0_ext5;
	UDA_DATA0_ext6 data0_ext6;
	UDA_DATA1 data1;
} UDA1341_regs_t;

#define UDA1341_REGS_DFLT {	\
	UDA_STATUS_0_DFLT,	\
	UDA_STATUS_1_DFLT,	\
	UDA_DATA0_0_DFLT,	\
	UDA_DATA0_1_DFLT,	\
	UDA_DATA0_2_DFLT,	\
	UDA_DATA0_ext0_DFLT,	\
	UDA_DATA0_ext1_DFLT,	\
	UDA_DATA0_ext2_DFLT,	\
	UDA_DATA0_ext4_DFLT,	\
	UDA_DATA0_ext5_DFLT,	\
	UDA_DATA0_ext6_DFLT,	\
	UDA_DATA1_DFLT		}

/*
 * Structure containing the necessary variables
 * for one instance of this chip.
 */
typedef struct {
	UDA1341_regs_t *regs;
	int (*L3Write)(char addr, char *data, int len);
	int (*L3Read)(char addr, char *data, int len);
	int active;		/* non zero if powered */
	int mix_modcnt;		/* mixer mods count */
} UDA1341_state_t;

/*
 * Function prototypes exported by this module
 */
extern int uda1341_mixer_ioctl (UDA1341_state_t *state, u16 cmd, u32 arg);
extern void uda1341_reset (UDA1341_state_t *state);

