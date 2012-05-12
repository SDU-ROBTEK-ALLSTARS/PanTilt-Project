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

#define  	PAN_FPGA_MAX		0x80D0
#define		PAN_FPGA_MIN		0x7F20
#define		TILT_FPGA_MAX		0x0434
#define		TILT_FPGA_MIN		0x0000
#define 	FPGA_PWM_MAX		0x8000
#define 	FPGA_PWM_MIN		0x7FFF
#define		FPGA_PWM_CENTER		0x8000
#define 	PAN_DEG_MAX			1800		//180.00 deg
#define		TILT_DEG_MAX		3600		//360.00 deg

/************************   Function declarations   ************************/

/*****************************************************************************
*   Input    :	-
*   Output   :	-
*   Function :	Task running the system control
******************************************************************************/
void control_task(void *pvParameters);

/*****************************************************************************
*   Input    :	feedback vector
*   Output   :	-
*   Function :	Converts feedback from FPGA
******************************************************************************/
void conversions(float *feedback);

/****************************** End Of Module *******************************/
#endif
