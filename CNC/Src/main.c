/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "motor_control.h"
#include "stm_utils.h"

int ADC_is_on;

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */
	if(ADC_is_on)
		ADC_callback();
  /* USER CODE END SysTick_IRQn 1 */
}

void TIM6_DAC_IRQHandler(void) 
{
	  static bool calibrated = false;
	  static uint32_t debouncer = 0;
	  if(!calibrated)
		{
			if(calibrate_CNC())
			{
				debouncer++;
				if(debouncer > 100)
				{
					calibrated = true;
				}
			}
			else
			{
				debouncer = 0;
			}
		}
		else
		{
			PWM_control_callback();
		}
    TIM6->SR &= ~TIM_SR_UIF;        // Acknowledge the interrupt
}

int strEq(char* str1, char* str2, int count){
	int i = 0;
	for(i = 0; i <= count; i++){
		if(str1[i] != str2[i]){
			return 0;
		}
	}
	return 1;	
}

int atoi(char *str)
{
 int res = 0; // Initialize result
 // Iterate through all characters of input string and
 // update result
 for (int i = 0; str[i] != '\0'; ++i) {
     if (str[i]> '9' || str[i]<'0')
         return -1;
     res = res*10 + str[i] - '0';
 }

 // return result.
 return res;
}

int main(void)
{
  HAL_Init();
	SystemClock_Config();
	setup_motor_system();
	ADC_is_on = 1;
	int str_pos = 0;
	char buffer[128] = "";
	int x_axis = 0;
	
	while(true)
	{
		if(str_pos == 0 && ADC_is_on == 0){
			writeString("Enter axis (x, y), distance (0-255), current position (current_position), or toggle ADC (on, off):\r\n");
		}
		else if(str_pos == 0){
			writeString("Enter option for ADC(on, off) or current position (current_position):\r\n");
		}
		// Wait for character.
		while((USART1->ISR & USART_ISR_RXNE) == 0){}
		char c = USART1->RDR;
		singleChar(c);
			
		if(c == '\n' || c == '\r'){
				writeString("\r\n");
				buffer[str_pos] = 0;
				// Check word
				
				if(strEq(buffer, "off", str_pos)){
					ADC_is_on = 0;
				}
				else if(strEq(buffer, "on", str_pos)){
					ADC_is_on = 1;
				}
				else if(strEq(buffer, "current_position", str_pos)){
					print_position();
				}
				else if(ADC_is_on == 0){
					int new_position = atoi(buffer);
					if(strEq(buffer, "y", str_pos)){
						x_axis = 0;
					}
					else if(strEq(buffer, "x", str_pos)){
						x_axis = 1;
					}
					else if(new_position >= 0 && new_position <= 255){
							if(x_axis){
								USART_setX(new_position);
							}
							else{
								USART_setY(new_position);
							}
					}
				}
				else{
					writeString("Error invalid input!!!!\r\n");
				}
				str_pos = 0;
			}
			else if(c != '\r'){
				buffer[str_pos] = c;
				str_pos++;
		}
		HAL_Delay(10);
	}
	
}


