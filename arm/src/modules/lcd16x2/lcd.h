/*****************************************************************************
 * University of Southern Denmark
 * Pan Tilt Project
 *
 * MODULENAME.: lcd
 *****************************************************************************/

/*****************************   Change Log   ********************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
* 120407  LBL	Added mutexes
* 120420  LBL	Cleaned up Goofy code
* 120505  LBL   Added blink functionality
*
*****************************************************************************/
#ifndef _LCD_H
#define _LCD_H

/***************************** Include files *******************************/
#include "maindefs.h"
#include "os.h"
#include "emp_type.h"
#include "itc/itc.h"
#include "lcd_port.h"

/*****************************    Defines    *******************************/
// LCD character size
#define COL_SIZE				16
#define ROW_SIZE				2
#define SEM_MAX_DELAY			(( portTickType ) 10)

//LCD Function definemens
#define	CLEAR_DISPLAY		0x01
#define BLINK_ON			0x0D
#define DISPLAY_ON			0x0C

//numbers
#define SIGN_POSITIVE		' '
#define SIGN_NEGATIVE		'-'
#define NO_DIGIT			' '
#define DECIMAL_SEPARATOR	'.'
#define BLINK_CHAR			0xFF
/*****************************   Typedefs   *******************************/
typedef struct
{
	INT8U	display_char;
	BOOLEAN	display_dirty_bit;
	BOOLEAN display_blink_bit;
} LCD_BUFFER ;

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/
void lcd_task(void *pvParameters);
void blink_task(void *pvParameters);
BOOLEAN display_buffer_set_blink(INT8U col_p, INT8U row_p, INT8U size);
BOOLEAN display_buffer_clear_blink(void);
BOOLEAN display_buffer_write_char(INT8U col_p, INT8U row_p, INT8U ch);
BOOLEAN display_buffer_write_string(INT8U col_p, INT8U row_p, const char *str);
BOOLEAN display_buffer_write_number(INT8U col_p,INT8U row_p ,INT8S digits, INT32U num);
BOOLEAN display_buffer_write_decimal(INT8U col_p,INT8U row_p,INT8S digits,INT8U decimals,INT32S num);
BOOLEAN lcd_function(INT8U function);
BOOLEAN display_buffer_goto_xy(INT8U col_p, INT8U row_p);
void lcd_init_display(void);
void display_init(void);

/****************************** End Of Module *******************************/
#endif
