#ifndef _ERCODECG711_H_
#define _ERCODECG711_H_

/**
 *********************************************************************
 *	@file       erG711.h
 *	@version    0.1
 *	@author     ELR
 *	@date       21 June 2006 
 *
 *  @brief      Codec G711 definitions
 *
 *
 *
 *	Revision History:
 * 
 * 
 *
 *	COPYRIGHT (C) 2006 Eduardo Luis Romero. Université de Sherbrooke.
 *  Québec, Canada.
 *
 *********************************************************************/


/* //////////////////////////////////////////////////////////////////////////// */
/* define cosntants                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

#define CODEC_DAC_2CHNNL        1
#define CODEC_ADC_2CHNNL        1

#define CODEC_ERRNONE           0
#define CODEC_ERR_BADARG        1

/* //////////////////////////////////////////////////////////////////////////// */
/* define types                            //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */


/* //////////////////////////////////////////////////////////////////////////// */
/* function prototypes                     //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

INT16U      erG711A_encoder (INT16S *ibufP, INT8U *obufP, INT16U bufLen);
INT16U      erG711A_decoder (INT8U *ibufP, INT16S *obufP, INT16U bufLen);
INT16U      erG711A_init (void* pData);
INT16U      erG711U_encoder (INT16S *ibufP, INT8U *obufP, INT16U bufLen);
INT16U      erG711U_decoder (INT8U *ibufP, INT16S *obufP, INT16U bufLen);
INT16U      erG711U_init (void* pData);


#endif  /* _ERCODECG711_H_ */
