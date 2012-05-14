/*****************************************************************************
 * University of Southern Denmark
 * Pan Tilt Project
 *
 * MODULENAME.: numpad
 *****************************************************************************/

/*****************************   Change Log   ********************************
* Date    Id    Change
* YYMMDD
* --------------------
* 120407  LBL   Module created
*
*****************************************************************************/

/***************************** Description *******************************/
#ifndef _KEYBOARD_H
  #define _KEYBOARD_H

/***************************** Include files *******************************/
#include "os.h"
#include "maindefs.h"
#include "inc/lm3s6965.h"
#include "glob_def.h"
#include "emp_type.h"
#include "itc/itc.h"

/*****************************    Defines    *******************************/
enum keyboard_states
{
	NOT_PRESSED,
	TEST_IF_PRESSED,
	PRESSED,
	TEST_IF_RELEASED
};

/*****************************   Variables   *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void numpad_task(void *pvParameters);
void key_port_init(void);
void select_x( INT8U x );
INT8U row( INT8U y );
INT8U key_catch(INT8U x, INT8U y );
void key_init(void);
/****************************** End Of Module *******************************/
#endif

