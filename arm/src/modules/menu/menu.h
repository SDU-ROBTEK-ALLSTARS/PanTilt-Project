/*****************************************************************************
 * University of Southern Denmark
 * Pan Tilt Project
 *
 * MODULENAME.: menu
 *****************************************************************************/

/*****************************   Change Log   ********************************
* Date    Id    Change
* YYMMDD
* --------------------
* 120407  LBL   Module created
* 120420  LBL   Added function pointers
* 120421  LBL   Removed menu_handler
*
*****************************************************************************/

/***************************** Description *******************************
*usage:
* This module is used for controlling the system based on menus
*************************************************************************/

/*****************************    Pragma    *******************************/
#ifndef MENU_H_
#define MENU_H_

/***************************** Include files *******************************/
#include "maindefs.h"
#include "os.h"
#include "emp_type.h"
#include "inc/lm3s6965.h"
#include "itc/itc.h"
#include "lcd16x2/lcd.h"
#include "control/control.h"
#include "dreh/dreh.h"
#include "adc/adc.h"
#include "leds/leds.h"
#include "comm/uart.h"

/*****************************    Macros    *******************************/
#define SYSTEM_START_MENU		MAIN_MENU

#define NUMBER_OF_FIELDS 		4
#define NUMBER_OF_MENUS 		15
#define NUMBER_OF_DECIMALS		1
#define NUMBER_OF_DIGITS		5

#define INIT_FIELDS				for(i=0 ; i < NUMBER_OF_FIELDS ; i++)dummy->field[i].show = FALSE;
#define ADC_TO_DEG(x) 			(x*3600)/1024
#define DREH_TO_DEG(x)			x*100

//for menu_setup.c
#define SETUP(x)		dummy = menu_handler(x);
#define TOP(x)			dummy->topline = x;
#define BOTTOM(x)		dummy->bottomline = x;
#define NEXT(x)			dummy->next = x;
#define PREV(x)			dummy->previous = x;
#define ENTER(x)		dummy->enter = x;
#define TYPE(x)			dummy->type = x;
#define FIELD(n,a,b,sz) dummy->field[n].begin.x = a;dummy->field[n].begin.y = b;dummy->field[n].size = sz;
#define PARAM(n,x)		dummy->field[n].show =	TRUE;dummy->field[n].parameter = x;
#define BLINK(n)		dummy->field[n].blink =	TRUE;
#define ADC(n)			dummy->field[n].adc_input = TRUE;
#define DREH(n)			dummy->field[n].dreh_input = TRUE;
#define FUNCTION(x)		dummy->function = x;

/***************************    Enumerations    ****************************/
enum menu_names
{
	FIRST_RUN,
	MAIN_MENU,
	SET_MENU,
	SET_SUB_RETURN,
	SET_SUB_POS,
	SET_SUB_FREE,
	SET_SUB_AUTO,
	NOT_IMPLEMENTED,
	SET_POS_SUB,
	SET_FREE_RETURN,
	SET_FREE_STO,
	STEP_MENU,
	STEP_START,
	STEP_END

};

enum menu_types
{
	NOT_DEFINED,
	MENU,
	VIEW,
	INPUT,
	CALL
};

/*************************    Type definitions    **************************/
typedef struct coordinate_struct
{
	INT8U x;							//horizontal coordinate 0-15
	INT8U y;							//vertical coordinate 0-1
} coordinate_t;

typedef struct field_struct
{
	BOOLEAN show;						//only active fields are displayed
	BOOLEAN blink;						//field blinks
	BOOLEAN dreh_input;					//field takes input from dreh impuls geber
	BOOLEAN numpad_input;				//field takes input from numpad
	BOOLEAN adc_input;					//field takes input from onboard potmeter via A/D converter
	coordinate_t begin;					//coordinate of first character in field
	INT8U size;							//size of field
	enum parameter_handles parameter;	//parameter to show
} field_t;


typedef struct menu_struct
{
	INT8U menu_handle;					//name of menu
	struct menu_struct *next_handle;	//next menu in list
	enum menu_types type;				//type of menu, following menu_types enumeration
	const char *topline;				//top line of display
	const char *bottomline;				//bottom line of display
	INT8U next;							//pointer to next menu
	INT8U previous;						//pointer to previous menu
	INT8U enter;						//pointer to next level
	void ((*function)(void));			//function to call if type is CALL
	INT8U parameter;					//parameter to update if type is INPUT
	field_t field[NUMBER_OF_FIELDS];	//fields to show
} menu_t;

/**********************   Function declarations   **************************/

/**************************** INIT MENUS ************************************
*   Input    : 	-
*   Output   : 	-
*   Function :  initialises menus (This function is located in menu_setup.c)
*****************************************************************************/
void init_menus(void);


/**************************** MENU TASK **********************************
*   Input    : 	task parameters
*   Output   :  -
*   Function :  initialise menus and call display menu
*****************************************************************************/
void menu_task(void *pvParameters);


/**************************** MENU HANDLER *******************************
*   Input    : 	name of new menu
*   Output   : 	pointer to new menu
*   Function :  changes system state based on menu item
*****************************************************************************/
menu_t* menu_handler(INT8U handler);


/**************************** PARSE DREH EVENT *******************************
*   Input    : 	pointer to menu
*   Output   : 	pointer to menu
*   Function :  changes current menu based on dreh input
*****************************************************************************/
menu_t* parse_dreh_event(menu_t* menu);


//-----------------old stuff

/**************************** PARSE DREH **********************************
*   Input    : 	menu parameters and input array
*   Output   : 	next state
*   Function :  parses key input from input buffer, based on current menu
*****************************************************************************/
INT8U parse_numpad_input(INT8U active_menu, char *input_array, INT8U *array_pointer);

/**************************** PARSE BUTTONS **********************************
*   Input    : 	menu parameters
*   Output   : 	next state
*   Function :  parses button events from event handler, based on current menu
*****************************************************************************/
INT8U parse_buttons_input(INT8U out);

/*************************** STRING TO NUMBER *******************************
*   Input    : 	character string
*   Output   : 	number
*   Function :  parses a string of characters to a number
*****************************************************************************/
INT32S string_to_number(char *array);

/*************************** NUMBER TO STRING *******************************
*   Input    : 	character string to convert into, number to convert
*   Output   : 	-
*   Function :  parses from number to string of characters
*****************************************************************************/
void number_to_string(char *array, INT32S number);

/****************************** POWER OF TEN ********************************
*   Input    : 	integer
*   Output   : 	integer
*   Function :  returns ten raised to the power of input
*****************************************************************************/
INT32S power_of_ten(INT8U operand);

/****************************** PARSE NUMPAD ********************************
*   Input    : 	former value
*   Output   : 	new value
*   Function :  parses input from numpad to field
*****************************************************************************/
INT32S parse_numpad(INT32S former_value);

void red_only(void);
void green_only(void);
void begin_step(void);
void end_step(void);
#endif /* MENU_H_ */
