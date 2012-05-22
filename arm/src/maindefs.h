
#ifndef MAINDEFS_H
#define MAINDEFS_H

/*****************************    Defines    *******************************/
#define 	DEFAULT_STACK  				( ( unsigned short ) 80 )
#define		LARGE_STACK					( ( unsigned short ) 180 )
#define 	NAME(x)						( signed portCHAR * )x
#define 	PRIORITY_HIGH				(tskIDLE_PRIORITY + 2)
#define 	PRIORITY_USER				(tskIDLE_PRIORITY + 1)
#define		PRIORITY_LOW				(tskIDLE_PRIORITY)

#define 	YIELD_TIME_DREH_T			10
#define		YIELD_TIME_LCD_T			2
#define 	YIELD_TIME_MENU_T			80
#define 	YIELD_TIME_CONTROL_T		5
#define		YIELD_TIME_BLINK_T			350
#define		YIELD_TIME_NUMPAD_T			40

enum task_handle_names
{
	DREH_T,
	LCD_T,
	MENU_T,
	NUMPAD_T,
	CONTROL_T,
	BLINK_T,
};

#endif /*MAINDEFS_H*/
