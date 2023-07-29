# Door_Locker_Security
Developing a system to unlock a door using a password.

## Software Architecture Layers:

### HMI MCU:

APP  (App)

HAL  (KEYPAD - LCD)

MCAL (GPIO - UART - TIMER1)


### Control MCU:

APP  (App)

HAL  (DC MOTOR - BUZZER - EXTERNAL EEPROM)

MCAL (GPIO - UART - TIMER1 - PWM - TWI)
