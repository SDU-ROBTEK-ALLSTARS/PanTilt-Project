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
			break;
		case SUB:
			parameters[name] -= va_arg(argument,signed long);
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

INT8U queue(enum data_commands command, enum queue_handles name, ...)
{
	static INT8U queue[NUMBER_OF_QUEUES][QUEUE_SIZE];
	static INT8U next_in = 0;
	static INT8U next_out = 0;
	INT8U i,out = 0;
	va_list argument;
	va_start(argument,name);

	TEST_SEM_QUEUE
	{
		switch(command)
		{
		case PUSH:
			if(!queue[name][next_in])				//prevent from overwriting non-popped
			{
				queue[name][next_in] = (INT8U)va_arg(argument,int);
				next_in++;
				if(next_in >= QUEUE_SIZE)
					next_in = 0;
			}
			break;
		case POP:
			if(queue[name][next_out])				//prevent from popping non-pushed
			{
				out = queue[name][next_out];
				queue[name][next_out] = 0;
				next_out++;
				if(next_out >= QUEUE_SIZE)
					next_out = 0;
			}
			break;
		case FLUSH:
			for(i = 0 ; i < QUEUE_SIZE ; i++)
				queue[name][i] = 0;
			break;
		case PEEK:
			out = queue[name][next_out];
			break;
		default:
			break;

		}
		GIVE_SEM_QUEUE
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
		default:
			break;
		}
		GIVE_SEM_STATE
	}
	return out;
}
