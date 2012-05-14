/*****************************************************************************
 * University of Southern Denmark
 * Pan Tilt Project
 *
 * MODULENAME.: numpad
 *****************************************************************************/

/***************************** Include files *******************************/
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "numpad.h"

/*****************************    Defines    *******************************/
//Port B masked bit-number defines
#define PB2   (1<<2)    // Bit 2
#define PB3   (1<<3)    // Bit 3
#define PB4   (1<<4)    // Bit 4

//Port D masked bit-number defines
#define PD2   4   // Bit 2
#define PD3   8   // Bit 3
#define PD4   16    // Bit 4
#define PD5   32    // Bit 5

//Matrix key pad defines x-posistions, left -> right
#define X1    PB2
#define X2    PB3
#define X3    PB4

//Matrix key pad defines y-posistions, top -> buttom
#define Y1    PD5
#define Y2    PD4
#define Y3    PD3
#define Y4    PD2

/*****************************   Constants   *******************************/

/*****************************   Main task   *******************************/

void numpad_task(void *pvParameters)
{
	static INT8U x = 1,prel = 0;
	static INT8U y = 0;
	static INT8U key = 0;
	static INT8U key_state = NOT_PRESSED;

	while( TRUE )
	{
		switch( key_state )
		{
		case NOT_PRESSED:
			TEST_SEM_COMMON_PINS
			{
				select_x( x );							//set current column high
				GIVE_SEM_COMMON_PINS
				key_state = TEST_IF_PRESSED;			//go to next state
			}
		case TEST_IF_PRESSED:
			TEST_SEM_COMMON_PINS
			{
				y = GPIO_PORTD_DATA_R & 0x3C;			//read input pins
				GIVE_SEM_COMMON_PINS

				if( y )										//if button is pressed
				{
					y = row( y );							//parse to numeric value of row
					key = key_catch( x, y );				//parse column and row into char
					queue(PUSH,NUMPAD_Q,key);				//push char to input queue
					prel = 0;
					key_state = PRESSED;			 		//go to next state
				}
				else
				{
					x++;									//increase x
					if( x > 3 )								//roll over
						x = 1;
					key_state = NOT_PRESSED;				//go to state 1
				}
			}
			break;

		case PRESSED:
			TEST_SEM_COMMON_PINS
			{
				select_x( x );								//set current column high
				GIVE_SEM_COMMON_PINS
				key_state = TEST_IF_RELEASED;				//go to next state
			}
		case TEST_IF_RELEASED:
			TEST_SEM_COMMON_PINS
			{
				y = GPIO_PORTD_DATA_R & 0x3C;				//read input pins
				GIVE_SEM_COMMON_PINS

				if( !y )									//if button is still pressed
				{
					prel++;
					if(prel>30)
					{
					x++;									//increase x
					if( x > 3 )								//roll over
						x = 1;
					key_state = NOT_PRESSED;				//go to state 1
					}
				}
			}
			break;
		}
		YIELD(YIELD_TIME_NUMPAD_T)
	}
}

/*****************************   Functions   *******************************/

void key_port_init(void)
{
	// Enable the GPIO ports that is used for Matrix Keypad.
	SYSCTL_RCGC2_R |= ( SYSCTL_RCGC2_GPIOB | SYSCTL_RCGC2_GPIOD );

	SysCtlDelay(3);

	// Set the direction as input.
	GPIO_PORTD_DIR_R &= ~( PD2 | PD3 | PD4 | PD5 );

	// Set the direction as output.
	GPIO_PORTB_DIR_R |= ( PB2 | PB3 | PB4 );

	// Enable the GPIO pins for digital function.
	GPIO_PORTD_DEN_R |= ( PD2 | PD3 | PD4 | PD5 );
	GPIO_PORTB_DEN_R |= ( PB2 | PB3 | PB4 );

	// Enable internal pull-up.
	GPIO_PORTB_PUR_R |= ( PB2 | PB3 | PB4 );
}

void select_x( INT8U x )
{
	switch( x )
	{
	case 1:
		GPIO_PORTB_DATA_R |= X1;
		GPIO_PORTB_DATA_R &= ~X2;
		GPIO_PORTB_DATA_R &= ~X3;
		break;
	case 2:
		GPIO_PORTB_DATA_R &= ~X1;
		GPIO_PORTB_DATA_R |= X2;
		GPIO_PORTB_DATA_R &= ~X3;
		break;
	case 3:
		GPIO_PORTB_DATA_R &= ~X1;
		GPIO_PORTB_DATA_R &= ~X2;
		GPIO_PORTB_DATA_R |= X3;
		break;
	}
}


INT8U row( INT8U y )
{
	INT8U result = 0;

	switch( y )
	{
	case Y1: result = 1; break;
	case Y2: result = 2; break;
	case Y3: result = 3; break;
	case Y4: result = 4; break;
	default: result = 0; break;
	}
	return( result );
}

INT8U key_catch(INT8U x, INT8U y )
{
	const INT8U matrix[3][4] = {{'1','4','7','*'},
			{'2','5','8','0'},
			{'3','6','9','#'}};

	return( matrix[x-1][y-1] );

}

void key_init(void)
{
	key_port_init();
}

//void parser(void *pvParameters)
//{
//	INIT_SEM_NUMPAD_QUEUE
//	static INT8U ch;
//	static INT8U pos_x = 0;
//	static INT8U pos_y = 0;
//
//	mode(SET,DREH_M,COUNT);
//
//	while(TRUE)
//	{
//		TAKE_SEM_NUMPAD_QUEUE
//		TEST_SEM_NUMPAD_QUEUE
//			ch = queue(POP,NUMPAD_Q,0);
//		GIVE_SEM_NUMPAD_QUEUE
//
//		if(ch)
//		{
//			if(display_buffer_write_char(pos_x,pos_y,ch) == TRUE)
//			{
//				pos_x++;
//				if (pos_x >= 15)
//				{
//					pos_x = 0;
//					pos_y++;
//					if(pos_y >= 2)
//						pos_y = 0;
//				}
//			}
//			else
//				queue(PUSH,NUMPAD_Q,ch);
//		}
//		YIELD(5)
//	}
//}

/****************************** End Of Module *******************************/
