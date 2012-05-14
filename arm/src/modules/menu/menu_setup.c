/*****************************************************************************
 * University of Southern Denmark
 * Pan Tilt Project
 *
 * MODULENAME.: menu
 *****************************************************************************/

//							MENU PROTOTYPE
//	SETUP(-name of menu-)
//		TOP(" -top 16 chars- ")			BOTTOM(" -bottom 16 chars- ")
//		NEXT( -name of menu- )			PREV( -name of menu- )
//		ENTER( -name of menu- )			TYPE( -menu type- )
//	INIT_FIELDS
//		FIELD(number,x,y,size)			PARAM(field, -name of parameter- ) ACD(field)
//		FIELD(number,x,y,size)			PARAM(field, -name of parameter- ) DREH(field)
//		FIELD(number,x,y,size)			BLINK(field)

/***************************** Include files *******************************/
#include "menu.h"

/*****************************   Functions   *******************************/
void init_menus(void)
{
	menu_t *dummy;
	INT8U i;

	SETUP(MAIN_MENU)
		TOP("CURRENT POSITION") 		BOTTOM(	"P:      T:      ")
		NEXT(SET_MENU)					PREV(STEP_MENU)
		ENTER(NOT_IMPLEMENTED)			TYPE(VIEW)
	INIT_FIELDS
		FIELD(0,2,1,6)					PARAM(0,PAN_CURRENT_P)
		FIELD(1,10,1,6)					PARAM(1,TILT_CURRENT_P)


	SETUP(SET_MENU)
		TOP("\x7F****  SET  ****")		BOTTOM("POS   FREE  AUTO")
		NEXT(STEP_MENU)					PREV(MAIN_MENU)
		ENTER(SET_SUB_POS)				TYPE(MENU)
	INIT_FIELDS


	SETUP(SET_SUB_RETURN)
		TOP("\x7F****  SET  ****")		BOTTOM("POS   FREE  AUTO")
		NEXT(SET_SUB_POS)				PREV(SET_SUB_AUTO)
		ENTER(MAIN_MENU)				TYPE(MENU)
	INIT_FIELDS
		FIELD(0,0,0,1)					BLINK(0)


	SETUP(SET_SUB_POS)
		TOP("\x7F****  SET  ****")		BOTTOM("POS   FREE  AUTO")
		NEXT(SET_SUB_FREE)				PREV(SET_SUB_RETURN)
		ENTER(SET_POS_SUB)				TYPE(MENU)
	INIT_FIELDS
		FIELD(0,0,1,3)					BLINK(0)


	SETUP(SET_SUB_FREE)
		TOP("\x7F****  SET  ****")		BOTTOM("POS   FREE  AUTO")
		NEXT(SET_SUB_AUTO)				PREV(SET_SUB_POS)
		ENTER(SET_FREE_STO)				TYPE(MENU)
	INIT_FIELDS
		FIELD(0,6,1,4)					BLINK(0)


	SETUP(SET_FREE_STO)
		TOP("\x7F*FREE       STO")		BOTTOM("P:      T:      ")
		NEXT(SET_FREE_RETURN)			PREV(SET_FREE_RETURN)
		ENTER(NOT_IMPLEMENTED)			TYPE(CALL)
		FUNCTION(red_only)
	INIT_FIELDS
		FIELD(0,2,1,6)					PARAM(0,PAN_CURRENT_P)
		FIELD(1,10,1,6)					PARAM(1,TILT_CURRENT_P)
		FIELD(2,12,0,3)					BLINK(2)


	SETUP(SET_FREE_RETURN)
		TOP("\x7F*FREE       STO")		BOTTOM("P:      T:      ")
		NEXT(SET_FREE_STO)				PREV(SET_FREE_STO)
		ENTER(SET_SUB_RETURN)			TYPE(CALL)
		FUNCTION(green_only)
	INIT_FIELDS
		FIELD(0,2,1,6)					PARAM(0,PAN_CURRENT_P)
		FIELD(1,10,1,6)					PARAM(1,TILT_CURRENT_P)
		FIELD(2,0,0,1)					BLINK(2)


	SETUP(SET_SUB_AUTO)
		TOP("\x7F****  SET  ****")		BOTTOM("POS   FREE  AUTO")
		NEXT(SET_SUB_RETURN)			PREV(SET_SUB_FREE)
		ENTER(NOT_IMPLEMENTED)			TYPE(MENU)
	INIT_FIELDS
		FIELD(0,12,1,4)					BLINK(0)


	SETUP(NOT_IMPLEMENTED)
		TOP("THIS FEATURE IS ")			BOTTOM("NOT IMPLEMENTED ")
		NEXT(MAIN_MENU)					PREV(MAIN_MENU)
		ENTER(MAIN_MENU)				TYPE(MENU)
	INIT_FIELDS


	SETUP(SET_POS_SUB)
		TOP("\x7F* SET POSITION*")		BOTTOM("P:      T:      ")
		NEXT(SET_POS_SUB)				PREV(SET_POS_SUB)
		ENTER(SET_SUB_RETURN)			TYPE(INPUT)
	INIT_FIELDS
		FIELD(0,2,1,6)					PARAM(0,PAN_SETPOINT_P)	ADC(0)
		FIELD(1,10,1,6)					PARAM(1,TILT_SETPOINT_P)DREH(1)
		FIELD(2,0,0,1)					BLINK(2)

	SETUP(STEP_MENU)
		TOP("PRES DREH TO    ")			BOTTOM("STEP TILT MOTOR ")
		NEXT(MAIN_MENU)					PREV(SET_MENU)
		ENTER(STEP_START)				TYPE(MENU)
	INIT_FIELDS

	SETUP(STEP_START)
		TOP("PRES DREH TO    ")			BOTTOM("END STEP        ")
		NEXT(STEP_START)				PREV(STEP_START)
		ENTER(STEP_END)					TYPE(CALL)
		FUNCTION(begin_step)
	INIT_FIELDS

	SETUP(STEP_END)
		TOP("*  STEP ENDED  *")			BOTTOM("  PRESS TO EXIT ")
		NEXT(STEP_END)					PREV(STEP_END)
		ENTER(MAIN_MENU)				TYPE(CALL)
		FUNCTION(end_step)
	INIT_FIELDS

}
