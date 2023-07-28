/**********************************************************************************
 *Description: Source File For Application Layer Code
 *
 *Control_ECU
 *
 *Author: Abdelmonem Magdi
 **********************************************************************************/
#include "buzzer.h"
#include "dc_motor.h"
#include "timerOne.h"
#include "twi.h"
#include "uart.h"
#include "external_eeprom.h"
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
	uint8 order = 0 , flag = 0 , pass1[5] = {0} , pass2[5] = {0} , i = 0 ;
	uint8 equality = 1  , data = 0 ;

	SREG |= (1<<7); /* Enable Global Interrupt */

	/* Configure The UART ,Timer1 & TWI With Desired Specifications */
	UART_ConfigType u_configure = {BIT8 , Disable , ONE_Stop , 9600};
	Timer1_ConfigType t_configure = {0 , 7812 , Fcpu1024 , CTC };
	TWI_ConfigType i_configure = { 1 , 2 };

	/* Initialization Drivers */
	UART_init(&u_configure);
	TWI_init(&i_configure);
	BUZZER_init();
	DcMotor_init();

	Timer1_setCallBack(Timer); /* Setup CallBAck Function For Timer */

	UART_sendByte(MC2_Ready); /* Send to 1st MCU That MCU2 is Ready To Receive */

	while(1)
	{
		if(flag == 0) /* Enter here if user Creates Password */
		{
			equality = 1 ; /* Reset The Equality variable */

			/* Receive 2 Passwords From HMI MC */
			for(i = 0 ; i < 5 ; i++)
			{
				pass1[i]  = UART_recieveByte();
			}

			for(i = 0 ; i < 5 ; i++)
			{
				pass2[i]  = UART_recieveByte();
			}

			/* Check Matching of 2 Passwords */
			for(i = 0 ; i < 5 ; i++)
			{
				if(pass1[i] != pass2[i])
				{
					equality = 0 ;

					break ;
				}
			}

			if(equality == 1)
			{
				/* Store the Password inside EEPROM */
				for(i = 0 ; i < 5 ; i++)
				{
					EEPROM_writeByte((0x100+i), pass1[i]);
					_delay_ms(10);
				}

				UART_sendByte(OK); /* Send To HMI MC 2 passwords similar */

				flag = 1 ;
			}
			else
			{
				UART_sendByte(ERROR); /* Send To HMI MC 2 passwords Not similar */
			}
		}
		else if(flag == 1) /* Enter Here if The System is in Normal Mode  */
		{
			order = UART_recieveByte();

			switch(order)
			{
			case CHECK:
				equality = 1 ; /* Reset The Equality variable */

				/* Receive The Password From HMI MC */
				for(i = 0 ; i < 5 ; i++)
				{
					pass1[i]  = UART_recieveByte();
				}

				/* Check Matching of 2 Passwords (User & System) */
				for(i = 0 ; i < 5 ; i++)
				{
					EEPROM_readByte((0x100+i), &data); /* Read From EEPROM */

					if(data != pass1[i])
					{
						equality = 0 ;

						break ;
					}

					_delay_ms(10);
				}

				if(equality == 1)
				{
					UART_sendByte(OK);  /* The Password by User Correct */
				}
				else
				{
					UART_sendByte(ERROR); /* The Password by User not Correct */
				}

				break;

			case UNLOCK:

				DcMotor_Rotate(100, CW); /* Unlocking The Door*/
				Timer1_init(&t_configure); /* Start Timer */
				while(g_count != 15); /* Waiting For 15 Seconds */
				Timer1_deInit();
				g_count = 0 ;

				DcMotor_Rotate(0, OFF); /* Hold */
				Timer1_init(&t_configure); /* Start Timer */
				while(g_count != 3); /* Waiting For 3 Seconds */
				Timer1_deInit();
				g_count = 0 ;

				break;

			case LOCK:

				DcMotor_Rotate(100, CCW); /* Locking The Door */
				Timer1_init(&t_configure); /* Start Timer */
				while(g_count != 15); /* Waiting For 15 Seconds */
				Timer1_deInit();
				g_count = 0 ;
				DcMotor_Rotate(0, OFF); /* Stop Motor */

				break;

			case BUZZER:

				Buzzer_on(); /* Turn On Buzzer */
				Timer1_init(&t_configure); /* Start Timer */
				while(g_count != 60); /* Waiting For 60 Seconds */
				Timer1_deInit();
				g_count = 0 ;
				Buzzer_off(); /* Turn Off Buzzer */

				break;

			case CR_PASS:
				flag = 0 ;
				break ;
			}
		}
	}
}

