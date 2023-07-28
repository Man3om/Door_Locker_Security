/******************************************************************************
 *
 * Module: PWM
 *
 * File Name: pwm.c
 *
 * Description: Source file for the PWM driver
 *
 * Author: Abdelmonem Magdi
 *
 *******************************************************************************/
#include "pwm.h"
#include "gpio.h"
#include "common_macros.h"
#include <avr/io.h>
/*******************************************************************************
 * Description:Initialization PWM
 * Input: Duty Cycle
 * Return: Void
 *******************************************************************************/
void PWM_Timer0_Start(uint8 duty_cycle)
{
	uint8 targetTicks = (uint8)(((uint16)(duty_cycle * 255))/100) ;

	/*
	 * Fast PWM => FOC0 = 0 ,  WGM00:01 = 1
	 * Non-Inverting => COM01 = 1 , COM00 = 0
	 * Pre-scaler = 8 => CS00 = 0 , CS01 = 1 , CS02 = 0
	 * */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01) ;

	/* PB3 As output Pin (OC0)*/
	GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT);

	TCNT0 = 0 ; /* Initial Value */

	OCR0 = targetTicks; /* Compared Value */
}
