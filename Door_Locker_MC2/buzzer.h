/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the BUZZER driver
 *
 * Author: Abdelmonem Magdi
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

/* Static Config. */
#define BUZZER_PIN  PIN0_ID
#define BUZZER_PORT PORTA_ID

/* Functions Prototype*/
void BUZZER_init(void);
void Buzzer_on(void);
void Buzzer_off(void);

#endif /* BUZZER_H_ */
