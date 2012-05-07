/*****************************************************************************
* University of Southern Denmark
* Robotics Engineering Spring 2012
* Pan Tilt Project
*
* MODULENAME.: control
*
 *****************************************************************************/

/***************************** Include files *******************************/
#include "control.h"

/*****************************   Varables   *******************************/

/*****************************   Functions   *******************************/
void control_task(void *pvParameters)
{
	INT32S error[2];

	//setup initial system parameters
	parameter(PUSH,PAN_SETPOINT_P,2000);
	parameter(PUSH,PAN_CURRENT_P,0);

	parameter(PUSH,TILT_SETPOINT_P,0);
	parameter(PUSH,TILT_CURRENT_P,2000);

	while( TRUE )
	{
		//error = setpoint - feedback
		error[0] = parameter(POP,PAN_SETPOINT_P) - parameter(POP,PAN_CURRENT_P);
		error[1] = parameter(POP,TILT_SETPOINT_P) - parameter(POP,TILT_CURRENT_P);

		//regulate
		if(error[0] > TRESHOLD)
			parameter(ADD,PAN_CURRENT_P,(signed long)(error[0]*P_TERM));
		else if(error[0] < -TRESHOLD)
			parameter(ADD,PAN_CURRENT_P,(signed long)(error[0]*P_TERM));
		if(error[1] > TRESHOLD)
			parameter(ADD,TILT_CURRENT_P,(signed long)(error[1]*P_TERM));
		else if(error[1] < -TRESHOLD)
			parameter(ADD,TILT_CURRENT_P,(signed long)(error[1]*P_TERM));

		YIELD(YIELD_TIME_CONTROL_T);
	}
}



/****************************** End Of Module *******************************/
