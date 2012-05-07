/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* StellarisWare drivers */
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"

/* EMP course standard headers */
#include "glob_def.h"
#include "emp_type.h"

/* This modules header */
#include "spi.h"

/**
 * @file spi.c
 *
 * Using the FreeRTOS pvPortMalloc (heap2.c) to dynamically allocate queues
 *
 * Relies heavily on the fact that the MCU running this code is the master of
 * the SPI!
 *
 * TODO:
 * - Documentation
 * - ISR speed optimization (write directly to hardware here)
 * - Garbage collection: Deletion of list items (queues) which have not been
 *   used for a long time.
 * - Mutex considerations
 */

#define SPI_INTERN_QUEUE_IN_SIZE     32
#define SPI_PUBLIC_QUEUE_SIZE        16

typedef unsigned char spi_message_t;

/* Types holding list information (one per list) and list items. */
struct queue_list_item_t
{
  struct queue_list_item_t *next;
  struct queue_list_item_t *prev;
  xTaskHandle  owner_task;
  xQueueHandle queue_in;
  xQueueHandle queue_out;
  INT32U num_waiting_to_receive;
};
struct queue_list_t
{
  struct queue_list_item_t *head;
  struct queue_list_item_t *tail;
  INT32U num_items;
  xSemaphoreHandle mutex;
};

/* List to store which queues are associated with which tasks,
   data input queue and queue of tasks waiting to receive data */
static struct queue_list_t list_registered_tasks;
static xQueueHandle intern_queue_in, intern_queue_waiting_to_receive;

/**
 * @internal
 */
void queue_list_initialize(struct queue_list_t *p_list)
{
  p_list->head = NULL;
  p_list->tail = NULL;
  p_list->num_items = 0;
  p_list->mutex = xSemaphoreCreateMutex();
}

/**
 * @internal
 */
void queue_list_item_insert_end(struct queue_list_t *p_list, struct queue_list_item_t *p_item)
{
  if (xSemaphoreTake(p_list->mutex, portMAX_DELAY) == pdTRUE)
  {
    if (p_list->head == NULL)
    {
      /* List is empty: This becomes first and last item in it */
      p_item->next = NULL;
      p_item->prev = NULL;
      p_list->head = p_item;
      p_list->tail = p_list->head;
    }
    else
    {
      /* List is not empty, insert in back */
      p_item->next = NULL;
      p_item->prev = p_list->tail;
      p_list->tail->next = p_item;
      p_list->tail = p_item;
    }
    p_list->num_items++;

    xSemaphoreGive(p_list->mutex);
  }
}

/**
 * @internal
 */
void queue_list_item_remove(struct queue_list_t *p_list, struct queue_list_item_t *p_item)
{
  if (xSemaphoreTake(p_list->mutex, portMAX_DELAY) == pdTRUE)
  {
    if (p_list->head != NULL && p_item != NULL)
    {
      if (p_item == p_list->head)
      {
        p_list->head = p_item->next;
        p_list->head->prev = NULL;
      }
      else if (p_item == p_list->tail)
      {
        p_list->tail = p_item->prev;
        p_list->tail->next = NULL;
      }
      else
      {
        p_item->prev->next = p_item->next;
        p_item->next->prev = p_item->prev;
      }
      p_list->num_items--;
    }
    xSemaphoreGive(p_list->mutex);
  }
}

/**
 * @internal
 */
struct queue_list_item_t *queue_list_item_find(struct queue_list_t *p_list,
    xTaskHandle owner_task_target)
{
  struct queue_list_item_t *current_item = NULL;

  if (xSemaphoreTake(p_list->mutex, portMAX_DELAY) == pdTRUE)
  {
    if (p_list->head != NULL)
    {
      current_item = p_list->head;

      while (current_item != NULL)
      {
        if (current_item->owner_task == owner_task_target)
        {
          break;
        }
        else
        {
          current_item = current_item->next;
        }
      }
    }
    xSemaphoreGive(p_list->mutex);
  }

  /* Dont return HEAD if its not the right one */
  if (current_item->owner_task != owner_task_target)
  {
    return NULL;
  }
  else
  {
    return current_item;
  }
}

/**
 * @internal (not used atm)
 */
struct queue_list_item_t *queue_list_item_new(xTaskHandle task_handle)
{
  struct queue_list_item_t *new_list_item;
  new_list_item = (struct queue_list_item_t *) pvPortMalloc(sizeof(struct queue_list_item_t));

  if (new_list_item != NULL)
  {
    new_list_item->owner_task = task_handle;
    new_list_item->queue_in = xQueueCreate(SPI_PUBLIC_QUEUE_SIZE, sizeof(spi_message_t));
    new_list_item->queue_out = xQueueCreate(SPI_PUBLIC_QUEUE_SIZE, sizeof(spi_message_t));
    new_list_item->num_waiting_to_receive = 0;
  }

  return new_list_item;
}

/**
 * Writes to a tasks private SPI queue.
 *
 * @param buf is a pointer to the data to write.
 * @param nbytes is the number of bytes to write.
 * @param ticks_to_block is the amount of ticks the function may block for.
 *
 * @return the number of bytes succesfully written.
 */
INT32S spi_write_from_task(const INT8U *buf, INT32U nbytes, portTickType ticks_to_block)
{
  INT32U result;
  xTaskHandle current_task_handle = xTaskGetCurrentTaskHandle();
  struct queue_list_item_t *calling_task_list_item =
    queue_list_item_find(&list_registered_tasks, current_task_handle);

  /* If the calling task is un-registered, make a list item (containing I/O queues)
  for it */
  if (calling_task_list_item == NULL)
  {
    struct queue_list_item_t *new_caller_list_item;
    new_caller_list_item = (struct queue_list_item_t *) pvPortMalloc(sizeof(struct queue_list_item_t));

    if (new_caller_list_item != NULL)
    {
      new_caller_list_item->owner_task = current_task_handle;
      new_caller_list_item->queue_in = xQueueCreate(SPI_PUBLIC_QUEUE_SIZE, sizeof(spi_message_t));
      new_caller_list_item->queue_out = xQueueCreate(SPI_PUBLIC_QUEUE_SIZE, sizeof(spi_message_t));
      new_caller_list_item->num_waiting_to_receive = 0;

      queue_list_item_insert_end(&list_registered_tasks, new_caller_list_item);

      calling_task_list_item = new_caller_list_item;
    }
  }

  /* Write data to transmit. */
  result = 0;

  if (calling_task_list_item != NULL)
  {
    while (result < nbytes)
    {
      if (xQueueSendToBack(calling_task_list_item->queue_out, &buf[result], ticks_to_block) == errQUEUE_FULL)
      {
        break;
      }
      else
      {
        result++;
      }
    }
  }

  return result;
}

/**
 * Reads data from a tasks private SPI queue.
 *
 * @param buf is a pointer to a memory area to store read data.
 * @param nbytes is the number of bytes to read.
 * @param ticks_to_block is the amount of ticks the function may block for.
 *
 * @return the number of bytes succesfully read.
 */
INT32S spi_read_from_task(INT8U *buf, INT32U nbytes, portTickType ticks_to_block)
{
  INT32U result;
  xTaskHandle current_task_handle = xTaskGetCurrentTaskHandle();
  struct queue_list_item_t *calling_task_list_item =
    queue_list_item_find(&list_registered_tasks, current_task_handle);

  /* If the calling task is un-registered, make a list item (containing I/O queues)
  for it */
  if (calling_task_list_item == NULL)
  {
    struct queue_list_item_t *new_caller_list_item;
    new_caller_list_item = (struct queue_list_item_t *) pvPortMalloc(sizeof(struct queue_list_item_t));

    if (new_caller_list_item != NULL)
    {
      new_caller_list_item->owner_task = current_task_handle;
      new_caller_list_item->queue_in = xQueueCreate(SPI_PUBLIC_QUEUE_SIZE, sizeof(spi_message_t));
      new_caller_list_item->queue_out = xQueueCreate(SPI_PUBLIC_QUEUE_SIZE, sizeof(spi_message_t));
      new_caller_list_item->num_waiting_to_receive = 0;

      queue_list_item_insert_end(&list_registered_tasks, new_caller_list_item);

      calling_task_list_item = new_caller_list_item;
    }
  }

  /* Read, if task is registered */
  result = 0;

  if (calling_task_list_item != NULL)
  {
    while (result < nbytes)
    {
      if (xQueueReceive(calling_task_list_item->queue_in, &buf[result], ticks_to_block) == pdTRUE)
      {
        result++;
      }
      else
      {
        break;
      }
    }
  }

  return result;
}

//BOOLEAN spi_register_task(xTaskHandle task_handle)
//{
//  xTaskHandle task_handle_to_register;
//
//  /* If NULL is passed, the calling tasks task-handle is registered */
//  if (task_handle == NULL)
//  {
//    task_handle_to_register = xTaskGetCurrentTaskHandle();
//  }
//  else
//  {
//    task_handle_to_register = task_handle;
//  }
//
//
//
//
//
//
//  struct queue_list_item_t *calling_task_list_item =
//                           queue_list_item_find(&list_registered_tasks, current_task_handle);
//
//  /* If the calling task is un-registered, make a list item (containing I/O queues)
//  for it */
//  if (calling_task_list_item == NULL)
//  {
//    struct queue_list_item_t *new_caller_list_item;
//    new_caller_list_item = (struct queue_list_item_t *) pvPortMalloc(sizeof(struct queue_list_item_t));
//
//    if (new_caller_list_item != NULL)
//    {
//      new_caller_list_item->owner_task = current_task_handle;
//      new_caller_list_item->queue_in = xQueueCreate(SPI_PUBLIC_QUEUE_SIZE, sizeof(spi_message_t));
//      new_caller_list_item->queue_out = xQueueCreate(SPI_PUBLIC_QUEUE_SIZE, sizeof(spi_message_t));
//      new_caller_list_item->num_waiting_to_receive = 0;
//
//      queue_list_item_insert_end(&list_registered_tasks, new_caller_list_item);
//
//      calling_task_list_item = new_caller_list_item;
//    }
//  }
//
//}

/**
 * SPI transmit task (using FreeRTOS)
 *
 * @param params is a pointer to the task parameters (used when task
 * is created).
 *
 * @return None (the task should never return).
 */
void task_spi_transmit(void *params)
{
  struct queue_list_item_t *current_item;
  INT32U num_messages_sent, num_messages_waiting;
  spi_message_t message;

  while(1)
  {
    /* If there are messages waiting in ANY of the task-out-queues, transmit
    that data and remember by who it was sent and how much. */
    if (list_registered_tasks.head != NULL)
    {
      current_item = list_registered_tasks.head;

      while (current_item != NULL)
      {
        num_messages_sent = 0;
        num_messages_waiting = uxQueueMessagesWaiting(current_item->queue_out);

        while (num_messages_sent < num_messages_waiting)
        {
          xQueueReceive(current_item->queue_out, &message, portMAX_DELAY);
          SSIDataPut(SSI0_BASE, (unsigned long) message);
          //xQueueSendToBack(intern_queue_in, &message, portMAX_DELAY);/* FOR DEBUG "FAKE" LOOPBACK:  */
          num_messages_sent++;
        }

        if (num_messages_sent > 0)
        {
          current_item->num_waiting_to_receive = num_messages_sent;
          xQueueSendToBack(intern_queue_waiting_to_receive, &current_item, portMAX_DELAY);
        }

        current_item = current_item->next;
      }
    }
  }
}

/**
 * SPI receive task (using FreeRTOS)
 *
 * @param params is a pointer to the task parameters (used when task
 * is created).
 *
 * @return None (the task should never return).
 */
void task_spi_receive(void *params)
{
  spi_message_t message;
  struct queue_list_item_t *receiver;

  while(1)
  {
    xQueueReceive(intern_queue_in, &message, portMAX_DELAY);

    if (xQueuePeek(intern_queue_waiting_to_receive, &receiver, (portTickType) 0xFF) == pdTRUE)
    {
      xQueueSendToBack(receiver->queue_in, &message, portMAX_DELAY);
      receiver->num_waiting_to_receive--;

      if (receiver->num_waiting_to_receive == 0)
      {
        /* Receive just to delete from queue */
        xQueueReceive(intern_queue_waiting_to_receive, &receiver, portMAX_DELAY);
      }
    }
    /*
    else
    {
      Error: No receiver??
    }
    */
  }
}

/**
 * SPI interrupt service routine
 *
 * @param None
 *
 *
 * @return None
 */
void spi_int_handler(void)
{
  unsigned long message;
  portBASE_TYPE higher_prio_task_woken = FALSE;

  SSIIntClear(SSI0_BASE, SSI_RXTO); //SSI_ICR_RTIC

  if (SSIDataGetNonBlocking(SSI0_BASE, &message) == 1)
  {
    xQueueSendToBackFromISR(intern_queue_in, (spi_message_t *) &message, &higher_prio_task_woken); //pdPASS
  }

  portEND_SWITCHING_ISR(higher_prio_task_woken);
}

/**
 * Initializer for SPI related tasks and other ressources.
 *
 * @param None.
 *
 * This function creates the FreeRTOS SPI-transmit task and associated queues.
 * It also enables SPI receive interrupts.
 *
 * @return TRUE if the initialization was successful, FALSE otherwise.
 */
BOOLEAN spi_init(void)
{
  /* Public queues for in- and output */
  intern_queue_in = xQueueCreate(SPI_INTERN_QUEUE_IN_SIZE, sizeof(spi_message_t));
  intern_queue_waiting_to_receive = xQueueCreate(SPI_NUM_MAX_USERTASKS, sizeof(struct queue_list_item_t *));

  /* Create transmit task */
  INT8U task_create_success = xTaskCreate(task_spi_transmit,
                                          (signed portCHAR *) "SPI_TX",
                                          configMINIMAL_STACK_SIZE,
                                          NULL,
                                          tskIDLE_PRIORITY,
                                          NULL);

  task_create_success |= xTaskCreate(task_spi_receive,
                                     (signed portCHAR *) "SPI_RX",
                                     configMINIMAL_STACK_SIZE,
                                     NULL,
                                     tskIDLE_PRIORITY,
                                     NULL);

  queue_list_initialize(&list_registered_tasks);

  /* Make sure everything is constructed succesfully.
  The return value signals the outcome. */
  if (intern_queue_in && task_create_success && intern_queue_waiting_to_receive)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
 * Configures hardware periphirals for SPI function
 *
 * @param None
 * @return None
 */
void spi_config_hw(void)
{
  //SSIDisable(SSI0_BASE);

  SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
  SysCtlDelay(3);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlDelay(3);

  GPIOPinTypeSSI(GPIO_PORTA_BASE, (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5));

  /* Set clock rate, SPH=0, SPO=1, Freescale, 2 MHz, 8 bit data size */
  SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                     SSI_MODE_MASTER, SPI_CONF_BITRATE, SPI_CONF_DATA_WIDTH);

  /* Enable receive "FIFO half-full or more" and "receive timeout"-interrupts */
  IntEnable(INT_SSI0);
  SSIIntEnable(SSI0_BASE, (SSI_RXFF | SSI_RXTO));

  /* Set interrupt priority to one level LOWER than
  configMAX_SYSCALL_INTERRUPT_PRIORITY so that we can still use FreeRTOS API
  functions from within it.
  Remember: Higher value => lower priority. */
  IntPrioritySet(INT_UART0, configMAX_SYSCALL_INTERRUPT_PRIORITY + (unsigned char) (1 << 5));

  SSIEnable(SSI0_BASE);
}
