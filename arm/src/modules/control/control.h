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

/*****************************   Constants   *******************************/
#define 	PAN_P_TERM 				4
#define 	TILT_P_TERM 			2
#define 	I_TERM 					0
#define 	D_TERM 					0
#define 	TICK_PERIOD				1/configTICK_RATE_HZ
#define 	INTEGRAL_MAX			1000

#define		TASK_FREQUENCY			FREQUENCY(100) //frequency in Hz
#define		TASK_PERIOD				1/TASK_FREQUENCY
#define		FREQUENCY(x)			(configTICK_RATE_HZ/x)
#define		PWM_MIN					5000
#define		PWM_MAX					20000
#define		TRESHOLD				50

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
