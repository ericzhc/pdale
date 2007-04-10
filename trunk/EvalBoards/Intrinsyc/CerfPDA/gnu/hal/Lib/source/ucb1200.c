//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2003 Universite de Sherbrooke All rights reserved.
//
// Module name:
//
//      ucb1200.c
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

#include <ucb1200.h>
#include <mcp_sa.h>
#include <delay.h>
#include <types.h>

static int counter_contrast = 1;
static int UP = 1;
static int DOWN = 0;

////////////////////////////////////////////////////////////////////////////////
// ucb1200_init
// PURPOSE: Initializes the UCB1200 chip.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
ucb1200_init(void)
{
	mcp_write_codec_register(UCB1200_REG_IO_DIRECTION, mcp_read_codec_register(UCB1200_REG_IO_DIRECTION)
		| 0x001f);
}

////////////////////////////////////////////////////////////////////////////////
// ucb1200_enable_lcd
// PURPOSE: Enable the LCD panel.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
ucb1200_enable_lcd(void)
{

	mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA)
		| UCB1200_GPIO_CONT_ENA);

	for(counter_contrast = MAX_CONTRAST; counter_contrast >= 0; counter_contrast--)
		ucb1200_contrast_function(DOWN);
	for(counter_contrast = 0; counter_contrast <= DEFAULT_CONTRAST; counter_contrast++)               
		ucb1200_contrast_function(UP);

	mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA)
		| UCB1200_GPIO_LCD_RESET);
}

////////////////////////////////////////////////////////////////////////////////
// ucb1200_contrast_function
// PURPOSE: Set the LCD contrast Up or Down.
// PARAMS:  (IN) up = 1 for up, up = 0 for down.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
ucb1200_contrast_function(int up)
{
   if(up)
   {
      // Assert the chip select and the up modifier
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) | (UCB1200_GPIO_CONT_DOWN));
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(UCB1200_GPIO_CONT_CS));
      
      // Assert the line up, down then up again
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(UCB1200_GPIO_CONT_INC));
      udelay(1);
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) | (UCB1200_GPIO_CONT_INC));
      udelay(1);
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(UCB1200_GPIO_CONT_INC));
      
      // Deassert the chip select and the up modifier
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(UCB1200_GPIO_CONT_DOWN));
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) | (UCB1200_GPIO_CONT_CS));
   }
   else
   {
      // Assert the chip select and the up modifier
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(UCB1200_GPIO_CONT_CS|UCB1200_GPIO_CONT_DOWN));
      
      // Assert the line up, down then up again
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(UCB1200_GPIO_CONT_INC));
      udelay(1); 
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) | (UCB1200_GPIO_CONT_INC));        
      udelay(1); 
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(UCB1200_GPIO_CONT_INC));
      
      // Deassert the chip select and the up modifier
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) & ~(UCB1200_GPIO_CONT_DOWN));
      mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA) | (UCB1200_GPIO_CONT_CS));
   }

}

////////////////////////////////////////////////////////////////////////////////
// ucb1200_disable_lcd
// PURPOSE: Disable the LCD panel.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
ucb1200_disable_lcd(void)
{

	mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA)
		& ~(UCB1200_GPIO_LCD_RESET));

	mcp_write_codec_register(UCB1200_REG_IO_DATA, mcp_read_codec_register(UCB1200_REG_IO_DATA)
		& ~(UCB1200_GPIO_CONT_ENA));
}

////////////////////////////////////////////////////////////////////////////////
// ucb1200_activateX
// PURPOSE: Activate the measurement of the X-axis on the touch panel.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
ucb1200_activateX(void)
{
	mcp_write_codec_register(UCB1200_REG_TS_CTL, UCB1200_TS_CTL_TSPX_POW | UCB1200_TS_CTL_TSMX_POW
		| UCB1200_TS_CTL_TSPY_GND | UCB1200_TS_CTL_TSMY_GND);
	mcp_write_codec_register(UCB1200_REG_TS_CTL, UCB1200_TS_CTL_TSPX_POW | UCB1200_TS_CTL_TSMX_GND
		| UCB1200_TS_CTL_MODE_PRESS | UCB1200_TS_CTL_BIAS_ENA);
	mcp_read_codec_register(UCB1200_REG_TS_CTL);
	mcp_write_codec_register(UCB1200_REG_TS_CTL, UCB1200_TS_CTL_TSPX_POW | UCB1200_TS_CTL_TSMX_GND
		| UCB1200_TS_CTL_MODE_POS | UCB1200_TS_CTL_BIAS_ENA);
	mcp_write_codec_register(UCB1200_REG_ADC_CTL, UCB1200_ADC_CTL_INPUT_TSPY
		| UCB1200_ADC_CTL_ENA);
	udelay(50);
	mcp_write_codec_register(UCB1200_REG_ADC_CTL, UCB1200_ADC_CTL_INPUT_TSPY
		| UCB1200_ADC_CTL_ENA | UCB1200_ADC_CTL_START);
	mcp_write_codec_register(UCB1200_REG_ADC_CTL, UCB1200_ADC_CTL_INPUT_TSPY
		| UCB1200_ADC_CTL_ENA);
}

////////////////////////////////////////////////////////////////////////////////
// ucb1200_activateY
// PURPOSE: Activate the measurement of the Y-axis on the touch panel.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
ucb1200_activateY(void)
{
	mcp_write_codec_register(UCB1200_REG_TS_CTL, UCB1200_TS_CTL_TSPX_GND | UCB1200_TS_CTL_TSMX_GND
		| UCB1200_TS_CTL_TSPY_POW | UCB1200_TS_CTL_TSMY_POW);
	mcp_write_codec_register(UCB1200_REG_TS_CTL, UCB1200_TS_CTL_TSPY_POW | UCB1200_TS_CTL_TSMY_GND
		| UCB1200_TS_CTL_MODE_PRESS | UCB1200_TS_CTL_BIAS_ENA);
	mcp_read_codec_register(UCB1200_REG_TS_CTL);
	mcp_write_codec_register(UCB1200_REG_TS_CTL, UCB1200_TS_CTL_TSPY_POW | UCB1200_TS_CTL_TSMY_GND
		| UCB1200_TS_CTL_MODE_POS | UCB1200_TS_CTL_BIAS_ENA);
	mcp_write_codec_register(UCB1200_REG_ADC_CTL, UCB1200_ADC_CTL_INPUT_TSPX
		| UCB1200_ADC_CTL_ENA);
	udelay(50);
	mcp_write_codec_register(UCB1200_REG_ADC_CTL, UCB1200_ADC_CTL_INPUT_TSPX
		| UCB1200_ADC_CTL_ENA | UCB1200_ADC_CTL_START);
	mcp_write_codec_register(UCB1200_REG_ADC_CTL, UCB1200_ADC_CTL_INPUT_TSPX
		| UCB1200_ADC_CTL_ENA);
}

////////////////////////////////////////////////////////////////////////////////
// ucb1200_measureX
// PURPOSE: Measure the X-axis ADC value on the touch panel.
// PARAMS:  None.
// RETURNS: Measurement value for the X-axis 
////////////////////////////////////////////////////////////////////////////////
int
ucb1200_measureX(void)
{

	u16 data;
	while (!((data = mcp_read_codec_register(UCB1200_REG_ADC_DATA)) & UCB1200_ADC_DAT_VAL));
	data = (data >> 5) & 0x03ff;
	mcp_write_codec_register(UCB1200_REG_ADC_CTL, 0);

	return (int) data;
}

////////////////////////////////////////////////////////////////////////////////
// ucb1200_measureY
// PURPOSE: Measure the Y-axis ADC value on the touch panel.
// PARAMS:  None.
// RETURNS: Measurement value for the Y-axis 
////////////////////////////////////////////////////////////////////////////////
int
ucb1200_measureY(void)
{

	u16 data;
	while (!((data = mcp_read_codec_register(UCB1200_REG_ADC_DATA)) & UCB1200_ADC_DAT_VAL));
	data = (data >> 5) & 0x03ff;
	mcp_write_codec_register(UCB1200_REG_ADC_CTL, 0);

	return (int) data;
}
