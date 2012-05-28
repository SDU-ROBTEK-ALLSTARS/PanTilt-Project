/*****************************************************************************
* University of Southern Denmark
* Robotics Engineering Spring 2012
* Pan Tilt Project
*
* MODULENAME.: itc
*
* DESCRIPTION: This module is used for inter task communication
*
* Change Log:
* Date    Id    Change
* YYMMDD
* --------------------
* 120407  LBL   Module created
* 120506  LBL   Updated module for pan tilt application
*
*****************************************************************************/

/*****************************    Pragma    *******************************/
#ifndef _DATA_H
#define _DATA_H

/***************************** Include files *******************************/
#include "emp_type.h"
#include "glob_def.h"
#include "os.h"

/*****************************    Defines    *******************************/
#define QUEUE_SIZE 				10

#define NUMBER_OF_QUEUES 		5
#define NUMBER_OF_EVENTS 		5
#define NUMBER_OF_COUNTERS		5
#define NUMBER_OF_STATES 		10
#define NUMBER_OF_VARIABLES 	20
#define NUMBER_OF_PARAMETERS	20
#define NUMBER_OF_POSITIONS		3

/***************************    Enumerations    ****************************/
enum data_commands
{
	RESET,
	PUSH,
	POP,
	INCREMENT,
	DECREMENT,
	NEW,
	ADD,
	ADD_POS,
	SUB,
	SEARCH,
	TOGGLE,
	FLUSH,
	PEEK,
	SAVE,
	GOTO
} ;

enum event_handles
{
	IDLE_E,
	DREH_E,
	NUMPAD_E
};

enum counter_handles
{
	IDLE_C,
	DREH_C,
	TIME_C
} ;

enum state_handles
{
	IDLE_S,
	DREH_S,
	BLINK_S,
	AUTO_MODE_S,
	AUTO_WAIT_S,
	FREE_MODE_S,
};

enum parameter_handles
{
  IDLE_P,
  PAN_CURRENT_P,
  TILT_CURRENT_P,
  PAN_SETPOINT_P,
  TILT_SETPOINT_P,
  PAN_POSITION_P,
  TILT_POSITION_P,
  PAN_ERROR_P,
  TILT_ERROR_P,
  PAN_PWM_P,
  TILT_PWM_P,
  PAN_VELOCITY_P,
  TILT_VELOCITY_P,
  FREE_P,
  SAVE_POS_P,
  NEXT_POS_P,
};

typedef struct position_struct
{
	struct position_struct *next;
	INT8U number;
	INT32S pan;
	INT32S tilt;
} position_t;

/**********************   Function declarations   **************************/

/***************************** PARAMETER ***********************************
*   Input    : 	PUSH, POP or ADD, name of parameter to access, third argument
*   			only used if pushing
*   Output   : 	popped value or zero if empty
*   Function :  Thread safe parameter handler.
*****************************************************************************/
INT32S parameter(INT8U command,INT8U name, ...);

/******************************** EVENT **************************************
*   Input    : 	PUSH or POP, name of event to access, third argument only
*   			used if pushing
*   Output   : 	popped value or zero if empty
*   Function :  Thread safe event handler. Elements are erased when popped
*****************************************************************************/
INT8U event(INT8U command,INT8U name,...);

/******************************* COUNTER *************************************
*   Input    : 	PUSH, POP, INCREMENT, DECREMENT or RESET
*   			name of counter to access, third argument only used if pushing
*   Output   : 	new counter value
*   Function :  Thread safe counter handler.
*****************************************************************************/
INT16S counter(INT8U command,INT8U name,...);

/******************************** STATE **************************************
*   Input    : 	PUSH or POP, name of state to access, third argument only
*   			used if pushing
*   Output   : 	popped value or zero if empty
*   Function :  Thread safe event state.
*****************************************************************************/
INT8U state(INT8U command,	INT8U name,...);

void position(enum data_commands command, INT8U number,...);

BOOLEAN itc_init_uartprinter(void);

#endif /*_DATA_H*/
