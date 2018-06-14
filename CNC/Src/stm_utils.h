#ifndef STM_UTILS_H
#define STM_UTILS_H
#include "stm32f0xx_hal.h"

typedef unsigned char bool;
#define true 1
#define false 0

#define TURN_ON(GPIO, PIN) (GPIO->BSRR |= (GPIO_BSRR_BS_0 << PIN))
#define TURN_OFF(GPIO, PIN) (GPIO->BSRR |= (GPIO_BSRR_BR_0 << PIN))
#define TOGGLE_PIN(GPIO, PIN) (GPIO->ODR ^= (0x1 << PIN))
#define READ_PIN(GPIO, PIN) (GPIO->IDR & (0x1 << PIN))
#define RCC_GPIO_EN(GPIO) ((GPIO == GPIOA) ? RCC_AHBENR_GPIOAEN : ((GPIO == GPIOB) ? RCC_AHBENR_GPIOBEN : RCC_AHBENR_GPIOCEN))

void SystemClock_Config(void);

/*
	Sets up the GPIOC pins used for the LEDs and turns the LEDs off.
  Uses PC6-PC9
*/
void initLeds(void);
/*
	Sets up the USART peripheral to run using PA9 and PA10
*/
void setup_USART(void);
/*
	Turns on an LED with a character representation. Returns true
  if valid input, false otherwise. Valid inputs correspond to LED
  colors: R, r, G, g, B, b, O, o 
*/
bool turn_on_LED(char ch);
/*
	Turns off an LED with a character representation. Returns true
  if valid input, false otherwise. Valid inputs correspond to LED
  colors: R, r, G, g, B, b, O, o 
*/
bool turn_off_LED(char ch);
/*
	Toggles an LED with a character representation. Returns true
  if valid input, false otherwise. Valid inputs correspond to LED
  colors: R, r, G, g, B, b, O, o 
*/
bool toggle_LED(char ch);
/*
	Writes a string out the USART cable. Can be used for console debugging
*/
void writeString(char* c);

void singleChar(char c);

void int_to_str(char* str, uint32_t len, uint32_t val);

/*
	Sets up the User Button. Uses PA0.
*/
void  button_init(void);
/*
	Sets up the TIM6 timer peripheral.
*/
void setup_timer_TIM6(void);

#endif
