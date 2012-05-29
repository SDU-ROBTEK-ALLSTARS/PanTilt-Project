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
	FP32 	input[2],				//pwm range 5000 - 32000
	setpoint[2],					//format 90.0 degrees = 900
	feedback[2], 					//position in ticks
	error[2], 						//setpoint - feedback
	old_error[2],					//saved error from last entry
	integral[2],					//integration of errors
	derivative[2];					//change in error
	INT8U	goal = 0;				//counts number of entries at setpoint

	portTickType wake_time;
	const portTickType frequency = TASK_FREQUENCY;

	while( TRUE )
	{
		//save counter value
		wake_time = xTaskGetTickCount();

		//if in automode and setpoint was reached, shut down control task
		if(state(POP,AUTO_MODE_S) && goal > HOLD_ON_GOAL)
		{
			goal = 0;
			counter(RESET,TIME_C);
			parameter(PUSH,PAN_PWM_P,0);
			parameter(PUSH,TILT_PWM_P,0);
			state(PUSH,AUTO_WAIT_S,TRUE);
			deactivate_regulator();
			deactivate_automode();
		}
		else
		{
			//get parameters
			setpoint[PAN] 	= (FP32)parameter(POP,PAN_SETPOINT_P);
			setpoint[TILT] 	= (FP32)parameter(POP,TILT_SETPOINT_P);
			feedback[PAN] 	= (FP32)parameter(POP,PAN_POSITION_P);
			feedback[TILT] 	= (FP32)parameter(POP,TILT_POSITION_P);

			//convert feedback to degrees
			feedback[PAN] 	= TICKS_TO_DEGREES(feedback[PAN]);		//new format 90.0 degrees = 900
			feedback[TILT] 	= TICKS_TO_DEGREES(feedback[TILT]);		//new format 90.0 degrees = 900

			//calculate error
			error[PAN] 		= setpoint[PAN] - feedback[PAN];		//format 90.0 degrees = 900
			error[TILT] 	= setpoint[TILT] - feedback[TILT];		//format 90.0 degrees = 900

			//check if within goal area
			if(error[PAN] < GOAL && error[PAN] > -GOAL && error[TILT] < GOAL && error[TILT] > -GOAL)
				goal++;

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
			input[PAN] 		= (error[PAN] * PAN_P_TERM)  + (integral[PAN] * PAN_I_TERM) - (derivative[PAN] * PAN_D_TERM);
			input[TILT] 	= (error[TILT] * TILT_P_TERM) + (integral[TILT] * TILT_I_TERM) - (derivative[TILT] * TILT_D_TERM);

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
		}
		//yield
		vTaskDelayUntil( &wake_time, frequency );
	}
}
void activate_regulator(void)
{
	vTaskResume( task_handles[CONTROL_T] );
}

void deactivate_regulator(void)
{
	vTaskSuspend( task_handles[CONTROL_T] );
}
void activate_automode(void)
{
	state(PUSH,AUTO_MODE_S,TRUE);
}

void deactivate_automode(void)
{
	state(PUSH,AUTO_MODE_S,FALSE);
}
/****************************** End Of Module *******************************/
