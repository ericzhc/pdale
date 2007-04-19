//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2003 Universite de Sherbrooke All rights reserved.
//
// Module name:
//
//      ucb1200.h
//
// Description:
//
//      Interfaces with the UCB1200 hardware.
//
// Author:
//
//      Philippe Mabilleau ing.
//
// Created:
//
//      October 2003
//
////////////////////////////////////////////////////////////////////////////////

#ifndef UCB1200_H
#define UCB1200_H

#include <types.h>

#define MAX_CONTRAST       99
#define DEFAULT_CONTRAST   25

#define UCB1200_REG_IO_DATA (0)
#define UCB1200_REG_IO_DIRECTION (1)
#define UCB1200_REG_RISE_INT_ENABLE (2)
#define UCB1200_REG_FALL_INT_ENABLE (3)
#define UCB1200_REG_INT_STATUS (4)
#define UCB1200_REG_TELECOM_CTL_A (5)
#define UCB1200_REG_TELECOM_CTL_B (6)
#define UCB1200_REG_AUDIO_CTL_A (7)
#define UCB1200_REG_AUDIO_CTL_B (8)
#define UCB1200_REG_TS_CTL (9)
#define UCB1200_REG_ADC_CTL (10)
#define UCB1200_REG_ADC_DATA (11)
#define UCB1200_REG_ID (12)
#define UCB1200_REG_MODE (13)

#define UCB1200_GPIO_CONT_CS      0x01
#define UCB1200_GPIO_CONT_DOWN    0x02
#define UCB1200_GPIO_CONT_INC     0x04
#define UCB1200_GPIO_CONT_ENA     0x08
#define UCB1200_GPIO_LCD_RESET    0x10

#define UCB1200_TS_CTL_TSMX_POW   0x01
#define UCB1200_TS_CTL_TSPX_POW   0x02
#define UCB1200_TS_CTL_TSMY_POW   0x04
#define UCB1200_TS_CTL_TSPY_POW   0x08
#define UCB1200_TS_CTL_TSMX_GND   0x10
#define UCB1200_TS_CTL_TSPX_GND   0x20
#define UCB1200_TS_CTL_TSMY_GND   0x40
#define UCB1200_TS_CTL_TSPY_GND   0x80
#define UCB1200_TS_CTL_MODE_POS   0x0200
#define UCB1200_TS_CTL_MODE_PRESS 0x0100
#define UCB1200_TS_CTL_BIAS_ENA   0x0800
#define UCB1200_TS_CTL_TSPX_LOW   0x1000

#define UCB1200_ADC_CTL_INPUT_TSPX 0x00
#define UCB1200_ADC_CTL_INPUT_TSMX 0x04
#define UCB1200_ADC_CTL_INPUT_TSPY 0x08
#define UCB1200_ADC_CTL_INPUT_TSMY 0x0C
#define UCB1200_ADC_CTL_START      0x80
#define UCB1200_ADC_CTL_ENA        0x8000

#define UCB1200_ADC_DAT_VAL       0x8000

void ucb1200_init(void);
void ucb1200_enable_lcd(void);
void ucb1200_contrast_function(int up);
void ucb1200_disable_lcd(void);

void ucb1200_activateX(void);
void ucb1200_activateY(void);
int ucb1200_measureX(void);
int ucb1200_measureY(void);

#endif //UCB1200_H
