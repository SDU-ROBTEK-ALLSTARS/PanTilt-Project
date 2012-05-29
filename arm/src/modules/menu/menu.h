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
* 120528  LBL	Added function descriptions
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
#define SYSTEM_START_MENU		FUNCTIONS_M_REG

#define NUMBER_OF_FIELDS 		4
#define NUMBER_OF_MENUS 		50
#define NUMBER_OF_DECIMALS		1

#define ADC_TO_DEG(x) 			((x * 1800) / 1024) - 900
#define DREH_TO_DEG(x)			x * 100
#define CHAR_TO_NUMBER(x)		x - '0'

//for menu_setup.c
#define INIT_FIELDS			for(i=0 ; i < NUMBER_OF_FIELDS ; i++)dummy->field[i].show = FALSE;
#define SETUP(x)			dummy = menu_handler(x);INIT_FIELDS;
#define MENU_TEXT(m,t,b)	menu_texts[m].topline = t;menu_texts[m].bottomline = b;
#define TEXT(m)				dummy->text = (menu_texts[m]);
#define NEXT(x)				dummy->next = x;
#define PREV(x)				dummy->previous = x;
#define ENTER(x)			dummy->enter = x;
#define TYPE(m,i,c)			dummy->type.is_menu = m;dummy->type.is_input = i;dummy->type.is_call = c;
#define FIELD(n,a,b,sz) 	dummy->field[n].begin.x = a;dummy->field[n].begin.y = b;dummy->field[n].size = sz;
#define PARAM(n,x)			dummy->field[n].show =	TRUE;dummy->field[n].parameter = x;
#define BLINK(n)			dummy->field[n].blink =	TRUE;
#define ADC(n)				dummy->field[n].adc_input = TRUE;
#define DREH(n)				dummy->field[n].dreh_input = TRUE;
#define NUMPAD(n)			dummy->field[n].numpad_input = TRUE;
#define FUNCTION(x)			dummy->function = x;

/***************************    Enumerations    ****************************/
enum menu_names
{
	FIRST_RUN_M,

	FUNCTIONS_M,
	FUNCTIONS_M_REG,
	FUNCTIONS_M_FREE,
	FUNCTIONS_M_AUTO,
	FUNC_REG_M,
	FUNC_REG_ON_M,
	FUNC_FREE_M,
	FUNC_FREE_RET_M,
	FUNC_FREE_RET_CALL_M,
	FUNC_FREE_STO_M,
	FUNC_FREE_SAVED_M,
	FUNC_FREE_M_STO,
	FUNC_AUTO_M,
	FUNC_AUTO_ON_M,

	SETTINGS_M,
	SETTINGS_M_REG,
	SETTINGS_M_AUTO,
	SETTINGS_M_POS,
	SET_REG_M,
	SET_REG_TYPE_M,
	SET_REG_PARAMETERS_M,
	SET_REG_PARAM_M,
	SET_REG_TYPE_PID_M,
	SET_REG_TYPE_SS_M,
	SET_AUTO_M,
	SET_POS_M,

	INFO_M,
	INFO_M_POS,
	INFO_M_ERROR,
	INFO_POS_M,
	INFO_ERROR_M,

	NOT_IMPLEMENTED

};

enum menu_types
{
 IS_MENU = 1,
 NOT_MENU = 0,
 IS_INPUT = 1,
 NOT_INPUT = 0,
 IS_CALL = 1,
 NOT_CALL = 0
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

typedef struct text_struct
{
	const char *topline;				//top line of display
	const char *bottomline;				//bottom line of display
} text_t;

typedef struct type_struct
{
	BOOLEAN is_menu;
	BOOLEAN is_input;
	BOOLEAN is_call;

} type_t;

typedef struct menu_struct
{
	INT8U menu_handle;					//name of menu
	struct menu_struct *next_handle;	//next menu in list
	struct text_struct text;			//display text
	struct type_struct type;			//type of menu
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

/**************************** ENTER FREEMODE ********************************
*   Input    : 	-
*   Output   : 	-
*   Function :  Activates freemode
*****************************************************************************/
void enter_freemode(void);

/***************************** EXIT FREEMODE ********************************
*   Input    : 	-
*   Output   : 	-
*   Function :  Deactivates freemode
*****************************************************************************/
void exit_freemode(void);

/***************************** ACTIVATE PID *********************************
*   Input    : 	-
*   Output   : 	-
*   Function :  Activates PID regulator and deactivates state space regulator
*****************************************************************************/
void activate_pid(void);

/***************************** ACTIVATE SS *********************************
*   Input    : 	-
*   Output   : 	-
*   Function :  Activates state space regulator and deactivates PID regulator
*****************************************************************************/
void activate_ss(void);

/***************************** ACTIVATE SS *********************************
*   Input    : 	-
*   Output   : 	-
*   Function :  Saves current position
*****************************************************************************/
void save_position(void);

#endif /* MENU_H_ */
