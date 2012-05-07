
#ifndef MAINDEFS_H
#define MAINDEFS_H

/*****************************    Defines    *******************************/
#define 	DEFAULT_STACK  				( ( unsigned short ) 80 )
#define		LARGE_STACK					( ( unsigned short ) 180 )
#define 	NAME(x)						( signed portCHAR * )x
#define 	PRIORITY_HIGH				3
#define 	PRIORITY_USER				2
#define		PRIORITY_LOW				1

#define 	YIELD_TIME_DREH_T			10
#define		YIELD_TIME_LCD_T			1
#define 	YIELD_TIME_MENU_T			5
#define 	YIELD_TIME_CONTROL_T		100
#define		YIELD_TIME_BLINK_T			350

enum task_handle_names
{
	DREH_T,
	LCD_T,
	MENU_T,
	NUMPAD_T,
	CONTROL_T,
	BLINK_T,

	FUELING_T,
	PWM_T,
	BUTTONS_T,
	RX_T,
	TX_T,
	HALL_T
};

#endif /*MAINDEFS_H*/
