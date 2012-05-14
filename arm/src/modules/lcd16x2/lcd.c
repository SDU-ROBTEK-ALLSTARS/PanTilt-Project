/*****************************************************************************
 * University of Southern Denmark
 * Robotics Engineering Spring 2012
 * Pan Tilt Project
 *
 * MODULENAME.: lcd
 *
 *****************************************************************************/

/***************************** Include files *******************************/
#include "lcd.h"

/*****************************   Variables   *******************************/
LCD_BUFFER	display_buffer[COL_SIZE][ROW_SIZE];

INT8U col_pointer = 0;
INT8U row_pointer = 0;

/*****************************   Main task   *******************************/

void lcd_task(void *pvParameters)
{
	INT8U	display_updated = FALSE;

	while(TRUE)
	{
		display_updated = FALSE;
		do
		{
			if (col_pointer >= COL_SIZE)
			{
				// next line
				col_pointer = 0;
				row_pointer++;

				if (row_pointer >= ROW_SIZE)
				{
					// Goto start of display.
					col_pointer = 0;
					row_pointer = 0;
					display_updated = TRUE;
				}
			}
			TEST_SEM_LCD_BUFFER
			{
				if (display_buffer[col_pointer][row_pointer].display_dirty_bit == TRUE)
				{
					TEST_SEM_COMMON_PINS
					{
						display_buffer_goto_xy(col_pointer, row_pointer);
						lcd_delay_us(40);
						lcd_rs_high();			// write character
						if(display_buffer[col_pointer][row_pointer].display_blink_bit && state(POP,BLINK_S))
							lcd_write_4bit_mode(BLINK_CHAR);
						else
							lcd_write_4bit_mode(display_buffer[col_pointer][row_pointer].display_char);
					}
					GIVE_SEM_COMMON_PINS
				}
				display_buffer[col_pointer][row_pointer].display_dirty_bit = FALSE;
				display_updated = TRUE;
				col_pointer++;
			}
			GIVE_SEM_LCD_BUFFER
		}
		while (!display_updated);

		YIELD(YIELD_TIME_LCD_T)
	}
}

void blink_task(void *pvParameters)
{
	while(TRUE)
	{
		state(TOGGLE,BLINK_S);
		YIELD(YIELD_TIME_BLINK_T);
	}
}
/*****************************   Functions   *******************************/
BOOLEAN display_buffer_set_blink(INT8U col_p, INT8U row_p, INT8U size)
{
	BOOLEAN return_value = TRUE;

	while( size )
	{

		TEST_SEM_LCD_BUFFER
		{
			size--;
			display_buffer[col_p+size][row_p].display_blink_bit = TRUE;
			GIVE_SEM_LCD_BUFFER
		}
	}

	return return_value;
}

BOOLEAN display_buffer_clear_blink(void)
{
	BOOLEAN return_value = TRUE;
	INT8U i,j;

	TEST_SEM_LCD_BUFFER
	{
		for(j=0 ; j < ROW_SIZE ; j++)
			for(i=0 ; i < COL_SIZE ; i++)
				display_buffer[i][j].display_blink_bit = FALSE;
		GIVE_SEM_LCD_BUFFER
	}
	return return_value;
}

BOOLEAN display_buffer_write_char(INT8U col_p, INT8U row_p, INT8U ch)
{
	BOOLEAN return_value = TRUE;

	TEST_SEM_LCD_BUFFER
	{
		display_buffer[col_p][row_p].display_char = ch;
		display_buffer[col_p][row_p].display_dirty_bit = TRUE;
		GIVE_SEM_LCD_BUFFER
	}

	return return_value;
}

BOOLEAN display_buffer_write_string(INT8U col_p, INT8U row_p, const char *str)
{
	BOOLEAN return_value = TRUE;

	while( *str )
	{

		TEST_SEM_LCD_BUFFER
		{
			display_buffer[col_p][row_p].display_char = *str;
			display_buffer[col_p][row_p].display_dirty_bit = TRUE;
			col_p++;
			str++;
			GIVE_SEM_LCD_BUFFER
		}
	}

	return return_value; 
}
BOOLEAN display_buffer_write_number(INT8U col_p,INT8U row_p,INT8S digits,INT32U num)
{
	BOOLEAN return_value = TRUE;

	do
	{
		TEST_SEM_LCD_BUFFER
		{
			display_buffer[col_p+digits-1][row_p].display_char = ( num % 10 ) + '0'; //display digit
			display_buffer[col_p+digits-1][row_p].display_dirty_bit = TRUE;
			digits--;
			GIVE_SEM_LCD_BUFFER
		}
	}
	while(( num /= 10 ) > 0 && digits > 0);											//remove digit

	TEST_SEM_LCD_BUFFER
	{
		while(digits > 0)														//run through unused digits
		{
			display_buffer[col_p+digits-1][row_p].display_char = NO_DIGIT;
			digits--;
		}
		GIVE_SEM_LCD_BUFFER
	}

	return return_value;
}

BOOLEAN display_buffer_write_decimal(INT8U col_p,INT8U row_p,INT8S digits,INT8U decimals,INT32S num)
{
	BOOLEAN return_value = TRUE;
	INT8S dot;


	TEST_SEM_LCD_BUFFER
	{
		if(num < 0)
		{
			num = 0 - num;												//absolute value
			display_buffer[col_p][row_p].display_char = SIGN_NEGATIVE;	//insert minus sign
		}
		else if(num >= 0)
			display_buffer[col_p][row_p].display_char = SIGN_POSITIVE;	//insert positive sign

		display_buffer[col_p][row_p].display_dirty_bit = TRUE;
		col_p++;
		GIVE_SEM_LCD_BUFFER
	}

	digits--;															//decrease since first place is sign
	dot = digits;
	do
	{
		TEST_SEM_LCD_BUFFER
		{
			display_buffer[col_p+digits-1][row_p].display_char = ( num % 10 ) + '0'; //display digit
			display_buffer[col_p+digits-1][row_p].display_dirty_bit = TRUE;
			digits--;
			if((dot-digits) == decimals)
			{
				display_buffer[col_p+digits-1][row_p].display_char = DECIMAL_SEPARATOR;
				display_buffer[col_p+digits-1][row_p].display_dirty_bit = TRUE;
				digits--;
			}
			GIVE_SEM_LCD_BUFFER
		}
	}
	while(( num /= 10 ) > 0 && digits > 0);											//remove digit

	TEST_SEM_LCD_BUFFER
	{
		while(digits > 0)														//run through unused digits
		{
			display_buffer[col_p+digits-1][row_p].display_char = NO_DIGIT;
			digits--;
		}
		GIVE_SEM_LCD_BUFFER
	}

	return return_value;
}

BOOLEAN lcd_function(INT8U function)
{
	BOOLEAN	return_value = TRUE;  
	{
		lcd_rs_low();
		lcd_write_4bit_mode(function);
	}
	return return_value;
}

BOOLEAN display_buffer_goto_xy(INT8U col_p, INT8U row_p)
{
	INT8U pos;
	INT8U return_value = FALSE;

	if ((col_p < COL_SIZE) && (row_p < ROW_SIZE))
	{
		pos = row_p*0x40 + col_p;
		pos |= 0x80;
		return_value = lcd_function(pos);
	}
	return return_value;
}

void lcd_init_display(void)
{
	INT8U col;
	INT8U row;

	lcd_delay_us(15000);
	lcd_rs_low();						// write control bytes
	lcd_write_port(0x03);  	// attention!
	lcd_strobe();
	lcd_delay_us(5000);
	lcd_strobe();
	lcd_delay_us(100);
	lcd_strobe();
	lcd_delay_us(5000);
	lcd_write_port(0x2);			// set 4 bit mode
	lcd_strobe();
	lcd_delay_us(40);
	lcd_write_4bit_mode(0x28);	// 4 bit mode, 1/16 duty, 5x8 font
	lcd_delay_us(40);
	lcd_write_4bit_mode(0x08);	// display off
	lcd_delay_us(40);
	lcd_write_4bit_mode(0x0C);	// display on, blink cursor on
	lcd_delay_us(40);
	lcd_write_4bit_mode(0x06);	// entry mode
	lcd_delay_us(40);
	lcd_write_4bit_mode(0x01);	// Home
	lcd_delay_us(40);

	// clear display

	{
		for (row=0; row < ROW_SIZE; row++)
		{
			for (col=0; col < COL_SIZE; col++)
			{
				display_buffer[col][row].display_char = ' ';
				display_buffer[col][row].display_dirty_bit = FALSE;
			}
		}
	}
}

void display_init(void)
{
	lcd_port_setup();
	lcd_init_display();
}

/****************************** End Of Module *******************************/
