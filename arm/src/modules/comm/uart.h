
#ifndef UART_H
#define UART_H

#define UART_TASK_TRANSMIT_STACK_SIZE  configMINIMAL_STACK_SIZE
#define UART_TASK_TRANSMIT_PRIORITY    (tskIDLE_PRIORITY)
#define UART_QUEUE_OUT_SIZE            32
#define UART_QUEUE_IN_SIZE             64

void uart_init_hw(void);
BOOLEAN uart_init_task(void);
INT32U uart_write(const INT8U *buffer, INT32U length, portTickType ticks_to_block);

#endif /* UART_H */
