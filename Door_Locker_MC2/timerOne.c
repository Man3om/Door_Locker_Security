/******************************************************************************
 *
 * Module: TIMER 1
 *
 * File Name: timerOne.c
 *
 * Description: Source file for the Timer 1 AVR driver
 *
 * Author: Abdelmonem Magdi
 *
 *******************************************************************************/
#include "timerOne.h"
#include <avr/interrupt.h>

static volatile void (*g_callBack_ptr)(void) = NULL_PTR ; /* Global Pointer to Function */

/*******************************************************************************
 * Description: Function to initialize the Timer driver
 * Input: pointer to the configuration structure with type Timer1_ConfigType
 * Return: Void
 *******************************************************************************/
void Timer1_init(const Timer1_ConfigType * Config_ptr)
{
	/* Non-PWM MODE */
	TCCR1A = (1<<FOC1A) | (1<<FOC1B) ;

	/* Configure Mode For Timer 1 , (WGM10:WGM11) Bits */
	TCCR1A = (TCCR1A & 0xFC) | (Config_ptr->mode & 0x03) ;

	/* Configure Mode For Timer 1 , (WGM12:WGM13) Bits */
	TCCR1B = (TCCR1B & 0xE7) |((Config_ptr->mode & 0x0C)<<1) ;

	TCNT1 = Config_ptr->initial_value ; /* Configure Initial Value For Timer 1 */

	/* Check The Mode Of Timer */
	if(Config_ptr->mode == Normal)
	{
		TIMSK = (1<<TOIE1) ; /* Enable Timer 1 Normal Mode Interrupt */
	}
	else if (Config_ptr->mode == CTC)
	{
#ifdef COMPARE1A
		/* Configure Compare Value in Compare1A Register */
		OCR1A = Config_ptr->compare_value ;

		TIMSK = (1<<OCIE1A); /* Enable Timer 1 Compare1A Mode Interrupt */
#endif

#ifdef COMPARE1B
		/* Configure Compare Value in Compare1A Register */
		OCR1B = Config_ptr->compare_value ;

		TIMSK = (1<<OCIE1B); /* Enable Timer 1 Compare1A Mode Interrupt */
#endif
	}

	/* Configure Pre-scaler For Timer 1 , (CS10:CS12) Bits */
	TCCR1B = (TCCR1B & 0xF8) | (Config_ptr->prescaler);
}

/*******************************************************************************
 * Description: Function to disable the Timer1
 * Input: Void
 * Return: Void
 *******************************************************************************/
void Timer1_deInit(void)
{
	TCCR1A = 0 ;

	TCCR1B = 0 ;

	TIMSK = 0 ;
}

/*******************************************************************************
 * Description: set the Call Back function address
 * Input: Void
 * Return: Void
 *******************************************************************************/
void Timer1_setCallBack(void (*a_ptr)(void))
{
	g_callBack_ptr = a_ptr ;
}

/*******************************************************************************
 *                    ISR (Interrupt Service Routines)                         *
 *******************************************************************************/
ISR(TIMER1_OVF_vect) /* ISR For Normal MODE */
{
	if(g_callBack_ptr != NULL_PTR)
	{
		(*g_callBack_ptr)() ;
	}
}

ISR(TIMER1_COMPA_vect) /* ISR For Compare1A Mode */
{
	if(g_callBack_ptr != NULL_PTR)
	{
		(*g_callBack_ptr)() ;
	}
}

ISR(TIMER1_COMPB_vect) /* ISR For Compare1B Mode */
{
	if(g_callBack_ptr != NULL_PTR)
	{
		(*g_callBack_ptr)() ;
	}
}
