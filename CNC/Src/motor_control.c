/*
	This file is the only file that should need to be edited. 	
	See the precompiler directives in the header file to see the pins used by the motor

*/
#include "motor_control.h"
#include "stm32f0xx_hal.h"
#include "stm_utils.h"

volatile int desired_steps_X;
volatile int desired_steps_Y;
volatile int current_step_count_X;
volatile int current_step_count_Y;

void setup_motor_system()
{
	setup_USART();
	setup_GPIOs_for_PWM();
	setup_cal_GPIOs();
	setup_ADC();
	desired_steps_X = 0;
	desired_steps_Y = 0;
	setup_timer_TIM6();
}

void adjust_desired(uint32_t calculated, volatile int *desired, uint32_t* num_above, uint32_t* num_below)
{
	static const uint32_t threshold = 1000;
	static const uint32_t iterations_threshold = 100;
	if( *desired > calculated && *desired > (calculated + threshold))
	{
		*num_below = 0;
		*num_above = *num_above + 1;
		if(*num_above > iterations_threshold)
		{
			*desired = calculated;
		}
	}
	else if(*desired < calculated && *desired < (calculated - threshold))
	{
	  *num_above = 0;
		*num_below = *num_below + 1;
		if(*num_below > iterations_threshold)
		{
			*desired = calculated;
		}
	}
}

void ADC_callback(void)
{
	uint32_t calculated_X = calculate_desired_steps(Read_ADC_channel(ADC_CH_X));
	uint32_t calculated_Y = calculate_desired_steps(Read_ADC_channel(ADC_CH_Y));
	static uint32_t above_threshold_X = 0;
	static uint32_t below_threshold_X = 0;
	static uint32_t above_threshold_Y = 0;
	static uint32_t below_threshold_Y = 0;
  adjust_desired(calculated_X, &desired_steps_X, &above_threshold_X, &below_threshold_X);
	adjust_desired(calculated_Y, &desired_steps_Y, &above_threshold_Y, &below_threshold_Y);
}

void USART_setX(uint32_t x)
{
	desired_steps_X = calculate_desired_steps(x);
}

void USART_setY(uint32_t y)
{
	desired_steps_Y = calculate_desired_steps(y);
}

void print_position(void){
		static const int str_len = 16;
		char str[str_len];
		int_to_str(str, str_len, current_step_count_X/75);
		writeString("Current steps X: ");
		writeString(str);
		writeString("\r\n");
		int_to_str(str, str_len, current_step_count_Y/75);
		writeString("Current steps Y: ");
		writeString(str);
		writeString("\r\n\r\n");
}

void PWM_control_callback(void)
{
	adjust_position_X();
	adjust_position_Y();
}

void adjust_position_X()
{
	//static uint32_t count = 0;
	//static int current_step_count_X = 0;
	if(desired_steps_X > current_step_count_X)
	{
		take_step_X(direction_forward);
		current_step_count_X++;
	}
	else if(desired_steps_X < current_step_count_X)
	{
		take_step_X(direction_backward);
		current_step_count_X--;
	}
	else
	{
		//If desired_steps == current_step_count, do nothing
	}
	
	// DEBUG CODE
	/*if(5000 == count++)
	{
		static const int str_len = 16;
		char str[str_len];
		int_to_str(str, str_len, desired_steps_X);
		writeString("Desired steps X: ");
		writeString(str);
		writeString("\t\t");
		int_to_str(str, str_len, current_step_count_X);
		writeString("Current steps X: ");
		writeString(str);
		writeString("\r\n");
		count = 0;
	}*/
	
}

void adjust_position_Y()
{
	//static uint32_t count = 0;
	//static int current_step_count_Y = 0;
	if(desired_steps_Y > current_step_count_Y)
	{
		take_step_Y(direction_forward);
		current_step_count_Y++;
	}
	else if(desired_steps_Y < current_step_count_Y)
	{
		take_step_Y(direction_backward);
		current_step_count_Y--;
	}
	else
	{
		//If desired_steps == current_step_count, do nothing
	}
	
	// DEBUG CODE
	/*if(5000 == count++)
	{
		static const int str_len = 16;
		char str[str_len];
		int_to_str(str, str_len, desired_steps_Y);
		writeString("Desired steps Y: ");
		writeString(str);
		writeString("\t\t");
		int_to_str(str, str_len, current_step_count_Y);
		writeString("Current steps Y: ");
		writeString(str);
		writeString("\r\n\r\n");
		count = 0;
	}*/
}

int calculate_desired_steps(int ADC_reading)
{
	return ADC_reading*75;
}

void take_step_X(direction dir)
{
	// Set direction of motor movement
	if(dir == direction_forward)
	{
		TURN_ON(GPIO_PWM_DIR_X, PWM_DIR_X_PIN_NUM);
	}
	else
	{
		TURN_OFF(GPIO_PWM_DIR_X, PWM_DIR_X_PIN_NUM);
	}
	
	// Send out pulse to move motor one step
	static volatile int count = 0;
	TURN_ON(GPIO_PWM_X, PWM_X_PIN_NUM);
	while(100 > count++){} // Short delay to give a pulse
	count=0;
	TURN_OFF(GPIO_PWM_X, PWM_X_PIN_NUM); 
}

void take_step_Y(direction dir)
{
	// Set direction of motor movement
	if(dir == direction_forward)
	{
		TURN_ON(GPIO_PWM_DIR_Y, PWM_DIR_Y_PIN_NUM);
	}
	else
	{
		TURN_OFF(GPIO_PWM_DIR_Y, PWM_DIR_Y_PIN_NUM);
	}
	
	// Send out pulse to move motor one step
	static volatile int count = 0;
	TURN_ON(GPIO_PWM_Y, PWM_Y_PIN_NUM);
	while(100 > count++){} // Short delay to give a pulse
	count=0;
	TURN_OFF(GPIO_PWM_Y, PWM_Y_PIN_NUM); 
}

void setup_GPIOs_for_PWM()
{	
	RCC->AHBENR |= RCC_GPIO_EN(GPIO_PWM_X); //Enable GPIOs for PWM
	RCC->AHBENR |= RCC_GPIO_EN(GPIO_PWM_DIR_X);
	RCC->AHBENR |= RCC_GPIO_EN(GPIO_PWM_Y);
	RCC->AHBENR |= RCC_GPIO_EN(GPIO_PWM_DIR_Y);
	
	//Set PC6-PC9 to General purpose in/out
	GPIO_PWM_X->MODER = (GPIO_PWM_X->MODER & ~(0x3 << (2 * PWM_X_PIN_NUM)))
											| (0x01 << (2 * PWM_X_PIN_NUM));
	GPIO_PWM_DIR_X->MODER = (GPIO_PWM_DIR_X->MODER & ~(0x3 << (2 * PWM_DIR_X_PIN_NUM))) 
													| (0x01 << (2 * PWM_DIR_X_PIN_NUM));
	GPIO_PWM_Y->MODER = (GPIO_PWM_Y->MODER & ~(0x3 << (2 * PWM_Y_PIN_NUM))) 
											| (0x01 << (2 * PWM_Y_PIN_NUM));
	GPIO_PWM_DIR_Y->MODER = (GPIO_PWM_DIR_Y->MODER & ~(0x3 << (2 * PWM_DIR_Y_PIN_NUM))) 
													| (0x01 << (2 * PWM_DIR_Y_PIN_NUM));
	
		//Set PC6-PC9 to push-pull 
	GPIO_PWM_X->OTYPER &= ~(0x1 << PWM_X_PIN_NUM); 
	GPIO_PWM_DIR_X->OTYPER &= ~(0x1 << PWM_DIR_X_PIN_NUM); 
	GPIO_PWM_Y->OTYPER &= ~(0x1 << PWM_Y_PIN_NUM); 
	GPIO_PWM_DIR_Y->OTYPER &= ~(0x1 << PWM_DIR_Y_PIN_NUM);
	
	//Set PC6-PC9 to low_speed mode
	GPIO_PWM_X->OSPEEDR &= ~(0x3 << (2 * PWM_X_PIN_NUM));
  GPIO_PWM_DIR_X->OSPEEDR &= ~(0x3 << (2 * PWM_DIR_X_PIN_NUM));
  GPIO_PWM_Y->OSPEEDR &= ~(0x3 << (2 * PWM_Y_PIN_NUM));
  GPIO_PWM_DIR_Y->OSPEEDR &= ~(0x3 << (2 * PWM_DIR_Y_PIN_NUM));	
	
		//Set no pull up/down for PC6-PC9
	GPIO_PWM_X->PUPDR &= ~(0x3 << (2 * PWM_X_PIN_NUM));
	GPIO_PWM_DIR_X->PUPDR &= ~(0x3 << (2 * PWM_DIR_X_PIN_NUM));
	GPIO_PWM_Y->PUPDR &= ~(0x3 << (2 * PWM_Y_PIN_NUM));
	GPIO_PWM_DIR_Y->PUPDR &= ~(0x3 << (2 * PWM_DIR_Y_PIN_NUM));	
	
	// Initialize GPIO states
	TURN_OFF(GPIO_PWM_X, PWM_X_PIN_NUM);
	TURN_OFF(GPIO_PWM_DIR_X, PWM_DIR_X_PIN_NUM);
	TURN_OFF(GPIO_PWM_Y, PWM_Y_PIN_NUM);
	TURN_OFF(GPIO_PWM_DIR_Y, PWM_DIR_Y_PIN_NUM);
}

void setup_ADC()
{
	RCC->AHBENR |= RCC_GPIO_EN(GPIO_ADC_X); //Enable GPIOs for ADC
	RCC->AHBENR |= RCC_GPIO_EN(GPIO_ADC_Y);
	
	//Set pins to General purpose in/out
	GPIO_ADC_X->MODER |= (0x3 << (2 * ADC_X_PIN_NUM));
	GPIO_ADC_Y->MODER |= (0x3 << (2 * ADC_Y_PIN_NUM));
	
	
	//Set no pull up/down
	GPIO_ADC_X->PUPDR &= ~(0x3 << (2 * ADC_X_PIN_NUM));
	GPIO_ADC_Y->PUPDR &= ~(0x3 << (2 * ADC_Y_PIN_NUM));
	
	
	// Enable ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
	
	// Set resolution to 8-bit
	ADC1->CFGR1 &= ~ADC_CFGR1_RES;
	ADC1->CFGR1 |= ADC_CFGR1_RES_1;
	
	// Clear Cont. Conversion mode
	ADC1->CFGR1 &= ~ADC_CFGR1_CONT;
	ADC1->CFGR1 |= ADC_CFGR1_DISCEN;
	ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN;
	
	// Hardware triger disabled
	ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN;
	
	// Disable all input channels
	ADC1->CHSELR &= ~ADC_CHSELR_CHSEL;
	
	// Calibrate
	ADC1->CR |= ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL){}
	
	//Enable ADC
	ADC1->CR |= ADC_CR_ADEN;
		
	ADC1->ISR |= ADC_ISR_ADRDY;
	while((ADC1->ISR & ADC_ISR_ADRDY) == 0){}
			
}

int Read_ADC_channel(uint32_t channel)
{
	int ret_val = 0;
	// Disable all input channels
	ADC1->CHSELR &= ~ADC_CHSELR_CHSEL;
		
	// Select/Enable the input channel for the X direction
	ADC1->CHSELR |= channel;
	ADC1->CR |= ADC_CR_ADSTART; // Start read
	while(!(ADC1->ISR & ADC_ISR_EOSEQ)); // Wait for read to end
	ret_val = ADC1->DR;
	ADC1->CHSELR &= ~ADC_CHSELR_CHSEL;
	return ret_val;
}

void setup_cal_GPIOs()
{	
	RCC->AHBENR |= RCC_GPIO_EN(GPIO_CAL_X);
	GPIO_CAL_X->MODER &= ~(0x3 << (2 * CAL_X_PIN_NUM)); // Input mode
	GPIO_CAL_X->OSPEEDR &= ~(0x3 << (2 * CAL_X_PIN_NUM)); // Low speed
	GPIO_CAL_X->PUPDR &= ~(0x3 << (2 * CAL_X_PIN_NUM));
	GPIO_CAL_X->PUPDR |= (0x2 << (2 * CAL_X_PIN_NUM)); // Pull down resistor
	
	RCC->AHBENR |= RCC_GPIO_EN(GPIO_CAL_Y);
	GPIO_CAL_Y->MODER &= ~(0x3 << (2 * CAL_Y_PIN_NUM)); // Input mode
	GPIO_CAL_Y->OSPEEDR &= ~(0x3 << (2 * CAL_Y_PIN_NUM)); // Low speed
	GPIO_CAL_Y->PUPDR &= ~(0x3 << (2 * CAL_Y_PIN_NUM));
	GPIO_CAL_Y->PUPDR |= (0x2 << (2 * CAL_Y_PIN_NUM)); // Pull down resistor
	
}

bool calibrate_CNC()
{
	bool calibrated = true;
	if(!READ_PIN(GPIO_CAL_X, CAL_X_PIN_NUM))
	{
		take_step_X(direction_backward);
	  calibrated = false;
	}
	if(!READ_PIN(GPIO_CAL_Y, CAL_Y_PIN_NUM))
	{
		take_step_Y(direction_backward);
		calibrated = false;
	}
	return calibrated;
}
