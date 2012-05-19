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
	FP32 input[2];				//pwm range 5000 - 32000
	FP32 setpoint[2];			//format 90.0 degrees = 900
	FP32 feedback[2]; 			//position in ticks
	FP32 error[2]; 				//setpoint - feedback
	FP32 old_error[2],integral[2],derivative[2],delta_time;
	portTickType wake_time,last_time;
	const portTickType frequency = TASK_FREQUENCY;

	while( TRUE )
	{
		//save counter value
		wake_time = xTaskGetTickCount();

		//get parameters
		setpoint[PAN] 	= (FP32)parameter(POP,PAN_SETPOINT_P);
		setpoint[TILT] 	= (FP32)parameter(POP,TILT_SETPOINT_P);
		feedback[PAN] 	= (FP32)parameter(POP,PAN_POSITION_P);
		feedback[TILT] 	= (FP32)parameter(POP,TILT_POSITION_P);

		//convert feedback to degrees
		feedback[PAN] 	= TICKS_TO_DEGREES(feedback[PAN]);		//new format 90.0 degrees = 900
		feedback[TILT] 	= TICKS_TO_DEGREES(feedback[TILT]);		//new format 90.0 degrees = 900

		//calculate time period
		delta_time = (wake_time - last_time) * TICK_PERIOD;

		//calculate error
		error[PAN] 		= setpoint[PAN] - feedback[PAN];	//format 90.0 degrees = 900
		error[TILT] 	= setpoint[TILT] - feedback[TILT];	//format 90.0 degrees = 900

		//calculate derivatives
		derivative[PAN] = (error[PAN] - old_error[PAN]) / delta_time;
		derivative[TILT] = (error[TILT] - old_error[TILT]) / delta_time;

		//calculate integrals
		integral[PAN] += error[PAN] * delta_time;
		integral[TILT] += error[TILT] * delta_time;

		//check for integral saturation
		if(integral[PAN] > INTEGRAL_MAX)
			integral[PAN] = INTEGRAL_MAX;
		if(integral[PAN] < -INTEGRAL_MAX)
			integral[PAN] = -INTEGRAL_MAX;
		if(integral[TILT] > INTEGRAL_MAX)
			integral[TILT] = INTEGRAL_MAX;
		if(integral[TILT] < -INTEGRAL_MAX)
			integral[TILT] = -INTEGRAL_MAX;

		//calculate inputs
		input[PAN] 		= (error[PAN] * P_TERM) + (integral[PAN] * I_TERM) + (derivative[PAN] * D_TERM);
		input[TILT] 	= (error[TILT] * P_TERM) + (integral[TILT] * I_TERM) + (derivative[TILT] * D_TERM);

		//zero input if under min to save motors
		if(input[PAN] < PWM_MIN && input[PAN] > -PWM_MIN)
			input[PAN] 	= 0;
		if(input[TILT] < PWM_MIN && input[TILT] > -PWM_MIN)
			input[TILT] = 0;

		//reduce input to max
		if(input[PAN] > PWM_MAX)
			input[PAN] = PWM_MAX;
		else if (input[PAN] < -PWM_MAX)
			input[PAN] = -PWM_MAX;
		if(input[TILT] > PWM_MAX)
			input[TILT] = PWM_MAX;
		else if (input[TILT] < -PWM_MAX)
			input[TILT] = -PWM_MAX;

		//update parameters
		parameter(PUSH,TILT_PWM_P,(INT32S)input[PAN]);
		parameter(PUSH,TILT_PWM_P,(INT32S)input[TILT]);
		parameter(PUSH,PAN_CURRENT_P,(INT32S)feedback[PAN]);
		parameter(PUSH,TILT_CURRENT_P,(INT32S)feedback[TILT]);

		//save data for next run
		old_error[PAN] = error[PAN];
		old_error[TILT] = error[TILT];
		last_time = wake_time;

		//if in automode and setpoint was reached, change position
		if(state(POP,AUTO_MODE_S))
			if(setpoint[0] == feedback[0] && setpoint[1] == feedback[1])
			{
				if(parameter(ADD,NEXT_POS_P,1) > NUMBER_OF_POSITIONS)
					parameter(PUSH,NEXT_POS_P,0);
				position(GOTO,parameter(POP,NEXT_POS_P));
			}

		//yield
		vTaskDelayUntil( &wake_time, frequency );
	}
}

////testing
//while( TRUE )
//{
//	//simulating regulator
//	setpoint[0] = (FP32)parameter(POP,PAN_SETPOINT_P);
//	setpoint[1] = (FP32)parameter(POP,TILT_SETPOINT_P);
//	feedback[0] = (FP32)parameter(POP,PAN_CURRENT_P);
//	feedback[1] = (FP32)parameter(POP,TILT_CURRENT_P);
//
//	if(setpoint[0] > (feedback[0] + 50))
//		feedback[0] += 50;
//	else if(setpoint[0] < (feedback[0] - 50))
//		feedback[0] -= 50;
//	else
//		feedback[0] = setpoint[0];
//
//	if(setpoint[1] > (feedback[1] + 50))
//		feedback[1] += 50;
//	else if(setpoint[1] < (feedback[1] - 50))
//		feedback[1] -= 50;
//	else
//		feedback[1] = setpoint[0];
//
//	parameter(PUSH,PAN_CURRENT_P,(INT32S)feedback[0]);
//	parameter(PUSH,TILT_CURRENT_P,(INT32S)feedback[1]);
//	YIELD(500);
//}
/****************************** End Of Module *******************************/
