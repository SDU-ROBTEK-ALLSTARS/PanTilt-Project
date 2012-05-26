/*****************************************************************************
* University of Southern Denmark
* Robotics Engineering Spring 2012
* Pan Tilt Project
*
* MODULENAME.: control
*
* DESCRIPTION: This module implements a simple control-like parsing between
*  			   setpoint values and current values
*
* Change Log:
* Date    Id    Change
* YYMMDD
* --------------------
* 120506  LBL   Module created.
*
*****************************************************************************/
#ifndef _CONTROL_H
#define _CONTROL_H

/***************************** Include files *******************************/
#include "emp_type.h"
#include "os.h"
#include "itc/itc.h"
#include "maindefs.h"
#include "leds/leds.h"

/*****************************   Constants   *******************************/
//optimized with respect to time
#define 	PAN_P_TERM 				30
#define 	TILT_P_TERM 			30
#define 	I_TERM 					2
#define 	D_TERM 					1
#define 	INTEGRAL_MAX			20000

//optimized with respect to precision
//#define 	PAN_P_TERM 				25
//#define 	TILT_P_TERM 			6
//#define 	I_TERM 					0.15
//#define 	D_TERM 					0.05
//#define 	INTEGRAL_MAX			10000

#define		TASK_FREQUENCY			FREQUENCY(100) //frequency in Hz
#define		TASK_PERIOD				1/TASK_FREQUENCY
#define		FREQUENCY(x)			(configTICK_RATE_HZ/x)
#define		PWM_MIN					6000
#define		PWM_MAX					30000
#define 	GOAL					2
#define		TRESHOLD				2
#define		BIAS					6000
#define 	HOLD_ON_GOAL			5

#define 	TICKS_PR_REVOLUTION		12*30*3 		//ticks pr motor revolution * gear ratio * belt ratio
#define		TICK_TO_DEGREE_FACTOR	3.34f
#define 	TICKS_ZERO				0x8000
#define		TICKS_TO_DEGREES(x)		(x - TICKS_ZERO) * TICK_TO_DEGREE_FACTOR

/*****************************   Enumerations   ****************************/
enum
{
	PAN,
	TILT,
};
/************************   Function declarations   ************************/

/*****************************************************************************
*   Input    :	-
*   Output   :	-
*   Function :	Task running the system control
******************************************************************************/
void control_task(void *pvParameters);


/****************************** End Of Module *******************************/
#endif
