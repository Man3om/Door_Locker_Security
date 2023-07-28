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
#define MC2_Ready 0x01
#define OK        0x10
#define ERROR     0x00
#define CHECK     0x11
#define UNLOCK    0x12
#define LOCK      0x13
#define BUZZER    0x14
#define CR_PASS   0x15

/*********************************************************************************
 *                            Global Variables                                   *
 ********************************************************************************/
uint8 g_count = 0 ;

/*********************************************************************************
 *                            CallBack Function                                  *
 ********************************************************************************/
void Timer(void) /* Every 1 Second Timer ISR Call This Function */
{
	g_count++ ;
}

/*********************************************************************************
 *                            Application Code                                   *
 ********************************************************************************/
int main(void)
{
	uint8 key = 0 , pass1[5] = {0} , pass2[5] = {0} ,i = 0 , status = 0;
	uint8 flag = 0 , count = 0 ;

	SREG |= (1<<7); /* Enable Global Interrupt */

	/* Configure The UART & Timer1 With Desired Specifications */
	UART_ConfigType u_configure = {BIT8 , Disable , ONE_Stop , 9600};
	Timer1_ConfigType t_configure = {0 , 7812 , Fcpu1024 , CTC };

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
			for(i = 0 ; i < 5 ; i++)
			{
				UART_sendByte(pass1[i]);

				_delay_ms(20);
			}

			for(i = 0 ; i < 5 ; i++)
			{
				UART_sendByte(pass2[i]);

				_delay_ms(20);
			}

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

				UART_sendByte(CHECK); /* */

				_delay_ms(20);

				/* Send Password To 2nd MCU */
				for(i = 0 ; i < 5 ; i++)
				{
					UART_sendByte(pass1[i]);

					_delay_ms(20);
				}

				status = UART_recieveByte();/* Waiting Checking Result From 2nd MCU */

				while(1)
				{
					if (status == OK) /* Right Password */
					{
						/* Door Unlocking */
						UART_sendByte(UNLOCK);
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 1, "Door Unlocking");
						Timer1_init(&t_configure); /* Start Timer */
						while(g_count != 15); /* Waiting For 15 Seconds */
						Timer1_deInit();
						g_count = 0 ;

						/* Door Holding */
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 1, "Door Holding");
						Timer1_init(&t_configure); /* Start Timer */
						while(g_count != 3); /* Waiting For 3 Seconds */
						Timer1_deInit();
						g_count = 0 ;

						/* Door locking */
						UART_sendByte(LOCK);
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 1, "Door Locking");
						Timer1_init(&t_configure); /* Start Timer */
						while(g_count != 15); /* Waiting For 15 Seconds */
						Timer1_deInit();
						g_count = 0 ;

						break;
					}
					else if(status == ERROR) /* Wrong Password */
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

							UART_sendByte(CHECK); /* */

							_delay_ms(20);

							/* Send Password To 2nd MCU */
							for(i = 0 ; i < 5 ; i++)
							{
								UART_sendByte(pass1[i]);

								_delay_ms(20);
							}

							status = UART_recieveByte();

						}while((count != 2) && (status == ERROR));

						if((count == 2) && (status == ERROR))
						{
							UART_sendByte(BUZZER);
							LCD_clearScreen();
							LCD_displayString("System Locked");
							Timer1_init(&t_configure); /* Start Timer */
							while(g_count != 60); /* Waiting For 60 Seconds */
							Timer1_deInit();
							g_count = 0 ;

							break;
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

				UART_sendByte(CHECK); /* */

				_delay_ms(20);

				/* Send Password To 2nd MCU */
				for(i = 0 ; i < 5 ; i++)
				{
					UART_sendByte(pass1[i]);

					_delay_ms(20);
				}

				status = UART_recieveByte();/* Waiting Checking Result From 2nd MCU */

				while(1)
				{
					if(status == OK) /* Right Password */
					{
						flag = 0 ;
						UART_sendByte(CR_PASS);
						break;
					}
					else /* Wrong Password */
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

							UART_sendByte(CHECK); /* */

							_delay_ms(20);

							/* Send Password To 2nd MCU */
							for(i = 0 ; i < 5 ; i++)
							{
								UART_sendByte(pass1[i]);

								_delay_ms(20);
							}

							status = UART_recieveByte();

						}while((count != 2) && (status == ERROR));

						if((count == 2) && (status == ERROR))
						{
							UART_sendByte(BUZZER);
							LCD_clearScreen();
							LCD_displayString("System Locked");
							Timer1_init(&t_configure); /* Start Timer */
							while(g_count != 60); /* Waiting For 60 Seconds */
							Timer1_deInit();
							g_count = 0 ;

							break;
						}
					}
				}
			}
		}
	}
}
