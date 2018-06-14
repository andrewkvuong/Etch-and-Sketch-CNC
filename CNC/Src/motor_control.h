/*
	Pins used by other peripherals include:
	USART_TX = PA9
	USART_RX = PA10
*/

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
#include "stm32f0xx_hal.h"
#include "stm_utils.h"


#define GPIO_ADC_X 					GPIOC
#define ADC_X_PIN_NUM 			0
#define ADC_CH_X						ADC_CHSELR_CHSEL10

#define GPIO_ADC_Y 					GPIOC
#define ADC_Y_PIN_NUM 			3
#define ADC_CH_Y						ADC_CHSELR_CHSEL13

#define GPIO_PWM_X					GPIOC
#define PWM_X_PIN_NUM				9
#define GPIO_PWM_DIR_X			GPIOC
#define PWM_DIR_X_PIN_NUM		8

#define GPIO_PWM_Y					GPIOC
#define PWM_Y_PIN_NUM				7
#define GPIO_PWM_DIR_Y			GPIOC
#define PWM_DIR_Y_PIN_NUM		6

#define GPIO_CAL_X					GPIOC
#define CAL_X_PIN_NUM				5

#define GPIO_CAL_Y					GPIOC
#define CAL_Y_PIN_NUM				4

typedef enum
{
	direction_forward = 0,
	direction_backward = 1
} direction;


void USART_setX(uint32_t x);

void USART_setY(uint32_t y);

void print_position(void);

/* 
	Sets up the whole motor control system
*/
void setup_motor_system(void);
/*
	Function that gets called during SysTick interrupt handler
*/
void ADC_callback(void);
/*
	Function callback to adjust PWM
*/
void PWM_control_callback(void);
/*
	Sets up the GPIOs that will be turned on and off to control the motor speed.
*/
void setup_GPIOs_for_PWM(void);
/*
	Adjusts the position in the X direction
*/
void adjust_position_X(void);
/*
	Adjusts the position in the Y direction
*/
void adjust_position_Y(void);
/*
	Makes one step on the motor connected to PC9, either forward or backward
*/
void take_step_X(direction dir);
/*
	Makes one step on the motor connected to PC9, either forward or backward
*/
void take_step_Y(direction dir);
/*
	Takes an ADC reading and converts it into the desired steps set point
*/
int calculate_desired_steps(int ADC_reading);
/*
	Sets up and calibrates the ADC peripheral
*/
void setup_ADC(void);
/*
	Reads a channel on the ADC
*/
int Read_ADC_channel(uint32_t channel);
/*
	Sets up the GPIOs used for the buttons that are used for calibration
*/
void setup_cal_GPIOs(void);
/*
  Before starting the main program, calibrate the CNC setup so that its position is known
*/
bool calibrate_CNC(void);

#endif
