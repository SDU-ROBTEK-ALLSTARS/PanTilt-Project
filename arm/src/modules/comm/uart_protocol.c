
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* EMP course headers */
#include "inc/glob_def.h"
#include "inc/emp_type.h"

/* Modules */
#include "comm/uart.h"

/* This modules header */
#include "uart_protocol.h"

/*
 * Protocol packet format:
 * ----------------------
 *
 *
 * Byte#:         0          1          2 to 3     4 to MAX_DATA_SIZE+4
 *             ---------------------------------------------------------
 * Contains:   | TYPE | INSTRUCTION | DATALENGTH |         DATA        |
 *             ---------------------------------------------------------
 */

/**
 * Array of pointers to the functions that are to be executed when
 * a given command is received. Initializes to NULL.
 */
static void (*command[UART_PACKET_MAX_TYPE][UART_PACKET_MAX_INSTRUCTION])() = {{NULL},{NULL}};

/**
 * uart incoming queue (extern in uart.c)
 */
xQueueHandle uart_queue_in;

/**
 * Registers a function pointer in the array if the spot is not
 * already taken.
 */
BOOLEAN uart_protocol_register_handler(void (*func_ptr)(), INT8U type, INT8U instruction)
{
  if ((command[type][instruction] == NULL) && (type < UART_PACKET_MAX_TYPE) && (instruction < UART_PACKET_MAX_INSTRUCTION))
  {
    command[type][instruction] = func_ptr;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
 * UART protocol task
 *
 * This task handles incoming messages and executes the commands they contain.
 *
 * The instruction contained in the TYPE and INSTRUCTION fields is executed
 * if it exists in the COMMAND table. If an unknown instruction is sent,
 * it will be discarded.
 */
void uart_protocol_task(void *parameters)
{
  uart_packet_t packet;
  INT8U i;
  INT8U dataLengthTemp;

  while(1)
  {
    /* If there is a header in the uart_queue_in, get it */
    if (uxQueueMessagesWaiting(uart_queue_in) >= UART_PACKET_HEADER_SIZE)
    {
      for (i=0; i<UART_PACKET_HEADER_SIZE; i++)
      {
        /* The byte number received decides which packet field it fits into */
        switch (i)
        {
        case 0:
          xQueueReceive(uart_queue_in, &packet.type, portMAX_DELAY);
          break;
        case 1:
          xQueueReceive(uart_queue_in, &packet.instruction, portMAX_DELAY);
          break;
        case 2:
          xQueueReceive(uart_queue_in, &dataLengthTemp, portMAX_DELAY);
          packet.datalength = (INT16U) dataLengthTemp;
          break;
        case 3:
          xQueueReceive(uart_queue_in, &dataLengthTemp, portMAX_DELAY);
          packet.datalength += (INT16U) (dataLengthTemp << 8);
          break;
        default:
          break;
        }
      }

      /* Verify the validity of the received header. */
      if (command[packet.type][packet.instruction] != NULL &&
          packet.type < UART_PACKET_MAX_TYPE &&
          packet.instruction < UART_PACKET_MAX_INSTRUCTION &&
          packet.datalength <= UART_PACKET_MAX_DATA_SIZE)
      {
        /* Get the data field, if needed. */
        if (packet.datalength != 0)
        {
          for (i=0; i<packet.datalength; i++)
          {
            xQueueReceive(uart_queue_in, &packet.data[i], (portTickType) 255);
          }
        }

        /* Execute the command that was received. A pointer to packet is
        supplied as argument. The handler must decide whether to use it
        or not. */
        (*command[packet.type][packet.instruction])(&packet);
      }
    }
  }
}

/**
 * Initializes the UART protocol tasks
 */
BOOLEAN uart_protocol_init_task(void)
{
  return xTaskCreate(uart_protocol_task,
                     (signed portCHAR *)"UART_PROT",
                     UART_TASK_PROTOCOL_STACK_SIZE,
                     NULL,
                     UART_TASK_PROTOCOL_PRIORITY,
                     NULL);
}
