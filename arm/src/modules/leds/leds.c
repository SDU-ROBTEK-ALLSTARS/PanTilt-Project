/*****************************************************************************
 * University of Southern Denmark
 * Embedded C Programming (ECP)
 *
 *
 * MODULENAME.: leds
 *****************************************************************************/

/***************************** Include files *******************************/
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "leds.h"

/*****************************    Defines    *******************************/
#define	PD6					6 // bit number
#define PG0					0 // bit number
#define PG1					1 // bit number

#define LED_RED			PD6
#define LED_YELLOW		PG0
#define LED_GREEN		PG1

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void init_leds(void)
{

	// Enable the GPIO port that is used for the EMP-board LEDS.
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOG;

	SysCtlDelay(3);

	// Enable the GPIO port that is used for the EMP-board LEDS.
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD;

	SysCtlDelay(3);

	// Set the direction as output (PD6).
	GPIO_PORTD_DIR_R |= 0x40;
	// Enable the GPIO pins for digital function (PD6).
	GPIO_PORTD_DEN_R |= 0x40;
	// Set output to default value.
	GPIO_PORTD_DATA_R &= ~(0x40);
	// Set the direction as output (PG0 and PG1).
	GPIO_PORTG_DIR_R |= 0x03;
	// Enable the GPIO pins for digital function (PG0 and PG1).
	GPIO_PORTG_DEN_R |= 0x03;
	// Set output to default value.
	GPIO_PORTG_DATA_R &= ~(0x03);
}



void red_led(BOOLEAN state)
{
	if (state)
	{
		GPIO_PORTD_DATA_R &= ~(1 << LED_RED);
	}
	else
	{
		GPIO_PORTD_DATA_R |= (1 << LED_RED);
	}
}


void yellow_led(BOOLEAN state)
{
	if (state)
	{
		GPIO_PORTG_DATA_R &= ~(1 << LED_YELLOW);
	}
	else
	{
		GPIO_PORTG_DATA_R |= (1 << LED_YELLOW);
	}
}

void green_led(BOOLEAN state)
{
	if (state)
	{
		GPIO_PORTG_DATA_R &= ~(1 << LED_GREEN);
	}
	else
	{
		GPIO_PORTG_DATA_R |= (1 << LED_GREEN);
	}
}


/****************************** End Of Module *******************************/
