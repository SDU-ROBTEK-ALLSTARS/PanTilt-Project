/*****************************************************************************
* University of Southern Denmark
* Robotics Engineering Spring 2012
* Pan Tilt Project
*
* MODULENAME.: dreh
*
 ****************************************************************************/

/***************************** Include files *******************************/
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "dreh.h"

/***********************   Function implementations   **********************/
void dreh_task(void *pvParameters)
{
	INT8U old_A = 0;
	/* INT8U old_B = 0; KIM fixes "warning: variable 'old_B' set but not used [-Wunused-but-set-variable]" */
	INT8U new_A = 0;
	INT8U new_B = 0;
	INT8U button = IDLE;
	INT8U dreh_state = 0;

	while(TRUE)
	{
		dreh_state = state(POP,DREH_S);
		new_A = DIGI_A;
		new_B = DIGI_B;

		switch(button)
		{
		case IDLE:
			if(!DIGI_P2)
			{
				button = PUSHED;
				event(PUSH,DREH_E,DREH_PUSH);
			}
			break;

		case PUSHED:
			if(DIGI_P2)
			{
				button = IDLE;
			}
			break;
		}

		if(new_A != old_A)
		{
			if(new_A)			//if rising edge on A
			{
				if(new_B)		//if B is high
				{
					if(dreh_state == EVENT)
						event(PUSH,DREH_E,DREH_CCW);
					if(dreh_state == COUNT)
						counter(DECREMENT,DREH_C);
				}
				else			//if B is low
				{
					if(dreh_state == EVENT)
						event(PUSH,DREH_E,DREH_CW);
					if(dreh_state == COUNT)
						counter(INCREMENT,DREH_C);
				}
			}
			else				//if falling edge on A
				if(new_B)		//if B is high
				{
					if(dreh_state == EVENT)
						event(PUSH,DREH_E,DREH_CW);
					if(dreh_state == COUNT)
						counter(INCREMENT,DREH_C);
				}
				else			//if B is low
				{
					if(dreh_state == EVENT)
						event(PUSH,DREH_E,DREH_CCW);
					if(dreh_state == COUNT)
						counter(DECREMENT,DREH_C);
				}
		}
		old_A = new_A;
		/* old_B = new_B; KIM fixes "warning: variable 'old_B' set but not used [-Wunused-but-set-variable]" */

		YIELD(YIELD_TIME_DREH_T)
	}
}

void init_dreh(void)
{
	SYSCTL_RCGC2_R    |= SYSCTL_RCGC2_GPIOB | SYSCTL_RCGC2_GPIOC;

  SysCtlDelay(3);

	GPIO_PORTB_DIR_R  &= ~B1;
	GPIO_PORTC_DIR_R  &= ~(B5 | B4);

	GPIO_PORTB_DEN_R  |= B1;
	GPIO_PORTC_DEN_R  |= (B5 | B4);

	GPIO_PORTC_PUR_R  |= B5;
}

/****************************** End Of Module ******************************/
