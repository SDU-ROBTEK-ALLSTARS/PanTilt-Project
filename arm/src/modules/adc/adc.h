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
* 120506  LBL	Cleaned up Goofy code
*
*****************************************************************************/

/******************************** Pragma ************************************/
#ifndef _ADC_H
#define _ADC_H

/***************************** Include files *******************************/
#include "emp_type.h"
#include "inc/lm3s6965.h"

/*****************************    Macros    ********************************/
#define 	DUMMY_READ  	(dummy = SYSCTL_RCGC2_R)
#define 	ADC_CHANNEL 	0x01

/************************   Function declarations   *************************/

/******************************** INIT ADC *********************************
*   Input    : 	-
*   Output   : 	-
*   Function :  sets up A/D conversion to read onboard potentiometer
*****************************************************************************/
void init_adc(void);

/********************************* GET ADC **********************************
*   Input    : 	-
*   Output   : 	conversion value 0-1024
*   Function :  returns latest conversion and starts a new conversion
*****************************************************************************/
int get_adc(void);

#endif
