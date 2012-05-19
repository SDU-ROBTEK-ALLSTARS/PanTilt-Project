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
#include "watchdog/watchdog.h"

/*****************************   Varables   *******************************/

/*****************************   Functions   *******************************/
void control_task(void *pvParameters)
{
	FP32 input[2];				//pwm range 5000 - 32000
	FP32 setpoint[2];			//setpoint in degrees
	FP32 feedback[2]; 			//position in ticks
	FP32 error[2]; 				//setpoint - feedback
	portTickType xLastWakeTime;

	//define frequency of task
	const portTickType xFrequency = TASK_FREQUENCY(100);		//frequency in Hz

	while( TRUE )
	{
		//save counter value
		xLastWakeTime = xTaskGetTickCount();

		//get parameters
		setpoint[PAN_POS] 	= (FP32)parameter(POP,PAN_SETPOINT_P);
		setpoint[TILT_POS] 	= (FP32)parameter(POP,TILT_SETPOINT_P);
		feedback[PAN_POS] 	= (FP32)parameter(POP,PAN_POSITION_P);
		feedback[TILT_POS] 	= (FP32)parameter(POP,TILT_POSITION_P);

		//convert feedback to degrees
		feedback[PAN_POS] 	= TICKS_TO_DEGREES(feedback[PAN_POS]);
		feedback[TILT_POS] 	= TICKS_TO_DEGREES(feedback[TILT_POS]);

		//calculate error
		error[PAN_POS] 		= setpoint[PAN_POS] - feedback[PAN_POS];
		error[TILT_POS] 	= setpoint[TILT_POS] - feedback[TILT_POS];

		//calculate inputs
		input[PAN_POS] 		= error[PAN_POS] * P_TERM;
		input[TILT_POS] 	= error[TILT_POS] * P_TERM;

		//zero input if under min to save motors
		if(input[PAN_POS] < PWM_MIN && input[PAN_POS] > -PWM_MIN)
			input[PAN_POS] 	= 0;
		if(input[TILT_POS] < PWM_MIN && input[TILT_POS] > -PWM_MIN)
			input[TILT_POS] = 0;

		//reduce input to max
		if(input[PAN_POS] > PWM_MAX)
			input[PAN_POS] = PWM_MAX;
		else if (input[PAN_POS] < -PWM_MAX)
			input[PAN_POS] = -PWM_MAX;
		if(input[TILT_POS] > PWM_MAX)
			input[TILT_POS] = PWM_MAX;
		else if (input[TILT_POS] < -PWM_MAX)
			input[TILT_POS] = -PWM_MAX;

		//update parameters
//		parameter(PUSH,TILT_PWM_P,(INT32S)input[PAN_POS]);
		parameter(PUSH,TILT_PWM_P,(INT32S)input[TILT_POS]);
		parameter(PUSH,PAN_CURRENT_P,(INT32S)feedback[0]);
		parameter(PUSH,TILT_CURRENT_P,(INT32S)feedback[1]);

		//yield
		wd_kick_from_task(); /* Kick the dog to let it know the task is running */
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

/****************************** End Of Module *******************************/
