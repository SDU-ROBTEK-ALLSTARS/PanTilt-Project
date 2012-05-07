
#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H

#define UART_TASK_PROTOCOL_STACK_SIZE  (configMINIMAL_STACK_SIZE * 3)
#define UART_TASK_PROTOCOL_PRIORITY    tskIDLE_PRIORITY

/* Packet format options */
#define UART_PACKET_HEADER_SIZE      4
#define UART_PACKET_MAX_DATA_SIZE    32

/* Number of possible uart command handlers */
#define UART_PACKET_MAX_TYPE         2
#define UART_PACKET_MAX_INSTRUCTION  16

/* Packet type field defs */
#define UART_PACKET_TYPE_SET         0
#define UART_PACKET_TYPE_GET         1

/* Packet type exposed in case functions called due to an UART
command needs the declaration. Data should be no larger than
UART_PACKET_MAX_DATA_SIZE */
typedef struct
{
  INT8U type;
  INT8U instruction;
  INT16U datalength;
  INT8U data[UART_PACKET_MAX_DATA_SIZE];
} uart_packet_t;

BOOLEAN uart_protocol_register_handler(void (*func_ptr)(), INT8U type, INT8U instruction);
BOOLEAN uart_protocol_init_task(void);


#endif /* UART_PROTOCOL_H */
