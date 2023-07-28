/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the BUZZER driver
 *
 * Author: Abdelmonem Magdi
 *
 *******************************************************************************/
#include "buzzer.h"
#include "gpio.h"

/********************************************************************************
 * Description: Initialize BUZEER
 * Input: Void
 * Return: Void
 *******************************************************************************/
void BUZZER_init(void)
{
	/* Setup Buzzer Pin as O/P Pin*/
	GPIO_setupPinDirection(BUZZER_PORT, BUZZER_PIN, PIN_OUTPUT);

	/* Turn Off Buzzer */
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);
}

/********************************************************************************
 * Description: Turn on Buzzer
 * Input: Void
 * Return: Void
 *******************************************************************************/
void Buzzer_on(void)
{
	/* Turn On Buzzer */
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_HIGH);
}

/********************************************************************************
 * Description: Turn off Buzzer
 * Input: Void
 * Return: Void
 *******************************************************************************/
void Buzzer_off(void)
{
	/* Turn Off Buzzer */
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);
}
