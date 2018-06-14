#include "main.h"
#include "stm_utils.h"
#include "stm32f0xx_hal.h"

void initLeds(void){
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; //Enable GPIOC for LEDs
	
	//Set PC6-PC9 to General purpose in/out
	GPIOC->MODER &= ~(0xFF << GPIO_MODER_MODER6_Pos);
	GPIOC->MODER |= 0x01 << GPIO_MODER_MODER6_Pos;
	GPIOC->MODER |= 0x01 << GPIO_MODER_MODER7_Pos;
	GPIOC->MODER |= 0x01 << GPIO_MODER_MODER8_Pos;
	GPIOC->MODER |= 0x01 << GPIO_MODER_MODER9_Pos;
	
		//Set PC6-PC9 to push-pull 
	GPIOC->OTYPER &= ~(0xF << 6); //Clear PC6-PC9 for push-pull mode 
	
	//Set PC6-PC9 to low_speed mode
	GPIOC->OSPEEDR &= ~(0xFF << GPIO_OSPEEDR_OSPEEDR6_Pos); // Clear PC6-PC9 for low-speed mode
	
		//Set no pull up/down for PC6-PC9
	GPIOC->PUPDR &= ~(0xFF << GPIO_PUPDR_PUPDR6_Pos);
	
	// Initialize LED states
	turn_off_LED('o');
	turn_off_LED('b');
	turn_off_LED('r');
	turn_off_LED('g');
}

bool turn_on_LED(char ch)
{
		switch(ch)
	{
		case 'r':
		case 'R':
			GPIOC->BSRR |= GPIO_BSRR_BS_6;
			break;
		case 'g':
		case 'G':
			GPIOC->BSRR |= GPIO_BSRR_BS_9;
			break;
		case 'b':
		case 'B':
			GPIOC->BSRR |= GPIO_BSRR_BS_7;
			break;
		case 'o':
		case 'O':
			GPIOC->BSRR |= GPIO_BSRR_BS_8;
			break;
		default:
			return 0;
	}
	return 1;
}

bool toggle_LED(char ch)
{
		switch(ch)
	{
		case 'r':
		case 'R':
			GPIOC->ODR ^= GPIO_ODR_6;
			break;
		case 'g':
		case 'G':
			GPIOC->ODR ^= GPIO_ODR_9;
			break;
		case 'b':
		case 'B':
			GPIOC->ODR ^= GPIO_ODR_7;
			break;
		case 'o':
		case 'O':
			GPIOC->ODR ^= GPIO_ODR_8;
			break;
		default:
			return 0;
	}
	return 1;
}

bool turn_off_LED(char ch)
{
		switch(ch)
	{
		case 'r':
		case 'R':
			GPIOC->BSRR |= GPIO_BSRR_BR_6;
			break;
		case 'g':
		case 'G':
			GPIOC->BSRR |= GPIO_BSRR_BR_9;
			break;
		case 'b':
		case 'B':
			GPIOC->BSRR |= GPIO_BSRR_BR_7;
			break;
		case 'o':
		case 'O':
			GPIOC->BSRR |= GPIO_BSRR_BR_8;
			break;
		default:
			return 0;
	}
	return 1;
}

void setup_USART(void){
	
	// Enable USART1 Clock
	//PA9 TX, PA10 RX
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	// Enable GPIOA Clock
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	// Set alternate function mode 10
	GPIOA->MODER |= GPIO_MODER_MODER9;
	GPIOA->MODER |= GPIO_MODER_MODER10;
	GPIOA->MODER &= ~(GPIO_MODER_MODER9_0);
	GPIOA->MODER &= ~(GPIO_MODER_MODER10_0);
	
	// Set AFRH 0001
	GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9);
	GPIOA->AFR[1] |= (0x1 << GPIO_AFRH_AFSEL9_Pos); 
	GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL10);
	GPIOA->AFR[1] |= (0x1 << GPIO_AFRH_AFSEL10_Pos);
	
	USART1->BRR = HAL_RCC_GetHCLKFreq()/115200;
	USART1->CR1 |= USART_CR1_UE;
	USART1->CR1 |= USART_CR1_RE;
	USART1->CR1 |= USART_CR1_TE;
	
	// Push-pull
	GPIOA->OTYPER = 0x00000;
	
	//High Speed
	GPIOA->OSPEEDR = GPIO_OSPEEDR_OSPEEDR9;
	GPIOA->OSPEEDR = GPIO_OSPEEDR_OSPEEDR10;
	
	// No pull resistors
	GPIOA->PUPDR = 0x0;
}

void singleChar(char c){
	while((USART1->ISR & USART_ISR_TXE) == 0){}
	USART1->TDR = c;
}

void writeString(char* c){
	int i = 0;
	while(1){
		singleChar(c[i]);
		i++;
		if(c[i] == 0){
			break;
		}
	}
}

void  button_init(void) {
    // Initialize PA0 for button input
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;                                          // Enable peripheral clock to GPIOA
    GPIOA->MODER &= ~(GPIO_MODER_MODER0_0 | GPIO_MODER_MODER0_1);               // Set PA0 to input
}

void setup_timer_TIM6(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	
	// 6k HZ
	TIM6->PSC = 28; // TODO: Change this!
  TIM6->ARR = 100;// TODO: Change this!
	
	// 10 Hz
	//TIM6->PSC = 7999; // TODO: Change this!
  //TIM6->ARR = 400; // TODO: Change this!

  TIM6->DIER |= TIM_DIER_UIE;             // Enable update event interrupt
  TIM6->CR1 |= TIM_CR1_CEN;               // Enable Timer

  NVIC_EnableIRQ(TIM6_DAC_IRQn);          // Enable interrupt in NVIC
  NVIC_SetPriority(TIM6_DAC_IRQn,2);
}

void int_to_str(char* str, uint32_t len, uint32_t val)
{
  char i;

  for(i=1; i<=len; i++)
  {
    str[len-i] = (char) ((val % 10UL) + '0');
    val/=10;
  }

  str[i-1] = '\0';
}


// STM Code

/* USER CODE END */
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
