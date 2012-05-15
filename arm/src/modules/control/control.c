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
	INT32S input[2];			//pwm range 5000 - 32000
	INT32S setpoint[2];			//setpoint in degrees
	float feedback[2]; 			//position in ticks
	INT32S error[2]; 			//setpoint - feedback
	portTickType xLastWakeTime;

	//define frequency of task
	const portTickType xFrequency = 100;		//Hz

	while( TRUE )
	{
		xLastWakeTime = xTaskGetTickCount();

		//get parameters
		setpoint[PAN] 	= parameter(POP,PAN_SETPOINT_P);
		setpoint[TILT] 	= parameter(POP,TILT_SETPOINT_P);

		feedback[PAN] 	= (float)parameter(POP,PAN_POSITION_P);
		feedback[TILT] 	= (float)parameter(POP,TILT_POSITION_P);

		//convert feedback to degrees
		conversions(feedback);

		//calculate error
		error[PAN] = setpoint[PAN] - feedback[PAN];
		error[TILT] = 1800 + (setpoint[TILT] - feedback[TILT]);

		if(error[PAN] > 100)
			input[PAN] = error[PAN]*P_TERM_PAN;
		else if(error[PAN] < -100)
			input[PAN] = error[PAN]*P_TERM_PAN;
		else
			input[PAN] = 0;

		if(error[TILT] > 150)
			input[TILT] = error[TILT]*P_TERM;
		else if(error[TILT] < -150)
			input[TILT] = error[TILT]*P_TERM;
		else
			input[TILT] = 0;

//		parameter(PUSH,PAN_PWM_P,input[PAN]);
		parameter(PUSH,TILT_PWM_P,input[TILT]);

		vTaskDelayUntil( &xLastWakeTime, xFrequency );

	}
}

void conversions(float *feedback)
{
	feedback[0] -= 0x8000;
  	feedback[1] -= 0x8000;

  	feedback[0] *= 3.34f;
  	feedback[1] *= 3.34f;

	//update current angles
		parameter(PUSH,PAN_CURRENT_P,(INT32S)feedback[0]);
		parameter(PUSH,TILT_CURRENT_P,(INT32S)feedback[1]);
}

/****************************** End Of Module *******************************/
