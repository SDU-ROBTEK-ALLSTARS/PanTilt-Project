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
	INT32S input[2]; //0 = pan voltage, 1 = tilt voltage
	INT32S setpoint[2]; //0 = wanted pan position, 1 = wanted tilt position
	float feedback[2]; //0 = feedback from pan, 1 = feedback from tilt
	INT32S error[2]; //setpoint - feedback
	portTickType xLastWakeTime;

	//define frequency of task
	const portTickType xFrequency = 100;

	//setup initial system parameters
	parameter(PUSH,PAN_SETPOINT_P,0);
	parameter(PUSH,TILT_SETPOINT_P,0);
	parameter(PUSH,PAN_PWM_P,0);
	parameter(PUSH,TILT_PWM_P,0);

	while( TRUE )
	{
		xLastWakeTime = xTaskGetTickCount();

		//get parameters - TODO: implement as burst
		setpoint[0] = parameter(POP,PAN_SETPOINT_P);
		setpoint[1] = parameter(POP,TILT_SETPOINT_P);

		feedback[0] = (float)parameter(POP,PAN_POSITION_P);
		feedback[1] = (float)parameter(POP,TILT_POSITION_P);


		//convert to human numbers
		conversions(feedback);

		//calculate error
		error[0] = setpoint[0] - feedback[0];
		error[1] = 1800 + (setpoint[1] - feedback[1]);

		if(error[0] > 100)
			input[0] = error[0]*P_TERM_PAN;
		else if(error[0] < -100)
			input[0] = error[0]*P_TERM_PAN;
		else
			input[0] = 0;

		if(error[1] > 150)
			input[1] = error[1]*P_TERM;
		else if(error[1] < -150)
			input[1] = error[1]*P_TERM;
		else
			input[1] = 0;

		//calculate new inputs
//		input[0] = (INT32S)( (error[0]*P_TERM));//* P_TERM;
//		input[1] = (INT32S)( (error[1]*P_TERM));//* P_TERM;

		//convert to pwm
//		if(input[0] < FPGA_PWM_MIN)
//			input[0] = FPGA_PWM_MIN;
//		if(input[0] > FPGA_PWM_MAX)
//			input[0] = FPGA_PWM_MAX;
//
//		if(input[1] < FPGA_PWM_MIN)
//			input[1] = FPGA_PWM_MIN;
//		if(input[1] > FPGA_PWM_MAX)
//			input[1] = FPGA_PWM_MAX;

//		parameter(PUSH,PAN_CURRENT_P,(INT32S)input[0]);
//		parameter(PUSH,TILT_CURRENT_P,(INT32S)input[1]);

//		parameter(PUSH,PAN_PWM_P,input[0]);
		parameter(PUSH,TILT_PWM_P,input[1]);
//		parameter(PUSH,TILT_PWM_P, -6000); // FIXME
//		parameter(PUSH,PAN_PWM_P, 0);

		vTaskDelayUntil( &xLastWakeTime, xFrequency );
//		YIELD(YIELD_TIME_CONTROL_T);
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
