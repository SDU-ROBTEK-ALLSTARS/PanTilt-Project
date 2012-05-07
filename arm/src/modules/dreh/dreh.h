/*****************************************************************************
* University of Southern Denmark
* Robotics Engineering Spring 2012
* Pan Tilt Project
*
* MODULENAME.: dreh
*
*
* DESCRIPTION:	This module implements the dreh impuls geber on the EMP-board.
*
*
* Change Log:
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  MS    Module created.
* 120420  LBL   Cleaned up Goofy code
*
****************************************************************************/
#ifndef DREH_H
#define DREH_H

/***************************** Include files *******************************/
#include "maindefs.h"
#include "os.h"
#include "emp_type.h"
#include "inc/lm3s6965.h"
#include "itc/itc.h"

/******************************    Macros    *******************************/
#define DIGI_B				(GPIO_PORTB_DATA_R & B1)
#define DIGI_A				(GPIO_PORTC_DATA_R & B4)
#define DIGI_P2				(GPIO_PORTC_DATA_R & B5)

#define B0 0x01
#define B1 0x02
#define B2 0x04
#define B3 0x08
#define B4 0x10
#define B5 0x20
#define B6 0x40
#define B7 0x80

/***************************    Enumerations    ****************************/
enum dreh_event_values
{
	IDLE,
	DREH_CW,
	DREH_CCW,
	DREH_PUSH,
	PUSHED
};

enum dreh_modes
{
	UNDEFINED,
	EVENT,
	COUNT
};

/**********************   Function declarations   **************************/

/****************************** DREH TASK **********************************
*   Input    : 	-
*   Output   : 	-
*   Function :  task that parses input from dreh impuls geber to event buffer
*   			or counter based on DREH_S state
*****************************************************************************/
void dreh_task(void *pvParameters);

/****************************** INIT DREH **********************************
*   Input    : 	-
*   Output   : 	-
*   Function :  initializes dreh impuls geber
*****************************************************************************/
void init_dreh(void);


/****************************** End Of Module ******************************/
#endif
