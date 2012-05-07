/*****************************************************************************
* University of Southern Denmark
* Robotics Engineering Spring 2012
* Pan Tilt Project
*
* HEADERNAME.: os
*
* DESCRIPTION: This header implements global macros for low coupling to OS
*
* Change Log:
* Date    Id    Change
* YYMMDD
* --------------------
* 120407  LBL   Module created
*
*****************************************************************************/

/*****************************    Pragma    *******************************/
#ifndef SYNC_H_
#define SYNC_H_

/***************************** Include files *******************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "emp_type.h"

/*****************************    Defines    *******************************/
#define TEST_SEM_COMMON_PINS		if( xSemaphoreTake(sem_common_pins, portMAX_DELAY ) == pdTRUE  )
#define GIVE_SEM_COMMON_PINS		xSemaphoreGive( sem_common_pins );

#define TEST_SEM_NUMPAD_QUEUE		if( xSemaphoreTake(sem_numpad_queue,portMAX_DELAY ) == pdTRUE  )
#define GIVE_SEM_NUMPAD_QUEUE		xSemaphoreGive( sem_numpad_queue );

#define TEST_SEM_LCD_BUFFER			if( xSemaphoreTake(sem_lcd_buffer,portMAX_DELAY ) == pdTRUE  )
#define GIVE_SEM_LCD_BUFFER			xSemaphoreGive( sem_lcd_buffer );

#define TEST_SEM_QUEUE				if( xSemaphoreTake(sem_queue,portMAX_DELAY ) == pdTRUE  )
#define GIVE_SEM_QUEUE				xSemaphoreGive( sem_queue );

#define TEST_SEM_EVENT				if( xSemaphoreTake(sem_event,portMAX_DELAY ) == pdTRUE  )
#define GIVE_SEM_EVENT				xSemaphoreGive( sem_event );

#define TEST_SEM_COUNTER			if( xSemaphoreTake(sem_counter,portMAX_DELAY )  == pdTRUE  )
#define GIVE_SEM_COUNTER			xSemaphoreGive( sem_counter );

#define TEST_SEM_STATE				if( xSemaphoreTake(sem_state,portMAX_DELAY ) == pdTRUE  )
#define GIVE_SEM_STATE				xSemaphoreGive( sem_state );

#define TEST_SEM_PARAMETER			if( xSemaphoreTake(sem_parameter, portMAX_DELAY )  == pdTRUE  )
#define GIVE_SEM_PARAMETER			xSemaphoreGive( sem_parameter );

#define YIELD(ms)					vTaskDelay( (portTickType)(ms / portTICK_RATE_MS) );

/*****************************   Variables   *******************************/
extern xSemaphoreHandle sem_common_pins;
extern xSemaphoreHandle sem_lcd_buffer;
extern xSemaphoreHandle sem_queue;
extern xSemaphoreHandle sem_event;
extern xSemaphoreHandle sem_counter;
extern xSemaphoreHandle sem_state;
extern xSemaphoreHandle sem_parameter;

#endif
