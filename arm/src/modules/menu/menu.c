/*****************************************************************************
 * University of Southern Denmark
 * Pan Tilt Project
 *
 * MODULENAME.: menu.c
 *****************************************************************************/

/***************************** Include files *********************************/
#include "menu.h"

/**********************   Function implementations   *************************/
void menu_task(void *pvParameters)
{
	menu_t *menu;
	INT8U i;

	//setup menus
	init_menus();
	menu = menu_handler(SYSTEM_START_MENU);

	red_led( FALSE );
	yellow_led( FALSE );
	green_led( FALSE );

	//setup initial system parameters
	parameter(PUSH,PAN_SETPOINT_P,0);
	parameter(PUSH,TILT_SETPOINT_P,0);
	parameter(PUSH,PAN_PWM_P,0);
	parameter(PUSH,TILT_PWM_P,0);
	parameter(PUSH,NEXT_POS_P,0);
	parameter(PUSH,PAN_CURRENT_P,0);
	parameter(PUSH,TILT_CURRENT_P,0);

	vTaskSuspend( task_handles[CONTROL_T] );

	while(TRUE)
	{
		//handle inputs
		menu = parse_dreh_event(menu);

		if(menu->type.is_input)
		{
			for(i = 0 ; i < NUMBER_OF_FIELDS ; i++)
			{
				if(menu->field[i].dreh_input)
					parameter(ADD, menu->field[i].parameter, DREH_TO_DEG(counter(RESET,DREH_C)) );
				if(menu->field[i].adc_input)
					parameter(PUSH, menu->field[i].parameter, ADC_TO_DEG(get_adc()) );
				if(menu->field[i].numpad_input)
					if(event(PEEK,NUMPAD_E))
						parameter(PUSH, menu->field[i].parameter, CHAR_TO_NUMBER(event(POP,NUMPAD_E) ));
			}
		}

		//update display buffer
		display_buffer_write_string(0,0,menu->text.topline);
		display_buffer_write_string(0,1,menu->text.bottomline);

		for(i=0 ; i < NUMBER_OF_FIELDS ; i++)
		{
			if(menu->field[i].show)
				display_buffer_write_decimal(
						menu->field[i].begin.x,
						menu->field[i].begin.y,
						menu->field[i].size,
						NUMBER_OF_DECIMALS,
						parameter(POP,menu->field[i].parameter));
			if(menu->field[i].blink)
				display_buffer_set_blink(
						menu->field[i].begin.x,
						menu->field[i].begin.y,
						menu->field[i].size);
		}

		YIELD(YIELD_TIME_MENU_T)
	}
}

menu_t* menu_handler(INT8U handle)
{
	static struct menu_struct *root;
	struct menu_struct *iterator;
	struct menu_struct dummy;
	dummy.menu_handle = handle;

	//if first run
	if(handle == FIRST_RUN_M)
	{
		root = pvPortMalloc( sizeof(menu_t) );
		root->menu_handle = 0;
		root->next_handle = 0;
	}
	else
	{

		//search for the menu
		for(iterator = root ; iterator->next_handle && iterator->menu_handle != dummy.menu_handle ; iterator = iterator->next_handle );

		//if menu was not found
		if(iterator->menu_handle != dummy.menu_handle)
		{
			//add to list
			iterator->next_handle = pvPortMalloc( sizeof(menu_t) );
			iterator = iterator->next_handle;
			iterator->next_handle = 0;
			iterator->menu_handle = handle;
		}
		else
		{
			if(iterator->type.is_menu)
				state(PUSH,DREH_S,EVENT);
			if(iterator->type.is_input)
				state(PUSH,DREH_S,COUNT);
			if(iterator->type.is_call)
				(*iterator->function)();

			display_buffer_clear_blink();
		}
	}
	return iterator;
}

menu_t* parse_dreh_event(menu_t* menu)
{
	switch (event(POP,DREH_E))
	{
	case IDLE:
		break;
	case DREH_CW:
		menu = menu_handler(menu->next);
		break;
	case DREH_CCW:
		menu = menu_handler(menu->previous);
		break;
	case DREH_PUSH:
		menu = menu_handler(menu->enter);
		break;
	}
	return menu;
}

INT32S string_to_number(char *array)
{
	INT8U i,j;
	INT32S value = 0;

	for(i = 0 ; array[i] != 0 ; i++);
	j = i - 1;
	while(i)
	{
		i--;
		value += (array[i] - '0')*power_of_ten(j-i);
	}

	return value;
}

void number_to_string(char *array, INT32S number)
{
	INT8U i = 4;

	do
	{
		array[i] = ( number % 10 ) + '0';		//add digit to array and decrease pointer
		i--;
	}
	while(( number /= 10 ) > 0);				//remove digit
}

INT32S power_of_ten(INT8U operand)
{
	INT32S out;
	switch(operand)
	{
	case 0: out = 1; break;
	case 1: out = 10; break;
	case 2: out = 100; break;
	case 3: out = 1000; break;
	case 4: out = 10000; break;
	case 5: out = 100000; break;
	case 6: out = 1000000; break;
	case 7: out = 10000000; break;
	case 8: out = 100000000; break;
	case 9: out = 1000000000; break;
	default: out = 0; break;
	}
	return out;
}

void enter_freemode(void)
{
	red_led( FALSE );
	yellow_led( FALSE );
	green_led( TRUE );
}

void exit_freemode(void)
{
	red_led( FALSE );
	yellow_led( FALSE );
	green_led( FALSE );
}
void activate_regulator(void)
{
	vTaskResume( task_handles[CONTROL_T] );
}

void deactivate_regulator(void)
{
	vTaskSuspend( task_handles[CONTROL_T] );
}
void activate_automode(void)
{
	state(PUSH,AUTO_MODE_S,TRUE);
	vTaskResume( task_handles[CONTROL_T] );
}

void deactivate_automode(void)
{
	state(PUSH,AUTO_MODE_S,FALSE);
}
void activate_ss(void)
{
	red_led( FALSE );
	yellow_led( FALSE );
	green_led( TRUE );
}
void activate_pid(void)
{
	red_led( FALSE );
	yellow_led( TRUE );
	green_led( FALSE );
}

void save_position(void)
{
	position(SAVE,	parameter(POP,NEXT_POS_P));
}


