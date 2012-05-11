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
	INT32S feedback[2]; //0 = feedback from pan, 1 = feedback from tilt
	INT32S error[2]; //setpoint - feedback

	//setup initial system parameters
	parameter(PUSH,PAN_SETPOINT_P,0);
	parameter(PUSH,TILT_SETPOINT_P,0);

	while( TRUE )
	{
		//get parameters - TODO: implement as burst
		setpoint[0] = parameter(POP,PAN_SETPOINT_P);
		setpoint[1] = parameter(POP,TILT_SETPOINT_P);
		feedback[0] = parameter(POP,PAN_CURRENT_P);
		feedback[1] = parameter(POP,TILT_CURRENT_P);

		//convert to human numbers
		conversions(feedback);

		//calculate error
		error[0] = setpoint[0] - feedback[0];
		error[1] = setpoint[1] - feedback[1];

		//calculate new inputs
		input[0] = error[0] * P_TERM;
		input[1] = error[1] * P_TERM;

		//convert to pwm
		if(input[0] < FPGA_PWM_MIN)
			input[0] = FPGA_PWM_MIN;
		if(input[0] > FPGA_PWM_MAX)
			input[0] = FPGA_PWM_MAX;

		if(input[1] < FPGA_PWM_MIN)
			input[1] = FPGA_PWM_MIN;
		if(input[1] > FPGA_PWM_MAX)
			input[1] = FPGA_PWM_MAX;

		parameter(PUSH,PAN_PWM_P,input[0]);
		parameter(PUSH,PAN_PWM_P,input[1]);

		YIELD(YIELD_TIME_CONTROL_T);
	}
}

void conversions(INT32S *feedback)
{
	BOOLEAN sign = 0;
	if(feedback[0] < 0x8000)
	{
		feedback[0] = 0x8000 - feedback[0];
		sign = 1;
	}

	feedback[0] = sign ? (	feedback[0] / PAN_FPGA_MAX ) * PAN_DEG_MAX: (	feedback[0] / PAN_FPGA_MAX ) * PAN_DEG_MAX;
	feedback[1] = (	feedback[0]	/ TILT_FPGA_MAX ) * TILT_DEG_MAX;
}

/****************************** End Of Module *******************************/
