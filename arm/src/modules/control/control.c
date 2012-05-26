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
	FP32 old_error[2],integral[2],derivative[2];
	FP32 goal = 0;
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

		//calculate error
		error[PAN] 		= setpoint[PAN] - feedback[PAN];	//format 90.0 degrees = 900
		error[TILT] 	= setpoint[TILT] - feedback[TILT];	//format 90.0 degrees = 900

		//check if goal was reached
		if(error[PAN] < GOAL && error[PAN] > -GOAL && error[TILT] < GOAL && error[TILT] > -GOAL)
			goal++;
		else
			goal = 0;

		//if in automode and setpoint was reached, change position
		if(state(POP,AUTO_MODE_S) && goal > HOLD_ON_GOAL)
		{
//			if(setpoint[0] == feedback[0] && setpoint[1] == feedback[1])
//			if(counter(POP,TIME_C) > 5)
//			if(error[PAN] < GOAL && error[PAN] > -GOAL && error[TILT] < GOAL && error[TILT] > -GOAL)
//			{
				goal = 0;
				counter(RESET,TIME_C);
				if(parameter(ADD,NEXT_POS_P,1) > NUMBER_OF_POSITIONS)
					parameter(PUSH,NEXT_POS_P,1);
				position(GOTO,parameter(POP,NEXT_POS_P));
				parameter(PUSH,PAN_PWM_P,0);
				parameter(PUSH,TILT_PWM_P,0);
				while(counter(POP,TIME_C) < 5)
				{
					red_led( TRUE );
					YIELD(100);
				}
				red_led( FALSE );

//			}
		}

		//calculate derivatives
		derivative[PAN] = (error[PAN] - old_error[PAN]);// / delta_time;
		derivative[TILT] = (error[TILT] - old_error[TILT]);// / delta_time;

		//calculate integrals
		if(error[PAN] < TRESHOLD && error[PAN] > -TRESHOLD)
			integral[PAN] = 0;
		else
			integral[PAN] += (error[PAN]);// * delta_time);

		if(error[TILT] < TRESHOLD && error[TILT] > -TRESHOLD)
			integral[TILT] = 0;
		else
			integral[TILT] += (error[TILT]);// * delta_time);

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
		input[PAN] 		= (error[PAN] * PAN_P_TERM)  + (integral[PAN] * I_TERM) - (derivative[PAN] * D_TERM);
		input[TILT] 	= (error[TILT] * TILT_P_TERM) + (integral[TILT] * I_TERM) - (derivative[TILT] * D_TERM);

		//zero input if inside treshold
		if(input[PAN] < TRESHOLD && input[PAN] > -TRESHOLD)
			input[PAN] 	= 0;
		if(input[TILT] < TRESHOLD && input[TILT] > -TRESHOLD)
			input[TILT] = 0;

		//bias
		if(input[PAN] > 0 )
			input[PAN] += BIAS;
		else if(input[PAN] < 0 )
			input[PAN] -= BIAS;
		if(input[TILT] > 0 )
			input[TILT] += BIAS;
		else if(input[TILT] < 0 )
			input[TILT] -= BIAS;

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
		parameter(PUSH,PAN_PWM_P,(INT32S)-input[PAN]);
		parameter(PUSH,TILT_PWM_P,(INT32S)input[TILT]);
		parameter(PUSH,PAN_CURRENT_P,(INT32S)feedback[PAN]);
		parameter(PUSH,TILT_CURRENT_P,(INT32S)feedback[TILT]);
		parameter(PUSH,PAN_ERROR_P,(INT32S)error[PAN]);
		parameter(PUSH,TILT_ERROR_P,(INT32S)error[TILT]);

		//save data for next run
		old_error[PAN] = error[PAN];
		old_error[TILT] = error[TILT];
		last_time = wake_time;

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
