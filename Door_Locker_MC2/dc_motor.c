/******************************************************************************
 *
 * Module: DC_Motor
 *
 * File Name: dc_motor.c
 *
 * Description: Source file for the DC_Motor driver
 *
 * Author: Abdelmonem Magdi
 *
 *******************************************************************************/
#include "pwm.h"
#include "dc_motor.h"
#include "gpio.h"

/*******************************************************************************
 * Description:Initialization Motor PINS
 * Input: Void
 * Return: Void
 *******************************************************************************/
void DcMotor_init(void)
{
	/* Define Motor Pins As Output */
	GPIO_setupPinDirection(PORT_MOTOR, IN1_MOTOR, PIN_OUTPUT);
	GPIO_setupPinDirection(PORT_MOTOR, IN2_MOTOR, PIN_OUTPUT);

	/* Stop Motor in The Beginning */
	GPIO_writePin(PORT_MOTOR, IN1_MOTOR, LOGIC_LOW);
	GPIO_writePin(PORT_MOTOR, IN2_MOTOR, LOGIC_LOW);
}

/********************************************************************************
 * Description: Control Motor's Speed and Direction
 * Input: Duty Cycle and Motor Direction
 * Return: Void
 *********************************************************************************/
void DcMotor_Rotate(uint8 speed , DcMotor_State state)
{
	switch(state)
	{
	case CW:
		GPIO_writePin(PORT_MOTOR, IN1_MOTOR, LOGIC_LOW);
		GPIO_writePin(PORT_MOTOR, IN2_MOTOR, LOGIC_HIGH);
		break;
	case CCW:
		GPIO_writePin(PORT_MOTOR, IN1_MOTOR, LOGIC_HIGH);
		GPIO_writePin(PORT_MOTOR, IN2_MOTOR, LOGIC_LOW);
		break;
	case OFF:
		GPIO_writePin(PORT_MOTOR, IN1_MOTOR, LOGIC_LOW);
		GPIO_writePin(PORT_MOTOR, IN2_MOTOR, LOGIC_LOW);
		break;
	}

	PWM_Timer0_Start(speed);
}
