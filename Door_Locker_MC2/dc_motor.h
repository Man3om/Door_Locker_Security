/******************************************************************************
 *
 * Module: DC_Motor
 *
 * File Name: DC_Motor.h
 *
 * Description: Header file for the DC_Motor driver
 *
 * Author: Abdelmonem Magdi
 *
 *******************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"

/* Static Config. */
#define PORT_MOTOR PORTD_ID

#define IN1_MOTOR  PIN5_ID
#define IN2_MOTOR  PIN6_ID

/* Data Types */
typedef enum
{
	OFF , CW , CCW
}DcMotor_State;

/* Functions Prototype */
void DcMotor_init(void);
void DcMotor_Rotate(uint8 speed , DcMotor_State state);

#endif /* DC_MOTOR_H_ */
