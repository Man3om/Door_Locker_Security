/**********************************************************************************
 *Description: Source File For Application Layer Code
 *
 *HMI_ECU (Human Interface)
 *
 *Author: Abdelmonem Magdi
 **********************************************************************************/
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timerOne.h"
#include <util/delay.h>
#include <avr/interrupt.h>

/*********************************************************************************
 *                            Important Macros                                   *
 ********************************************************************************/
#define MC2_Ready 0x01   /* MCU2 is Ready For Receiving */
#define OK        0x10   /* Pass is Right */
#define ERROR     0x00	 /* Pass is Wrong */
#define CHECK     0x11   /* Checking The Received Pass */
#define MOTOR    0x12    /* Control DOOR */
#define BUZZER    0x14   /* Turn On Buzzer */
#define CR_PASS   0x15   /* Create Pass */

/*********************************************************************************
 *                            Global Variables                                   *
 ********************************************************************************/
static uint8 g_count = 0 ;
static uint8 pass1[5] = {0};
static uint8 pass2[5] = {0};
static uint8 status = 0 ;
static uint8 count = 0 ;
static uint8 key = 0 ;
static uint8 i = 0 ;

/*********************************************************************************
 *                            Users Function                                  *
 ********************************************************************************/
void Timer(void) /* Every 1 Second Timer ISR Call This Function */
{
	g_count++ ;
}

/*
 * Description: Delay Function using Timer 1
 * Input: Number of Seconds
 * Return: Void
 */
void delay(uint8 second)
{
	/* Configure Timer With Desired Specifications */
	Timer1_ConfigType t_configure = {0 , 7812 , Fcpu1024 , CTC };
	Timer1_init(&t_configure); /* Start Timer */
	while(g_count != second); /* Waiting */
	Timer1_deInit(); /* Stop Timer */
	g_count = 0 ;
}

/*
 * Description: Function To handle Wrong Password
 * Input: Void
 * Return: Void
 */
void St_Error(void)
{
	count = 0 ;

	do
	{
		count++;

		LCD_clearScreen();
		LCD_displayString("PLZ Enter Pass:");
		LCD_moveCursor(1, 0);

		/* Enter The Password */
		for(i = 0 ; i < 5 ; i++)
		{
			key = KEYPAD_getPressedKey() ;
			pass1[i] = key ;
			LCD_displayCharacter('*');
			_delay_ms(500);
		}

		/* Waiting User to press The Enter Button */
		do
		{
			key = KEYPAD_getPressedKey() ;
			_delay_ms(500);
		} while(key != 13);

		UART_sendByte(CHECK); /* Inform Other MCU to Check The Pass */

		_delay_us(20);

		/* Send Password To 2nd MCU */
		for(i = 0 ; i < 5 ; i++)
		{
			UART_sendByte(pass1[i]);

			_delay_us(20);
		}

		status = UART_recieveByte(); /* Waiting Checking Result From 2nd MCU */

	}while((count != 2) && (status == ERROR));

	if((count == 2) && (status == ERROR))
	{
		UART_sendByte(BUZZER);
		LCD_clearScreen();
		LCD_displayString("System Locked");
		delay(60); /* Waiting For 60 Seconds */
	}
}

/*
 * Description: Function To send password through UART
 * Input: Array
 * Return: Void
 *
 */
void SendPass(uint8 *pass)
{
	for(i = 0 ; i < 5 ; i++)
	{
		UART_sendByte(pass[i]);

		_delay_us(20);
	}
}

/*********************************************************************************
 *                            Application Code                                   *
 ********************************************************************************/
int main(void)
{
	uint8 flag = 0 ;

	SREG |= (1<<7); /* Enable Global Interrupt */

	/* Configure The UART & Timer1 With Desired Specifications */
	UART_ConfigType u_configure = {BIT8 , Disable , ONE_Stop , 9600};

	/* Initialization Drivers */
	UART_init(&u_configure);
	LCD_init();

	Timer1_setCallBack(Timer); /* Setup CallBAck Function For Timer */

	UART_recieveByte(); /* Waiting For MCU2 To be Ready */

	while(1)
	{
		/* Create Password to The System */
		if(flag == 0)
		{
			LCD_clearScreen();
			LCD_displayString("PLZ Enter Pass:");
			LCD_moveCursor(1, 0);

			/* Enter The Password */
			for(i = 0 ; i < 5 ; i++)
			{
				key = KEYPAD_getPressedKey() ;
				pass1[i] = key ;
				LCD_displayCharacter('*');
				_delay_ms(500);
			}

			/* Waiting User to press The Enter Button */
			do
			{
				key = KEYPAD_getPressedKey() ;
				_delay_ms(500);
			} while(key != 13);

			LCD_clearScreen();
			LCD_displayString("Plz re-enter the");
			LCD_displayStringRowColumn(1, 0, "same Pass: ");

			/* Re-enter The same Password */
			for(i = 0 ; i < 5 ; i++)
			{
				key = KEYPAD_getPressedKey() ;
				pass2[i] = key ;
				LCD_displayCharacter('*');
				_delay_ms(500);
			}

			/* Waiting User to press The Enter Button */
			do
			{
				key = KEYPAD_getPressedKey() ;
				_delay_ms(500);
			} while(key != 13);

			/* Send 2 Passwords To 2nd MCU */
			SendPass(pass1);
			SendPass(pass2);

			status = UART_recieveByte();/* Waiting Checking Result From 2nd MCU */

			/* if 2 Passwords matched each other move to next phase */
			if(status == OK)
			{
				flag = 1;
			}
		}
		else if(flag == 1) /* Normal Phase Of System */
		{
			LCD_clearScreen();
			LCD_displayString("+ : Open Door");
			LCD_displayStringRowColumn(1, 0, "- : Change Pass");

			/* Waiting User to press (+ or -) Button */
			do
			{
				key = KEYPAD_getPressedKey() ;
				_delay_ms(500);
			} while((key != '+') && (key != '-'));

			if(key == '+') /* User Choose Open The Door */
			{
				LCD_clearScreen();
				LCD_displayString("Plz Enter Pass:");
				LCD_moveCursor(1, 0);

				/* Enter The Password */
				for(i = 0 ; i < 5 ; i++)
				{
					key = KEYPAD_getPressedKey() ;
					pass1[i] = key ;
					LCD_displayCharacter('*');
					_delay_ms(500);
				}

				/* Waiting User to press The Enter Button */
				do
				{
					key = KEYPAD_getPressedKey() ;
					_delay_ms(500);
				} while(key != 13);

				UART_sendByte(CHECK); /* Inform Other MCU to Check The Pass */

				_delay_us(20);

				/* Send Password To 2nd MCU */
				SendPass(pass1);

				status = UART_recieveByte();/* Waiting Checking Result From 2nd MCU */

				while(1)
				{
					if (status == OK) /* Right Password */
					{
						/* Door Unlocking */
						UART_sendByte(MOTOR);
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 1, "Door Unlocking");
						delay(15); /* Waiting For 15 Seconds */

						/* Door Holding */
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 1, "Door Holding");
						delay(3); /* Waiting For 3 Seconds */

						/* Door locking */
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 1, "Door Locking");
						delay(15); /* Waiting For 15 Seconds */

						break;
					}
					else if(status == ERROR) /* Wrong Password */
					{
						St_Error(); /* Call ERROR Function */

						if(status == ERROR)
						{
							break ;
						}
					}
				}
			}
			else if(key == '-') /* User Choose Change The Password */
			{
				LCD_clearScreen();
				LCD_displayString("Plz Enter Pass:");
				LCD_moveCursor(1, 0);

				/* Enter The Password */
				for(i = 0 ; i < 5 ; i++)
				{
					key = KEYPAD_getPressedKey() ;
					pass1[i] = key ;
					LCD_displayCharacter('*');
					_delay_ms(500);
				}

				/* Waiting User to press The Enter Button */
				do
				{
					key = KEYPAD_getPressedKey() ;
					_delay_ms(500);
				} while(key != 13);

				UART_sendByte(CHECK); /* Inform Other MCU to Check The Pass */

				_delay_us(20);

				/* Send Password To 2nd MCU */
				SendPass(pass1);

				status = UART_recieveByte();/* Waiting Checking Result From 2nd MCU */

				while(1)
				{
					if(status == OK) /* Right Password */
					{
						flag = 0 ;
						UART_sendByte(CR_PASS); /* Inform other MCU Current Phase Create Pass */
						break;
					}
					else /* Wrong Password */
					{
						St_Error(); /* Call ERROR Function */

						if(status == ERROR)
						{
							break ;
						}
					}
				}
			}
		}
	}
}
