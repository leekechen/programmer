/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define OE_Pin GPIO_PIN_2
#define OE_GPIO_Port GPIOE
#define RESET_A_Pin GPIO_PIN_3
#define RESET_A_GPIO_Port GPIOE
#define VS_ON_Pin GPIO_PIN_4
#define VS_ON_GPIO_Port GPIOE
#define VDD_ON_Pin GPIO_PIN_5
#define VDD_ON_GPIO_Port GPIOE
#define VPP_ON_Pin GPIO_PIN_6
#define VPP_ON_GPIO_Port GPIOE
#define BUZZER_Pin GPIO_PIN_13
#define BUZZER_GPIO_Port GPIOC
#define KEY_MODE_Pin GPIO_PIN_14
#define KEY_MODE_GPIO_Port GPIOC
#define KEY_MODE_EXTI_IRQn EXTI15_10_IRQn
#define ADC_VPP_Pin GPIO_PIN_0
#define ADC_VPP_GPIO_Port GPIOC
#define ADC_VDD_Pin GPIO_PIN_1
#define ADC_VDD_GPIO_Port GPIOC
#define ADC_VS_Pin GPIO_PIN_2
#define ADC_VS_GPIO_Port GPIOC
#define ADC_VS_B_Pin GPIO_PIN_3
#define ADC_VS_B_GPIO_Port GPIOC
#define ADC_PIN1_Pin GPIO_PIN_0
#define ADC_PIN1_GPIO_Port GPIOA
#define ADC_PIN2_Pin GPIO_PIN_1
#define ADC_PIN2_GPIO_Port GPIOA
#define ADC_PIN3_Pin GPIO_PIN_2
#define ADC_PIN3_GPIO_Port GPIOA
#define ADC_PIN4_Pin GPIO_PIN_3
#define ADC_PIN4_GPIO_Port GPIOA
#define DAC_VDD_Pin GPIO_PIN_4
#define DAC_VDD_GPIO_Port GPIOA
#define DAC_VPP_Pin GPIO_PIN_5
#define DAC_VPP_GPIO_Port GPIOA
#define ADC_PIN5_Pin GPIO_PIN_6
#define ADC_PIN5_GPIO_Port GPIOA
#define ADC_PIN6_Pin GPIO_PIN_7
#define ADC_PIN6_GPIO_Port GPIOA
#define ADC_PIN7_Pin GPIO_PIN_4
#define ADC_PIN7_GPIO_Port GPIOC
#define ADC_PIN8_Pin GPIO_PIN_5
#define ADC_PIN8_GPIO_Port GPIOC
#define ADC_12V_Pin GPIO_PIN_0
#define ADC_12V_GPIO_Port GPIOB
#define ADC_5V_Pin GPIO_PIN_1
#define ADC_5V_GPIO_Port GPIOB
#define GND_CTRL_Pin GPIO_PIN_7
#define GND_CTRL_GPIO_Port GPIOE
#define VS_ON_B_Pin GPIO_PIN_8
#define VS_ON_B_GPIO_Port GPIOE
#define D6_Pin GPIO_PIN_8
#define D6_GPIO_Port GPIOD
#define D7_Pin GPIO_PIN_9
#define D7_GPIO_Port GPIOD
#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOD
#define LCD_DCX_Pin GPIO_PIN_12
#define LCD_DCX_GPIO_Port GPIOD
#define LCD_RESET_Pin GPIO_PIN_13
#define LCD_RESET_GPIO_Port GPIOD
#define BACK_LIGHT_Pin GPIO_PIN_14
#define BACK_LIGHT_GPIO_Port GPIOD
#define START_Pin GPIO_PIN_15
#define START_GPIO_Port GPIOD
#define START_EXTI_IRQn EXTI15_10_IRQn
#define BUSY_Pin GPIO_PIN_6
#define BUSY_GPIO_Port GPIOC
#define NG_Pin GPIO_PIN_7
#define NG_GPIO_Port GPIOC
#define OK_Pin GPIO_PIN_8
#define OK_GPIO_Port GPIOC
#define D5_Pin GPIO_PIN_9
#define D5_GPIO_Port GPIOC
#define D4_Pin GPIO_PIN_8
#define D4_GPIO_Port GPIOA
#define USB_EN_Pin GPIO_PIN_15
#define USB_EN_GPIO_Port GPIOA
#define CLK_A_Pin GPIO_PIN_10
#define CLK_A_GPIO_Port GPIOC
#define MISO_A_Pin GPIO_PIN_11
#define MISO_A_GPIO_Port GPIOC
#define MOSI_A_Pin GPIO_PIN_12
#define MOSI_A_GPIO_Port GPIOC
#define D0_Pin GPIO_PIN_0
#define D0_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_1
#define D1_GPIO_Port GPIOD
#define D2_Pin GPIO_PIN_2
#define D2_GPIO_Port GPIOD
#define D3_Pin GPIO_PIN_3
#define D3_GPIO_Port GPIOD
#define IO1_A_Pin GPIO_PIN_5
#define IO1_A_GPIO_Port GPIOD
#define IO2_A_Pin GPIO_PIN_6
#define IO2_A_GPIO_Port GPIOD
#define IO3_A_Pin GPIO_PIN_7
#define IO3_A_GPIO_Port GPIOD

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
