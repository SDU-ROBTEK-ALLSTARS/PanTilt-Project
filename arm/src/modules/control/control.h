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
#define 	TRESHOLD 	10
#define 	P_TERM 		0.1

/************************   Function declarations   ************************/

/*****************************************************************************
*   Input    :	-
*   Output   :	-
*   Function :	Task running the system control
******************************************************************************/
void control_task(void *pvParameters);


/****************************** End Of Module *******************************/
#endif
