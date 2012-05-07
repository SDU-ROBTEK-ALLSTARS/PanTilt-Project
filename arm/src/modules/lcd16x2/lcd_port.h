/*****************************************************************************
* University of Southern Denmark
* Robotics Engineering Spring 2012
* Pan Tilt Project
*
* MODULENAME.: adc
*
* DESCRIPTION: This module implements the A/D converter of L3S6965. Used with
* 			   EMP-board, it reads the onboard potentiometer
*
* Change Log:
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
* 120506  LBL   Cleaned up dependencies for binary.h
*
*****************************************************************************/
#ifndef _LCD_PORT_H
#define _LCD_PORT_H

/***************************** Include files *******************************/
#include "emp_type.h"

/***********************   Functions declarations   **************************/

/****************************  LCD PORT SETUP  *******************************
*   Input    : 	-
*   Output   : 	-
*   Function : 	LCD port setup.
******************************************************************************/
void lcd_port_setup(void);


/****************************  LCD DELAY µS  **********************************
*   Input    : 	delay in micro sec.
*   Output   : 	-
*   Function :	This function makes a delay.
******************************************************************************/
void lcd_delay_us(INT32U time);


/*************************  LCD WRITE 4BIT MODE  *****************************
*   Input    :	c: character.
*   Output   :	-
*   Function : 	Writes a 8 bit character to the LCD in 4 bit mode
******************************************************************************/
void lcd_write_4bit_mode(INT8U c);

/***************************  LCD WRITE PORT  ********************************
*   Input    :	c: character.
*   Output   :	-
*   Function : 	write a character to the LCD in 4 bit mode
******************************************************************************/
void lcd_write_port(INT8U value);

/*****************************  LCD RS LOW  **********************************
*   Input    :	-
*   Output   :	-
*   Function : 	clear rs
******************************************************************************/
void lcd_rs_low(void);

/*****************************  LCD RS HIGH **********************************
*   Input    :	-
*   Output   :	-
*   Function : 	set rs
******************************************************************************/
void lcd_rs_high(void);

/******************************  LCD STROBE  *********************************
*   Input    :	-
*   Output   :	-
*   Function : 	strobe display
******************************************************************************/
void lcd_strobe(void);

/****************************** End Of Module *******************************/
#endif
