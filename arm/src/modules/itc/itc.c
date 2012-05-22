/*****************************************************************************
 * University of Southern Denmark
 * Robotics Engineering Spring 2012
 * Pan Tilt Project
 *
 * MODULENAME.: itc
 *****************************************************************************/

/***************************** Include files *******************************/
#include "itc.h"
#include <stdarg.h>
#include <string.h>
#include "comm/uart.h"
#include "comm/uart_protocol.h"
#include "utils/itoa.h"

/*****************************   Functions   *******************************/
INT32S parameter(enum data_commands command,enum parameter_handles name, ...)
{
	static INT32S parameters[NUMBER_OF_PARAMETERS];
	INT32S out = 0;
	va_list argument;
	va_start(argument,name);

	TEST_SEM_PARAMETER
	{
		switch(command)
		{
		case PUSH:
			parameters[name] = va_arg(argument,signed long);
			break;
		case POP:
			out = parameters[name];
			break;
		case ADD:
			parameters[name] += va_arg(argument,signed long);
			out = parameters[name];
			break;
		case SUB:
			parameters[name] -= va_arg(argument,signed long);
			out = parameters[name];
			break;
		case ADD_POS:
			parameters[name] += va_arg(argument,signed long);
			if(parameters[name] < 0)
				parameters[name] = 0;
			break;
		default:
			break;
		}
		GIVE_SEM_PARAMETER
	}
	return out;
}


INT8U event(enum data_commands command,enum event_handles name, ...)
{
	static INT8U events[NUMBER_OF_EVENTS];
	INT8U out = 0;
	va_list argument;
	va_start(argument,name);

	TEST_SEM_EVENT
	{
		switch(command)
		{
		case PUSH:
			events[name] = (INT8U)va_arg(argument,int);
			break;
		case POP:
			out = events[name];
			events[name] = 0;
			break;
		case PEEK:
			out = events[name];
			break;
		default:
			break;
		}
		GIVE_SEM_EVENT
	}
	return out;
}

INT16S counter(enum data_commands command,enum counter_handles name, ...)
{
	static INT16S counters[NUMBER_OF_COUNTERS];
	INT16S out = 0;
	va_list argument;
	va_start(argument,name);

	TEST_SEM_COUNTER
	{
		switch(command)
		{
		case POP:
			out = counters[name];
			break;
		case INCREMENT:
			counters[name]++;
			out = counters[name];
			break;
		case DECREMENT:
			counters[name]--;
			out = counters[name];
			break;
		case PUSH:
			counters[name] = va_arg(argument,int);
			out = counters[name];
			break;
		case RESET:
			out = counters[name];
			counters[name] = 0;
			break;
		default:
			break;

		}
		GIVE_SEM_COUNTER
	}

	return out;
}

INT8U state(enum data_commands command,enum state_handles name, ...)
{
	static INT8U states[NUMBER_OF_STATES];
	INT8U out = 0;
	va_list argument;
	va_start(argument,name);

	TEST_SEM_STATE
	{
		switch(command)
		{
		case PUSH:
			states[name] = (INT8U)va_arg(argument,int);
			break;
		case POP:
			out = states[name];
			break;
		case TOGGLE:
			if(states[name])
				states[name] = FALSE;
			else
				states[name] = TRUE;
			break;
		case ADD:
			states[name] += (INT8U)va_arg(argument,int);
			out = states[name];
			break;
		default:
			break;
		}
		GIVE_SEM_STATE
	}
	return out;
}

void position(enum data_commands command, INT8U number,...)
{
	static struct position_struct positions[NUMBER_OF_POSITIONS];
	struct position_struct dummy;
	INT8U i;
	va_list argument;
	va_start(argument,number);

	switch(command)
	{
	case NEW:
		dummy.pan = 0;
		dummy.tilt = 0;
		for(i = 0 ; i < NUMBER_OF_POSITIONS ; i++)
			positions[i] = dummy;
		break;
	case SAVE:
		if((number >= 0) && (number < NUMBER_OF_POSITIONS))
		{
			dummy.pan = parameter(POP,PAN_CURRENT_P);
			dummy.tilt = parameter(POP,TILT_CURRENT_P);
			positions[number] = dummy;
		}
		break;

	case GOTO:
		if(number >= 0 && number < NUMBER_OF_POSITIONS)
		{
			parameter(PUSH,PAN_SETPOINT_P,positions[number].pan);
			parameter(PUSH,TILT_SETPOINT_P,positions[number].tilt);
		}
		break;

	default:
		break;
	}

}

/*
 * Prints a report of the current value of the parameters saved in memory.
 *
 * TODO: Would benefit from a printf function
 */
void print_parameters_on_uart(uart_packet_t *packet)
{
	char number[12] = {0};

	uart_write((INT8U *) "PAN PWM:\t", 9, portMAX_DELAY);
	uart_write((INT8U *) itoa((int) parameter(POP, PAN_PWM_P), number, 10), strlen(number), portMAX_DELAY);
	uart_write((INT8U *) "\n", 1, portMAX_DELAY);

	uart_write((INT8U *) "TILT PWM:\t", 10, portMAX_DELAY);
	uart_write((INT8U *) itoa((int) parameter(POP, TILT_PWM_P), number, 10), strlen(number), portMAX_DELAY);
	uart_write((INT8U *) "\n", 1, portMAX_DELAY);

	uart_write((INT8U *) "PAN POS:\t", 9, portMAX_DELAY);
	uart_write((INT8U *) itoa((int) parameter(POP, PAN_POSITION_P), number, 10), strlen(number), portMAX_DELAY);
	uart_write((INT8U *) "\n", 1, portMAX_DELAY);

	uart_write((INT8U *) "TILT POS:\t", 10, portMAX_DELAY);
	uart_write((INT8U *) itoa((int) parameter(POP, TILT_POSITION_P), number, 10), strlen(number), portMAX_DELAY);
	uart_write((INT8U *) "\n", 1, portMAX_DELAY);

	uart_write((INT8U *) "PAN VEL:\t", 9, portMAX_DELAY);
	uart_write((INT8U *) itoa((int) parameter(POP, PAN_VELOCITY_P), number, 10), strlen(number), portMAX_DELAY);
	uart_write((INT8U *) "\n", 1, portMAX_DELAY);

	uart_write((INT8U *) "TILT VEL:\t", 10, portMAX_DELAY);
	uart_write((INT8U *) itoa((int) parameter(POP, TILT_VELOCITY_P), number, 10), strlen(number), portMAX_DELAY);
	uart_write((INT8U *) "\n", 1, portMAX_DELAY);
}

BOOLEAN itc_init_uartprinter(void)
{
	return uart_protocol_register_handler(&print_parameters_on_uart, UART_PACKET_TYPE_GET, 2);
}
