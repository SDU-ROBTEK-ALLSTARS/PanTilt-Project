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

	while(TRUE)
	{
		//handle inputs
		menu = parse_dreh_event(menu);

		if(menu->type == INPUT)
		{
			for(i = 0 ; i < NUMBER_OF_FIELDS ; i++)
			{
				if(menu->field[i].dreh_input)
					parameter(ADD, menu->field[i].parameter, DREH_TO_DEG(counter(RESET,DREH_C)) );
				if(menu->field[i].adc_input)
					parameter(PUSH, menu->field[i].parameter, ADC_TO_DEG(get_adc()) );
			}
		}

		//update display
		display_buffer_write_string(0,0,menu->topline);
		display_buffer_write_string(0,1,menu->bottomline);

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
	if(handle == FIRST_RUN)
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
			iterator->type = NOT_DEFINED;
		}
		else
		{

			switch(iterator->type)
			{
			case MENU:
				state(PUSH,DREH_S,EVENT);
				break;
			case VIEW:
				state(PUSH,DREH_S,EVENT);
				break;
			case INPUT:
				state(PUSH,DREH_S,COUNT);
				break;
			case CALL:
				state(PUSH,DREH_S,EVENT);
				(*iterator->function)();
				break;
			case NOT_DEFINED:
				break;
			default:
				state(PUSH,DREH_S,EVENT);
				break;
			}

			display_buffer_clear_blink();
		}
	}
	return iterator;
}


//menu_t* menu_handler(enum menu_names name)
//{
//	static menu_t menus[NUMBER_OF_MENUS];
//
//	switch(menus[name].type)
//	{
//	case MENU:
//		state(PUSH,DREH_S,EVENT);
//		break;
//	case VIEW:
//		state(PUSH,DREH_S,EVENT);
//		break;
//	case INPUT:
//		state(PUSH,DREH_S,COUNT);
//		break;
//	case CALL:
//		state(PUSH,DREH_S,EVENT);
//		(*menus[name].function)();
//		break;
//	default:
//		state(PUSH,DREH_S,EVENT);
//		break;
//	}
//
//	display_buffer_clear_blink();
//
//	return &menus[name];
//}

menu_t* parse_dreh_event(menu_t* menu)
{
	switch (event(POP,DREH_E))
	{
	case IDLE:
		break;
	case DREH_CW:
		if(menu->next)
		{
			menu = menu_handler(menu->next);
		}
		break;
	case DREH_CCW:
		if(menu->previous)
		{
			menu = menu_handler(menu->previous);
		}
		break;
	case DREH_PUSH:
		if(menu->enter)
		{
			menu = menu_handler(menu->enter);
		}
		break;
	}
	return menu;
}

INT32S parse_numpad(INT32S former_value)
{
	INT8U input;
	char string[NUMBER_OF_DIGITS];

	input = queue(POP,NUMPAD_Q);
	if(input)
	{
		//		for(i = 0 ; i < NUMBER_OF_DIGITS ; i++)
		//			string[i] = 0;
		number_to_string(string,former_value);

		//		while(input)
		//		{
		//			for(i = 0 ; i < NUMBER_OF_DIGITS - 1 ; i++)
		//				string[i] = string[i+1];
		//			string[NUMBER_OF_DIGITS - 1] = input;
		//			input = queue(POP,NUMPAD_Q);
		//		}

		former_value = string_to_number(string);


	}
	return former_value;
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

	//go to the end of array
	//	for(i = 0 ; array[i] != 0 ; i++);

	do
	{
		//add digit to array and decrease pointer
		array[i] = ( number % 10 ) + '0';
		i--;
	}
	//remove digit
	while(( number /= 10 ) > 0);

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

void begin_step(void)
{
	red_led( FALSE );
	yellow_led( FALSE );
	green_led( TRUE );

	parameter(PUSH,TILT_PWM_P, 0x8000);
	parameter(PUSH,PAN_PWM_P, 0);
}

void end_step(void)
{
	red_led( FALSE );
	yellow_led( FALSE );
	green_led( FALSE );

	parameter(PUSH,TILT_PWM_P, 0);
	parameter(PUSH,PAN_PWM_P, 0);
}

void red_only(void)
{
	red_led( TRUE );
	yellow_led( FALSE );
	green_led( FALSE );
}

void green_only(void)
{
	red_led( FALSE );
	yellow_led( FALSE );
	green_led( TRUE );
}
