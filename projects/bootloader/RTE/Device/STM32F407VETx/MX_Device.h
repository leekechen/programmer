/******************************************************************************
 * File Name   : MX_Device.h
 * Date        : 09/01/2019 11:35:53
 * Description : STM32Cube MX parameter definitions
 * Note        : This file is generated by STM32CubeMX (DO NOT EDIT!)
 ******************************************************************************/

#ifndef __MX_DEVICE_H
#define __MX_DEVICE_H

/*---------------------------- Clock Configuration ---------------------------*/

#define MX_LSI_VALUE                            32000
#define MX_LSE_VALUE                            32768
#define MX_HSI_VALUE                            16000000
#define MX_HSE_VALUE                            8000000
#define MX_EXTERNAL_CLOCK_VALUE                 12288000
#define MX_PLLCLKFreq_Value                     168000000
#define MX_SYSCLKFreq_VALUE                     168000000
#define MX_HCLKFreq_Value                       168000000
#define MX_FCLKCortexFreq_Value                 168000000
#define MX_CortexFreq_Value                     168000000
#define MX_AHBFreq_Value                        168000000
#define MX_APB1Freq_Value                       42000000
#define MX_APB2Freq_Value                       84000000
#define MX_APB1TimFreq_Value                    84000000
#define MX_APB2TimFreq_Value                    168000000
#define MX_48MHZClocksFreq_Value                48000000
#define MX_EthernetFreq_Value                   168000000
#define MX_I2SClocksFreq_Value                  192000000
#define MX_RTCFreq_Value                        32000
#define MX_WatchDogFreq_Value                   32000
#define MX_MCO1PinFreq_Value                    16000000
#define MX_MCO2PinFreq_Value                    168000000

/*-------------------------------- ADC1       --------------------------------*/

#define MX_ADC1                                 1

/* GPIO Configuration */

/* Pin PA0-WKUP */
#define MX_ADCx_IN0_Pin                         PA0_WKUP
#define MX_ADCx_IN0_GPIOx                       GPIOA
#define MX_ADCx_IN0_GPIO_PuPd                   GPIO_NOPULL
#define MX_ADC_PIN1                             ADCx_IN0
#define MX_ADCx_IN0_GPIO_Pin                    GPIO_PIN_0
#define MX_ADCx_IN0_GPIO_Mode                   GPIO_MODE_ANALOG

/* Pin PC5 */
#define MX_ADCx_IN15_Pin                        PC5
#define MX_ADCx_IN15_GPIOx                      GPIOC
#define MX_ADCx_IN15_GPIO_PuPd                  GPIO_NOPULL
#define MX_ADC_PIN8                             ADCx_IN15
#define MX_ADCx_IN15_GPIO_Pin                   GPIO_PIN_5
#define MX_ADCx_IN15_GPIO_Mode                  GPIO_MODE_ANALOG

/* Pin PC1 */
#define MX_ADCx_IN11_Pin                        PC1
#define MX_ADCx_IN11_GPIOx                      GPIOC
#define MX_ADCx_IN11_GPIO_PuPd                  GPIO_NOPULL
#define MX_ADC_VDD                              ADCx_IN11
#define MX_ADCx_IN11_GPIO_Pin                   GPIO_PIN_1
#define MX_ADCx_IN11_GPIO_Mode                  GPIO_MODE_ANALOG

/* Pin PC2 */
#define MX_ADCx_IN12_Pin                        PC2
#define MX_ADCx_IN12_GPIOx                      GPIOC
#define MX_ADCx_IN12_GPIO_PuPd                  GPIO_NOPULL
#define MX_ADC_VS                               ADCx_IN12
#define MX_ADCx_IN12_GPIO_Pin                   GPIO_PIN_2
#define MX_ADCx_IN12_GPIO_Mode                  GPIO_MODE_ANALOG

/* Pin PC3 */
#define MX_ADCx_IN13_Pin                        PC3
#define MX_ADCx_IN13_GPIOx                      GPIOC
#define MX_ADCx_IN13_GPIO_PuPd                  GPIO_NOPULL
#define MX_ADC_VS_B                             ADCx_IN13
#define MX_ADCx_IN13_GPIO_Pin                   GPIO_PIN_3
#define MX_ADCx_IN13_GPIO_Mode                  GPIO_MODE_ANALOG

/* Pin PA6 */
#define MX_ADCx_IN6_Pin                         PA6
#define MX_ADCx_IN6_GPIOx                       GPIOA
#define MX_ADCx_IN6_GPIO_PuPd                   GPIO_NOPULL
#define MX_ADC_PIN5                             ADCx_IN6
#define MX_ADCx_IN6_GPIO_Pin                    GPIO_PIN_6
#define MX_ADCx_IN6_GPIO_Mode                   GPIO_MODE_ANALOG

/* Pin PA7 */
#define MX_ADCx_IN7_Pin                         PA7
#define MX_ADCx_IN7_GPIOx                       GPIOA
#define MX_ADCx_IN7_GPIO_PuPd                   GPIO_NOPULL
#define MX_ADC_PIN6                             ADCx_IN7
#define MX_ADCx_IN7_GPIO_Pin                    GPIO_PIN_7
#define MX_ADCx_IN7_GPIO_Mode                   GPIO_MODE_ANALOG

/* Pin PC4 */
#define MX_ADCx_IN14_Pin                        PC4
#define MX_ADCx_IN14_GPIOx                      GPIOC
#define MX_ADCx_IN14_GPIO_PuPd                  GPIO_NOPULL
#define MX_ADC_PIN7                             ADCx_IN14
#define MX_ADCx_IN14_GPIO_Pin                   GPIO_PIN_4
#define MX_ADCx_IN14_GPIO_Mode                  GPIO_MODE_ANALOG

/* Pin PB0 */
#define MX_ADCx_IN8_Pin                         PB0
#define MX_ADCx_IN8_GPIOx                       GPIOB
#define MX_ADCx_IN8_GPIO_PuPd                   GPIO_NOPULL
#define MX_ADC_12V                              ADCx_IN8
#define MX_ADCx_IN8_GPIO_Pin                    GPIO_PIN_0
#define MX_ADCx_IN8_GPIO_Mode                   GPIO_MODE_ANALOG

/* Pin PB1 */
#define MX_ADCx_IN9_Pin                         PB1
#define MX_ADCx_IN9_GPIOx                       GPIOB
#define MX_ADCx_IN9_GPIO_PuPd                   GPIO_NOPULL
#define MX_ADC_5V                               ADCx_IN9
#define MX_ADCx_IN9_GPIO_Pin                    GPIO_PIN_1
#define MX_ADCx_IN9_GPIO_Mode                   GPIO_MODE_ANALOG

/* Pin PC0 */
#define MX_ADCx_IN10_Pin                        PC0
#define MX_ADCx_IN10_GPIOx                      GPIOC
#define MX_ADCx_IN10_GPIO_PuPd                  GPIO_NOPULL
#define MX_ADC_VPP                              ADCx_IN10
#define MX_ADCx_IN10_GPIO_Pin                   GPIO_PIN_0
#define MX_ADCx_IN10_GPIO_Mode                  GPIO_MODE_ANALOG

/*-------------------------------- ADC2       --------------------------------*/

#define MX_ADC2                                 1

/* GPIO Configuration */

/* Pin PA1 */
#define MX_ADCx_IN1_Pin                         PA1
#define MX_ADCx_IN1_GPIOx                       GPIOA
#define MX_ADCx_IN1_GPIO_PuPd                   GPIO_NOPULL
#define MX_ADC_PIN2                             ADCx_IN1
#define MX_ADCx_IN1_GPIO_Pin                    GPIO_PIN_1
#define MX_ADCx_IN1_GPIO_Mode                   GPIO_MODE_ANALOG

/*-------------------------------- ADC3       --------------------------------*/

#define MX_ADC3                                 1

/* GPIO Configuration */

/* Pin PA2 */
#define MX_ADCx_IN2_Pin                         PA2
#define MX_ADCx_IN2_GPIOx                       GPIOA
#define MX_ADCx_IN2_GPIO_PuPd                   GPIO_NOPULL
#define MX_ADC_PIN3                             ADCx_IN2
#define MX_ADCx_IN2_GPIO_Pin                    GPIO_PIN_2
#define MX_ADCx_IN2_GPIO_Mode                   GPIO_MODE_ANALOG

/* Pin PA3 */
#define MX_ADCx_IN3_Pin                         PA3
#define MX_ADCx_IN3_GPIOx                       GPIOA
#define MX_ADCx_IN3_GPIO_PuPd                   GPIO_NOPULL
#define MX_ADC_PIN4                             ADCx_IN3
#define MX_ADCx_IN3_GPIO_Pin                    GPIO_PIN_3
#define MX_ADCx_IN3_GPIO_Mode                   GPIO_MODE_ANALOG

/*-------------------------------- CRC        --------------------------------*/

#define MX_CRC                                  1

/* GPIO Configuration */

/*-------------------------------- DAC        --------------------------------*/

#define MX_DAC                                  1

/* GPIO Configuration */

/* Pin PA5 */
#define MX_COMP_DAC2_group_Pin                  PA5
#define MX_COMP_DAC2_group_GPIOx                GPIOA
#define MX_COMP_DAC2_group_GPIO_PuPd            GPIO_NOPULL
#define MX_DAC_VPP                              COMP_DAC2_group
#define MX_COMP_DAC2_group_GPIO_Pin             GPIO_PIN_5
#define MX_COMP_DAC2_group_GPIO_Mode            GPIO_MODE_ANALOG

/* Pin PA4 */
#define MX_COMP_DAC1_group_Pin                  PA4
#define MX_COMP_DAC1_group_GPIOx                GPIOA
#define MX_COMP_DAC1_group_GPIO_PuPd            GPIO_NOPULL
#define MX_DAC_VDD                              COMP_DAC1_group
#define MX_COMP_DAC1_group_GPIO_Pin             GPIO_PIN_4
#define MX_COMP_DAC1_group_GPIO_Mode            GPIO_MODE_ANALOG

/*-------------------------------- I2C1       --------------------------------*/

#define MX_I2C1                                 1

/* GPIO Configuration */

/* Pin PB6 */
#define MX_I2C1_SCL_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_I2C1_SCL_Pin                         PB6
#define MX_I2C1_SCL_GPIOx                       GPIOB
#define MX_I2C1_SCL_GPIO_PuPdOD                 GPIO_PULLUP
#define MX_I2C1_SCL_GPIO_Pin                    GPIO_PIN_6
#define MX_I2C1_SCL_GPIO_AF                     GPIO_AF4_I2C1
#define MX_I2C1_SCL_GPIO_Mode                   GPIO_MODE_AF_OD

/* Pin PB7 */
#define MX_I2C1_SDA_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_I2C1_SDA_Pin                         PB7
#define MX_I2C1_SDA_GPIOx                       GPIOB
#define MX_I2C1_SDA_GPIO_PuPdOD                 GPIO_PULLUP
#define MX_I2C1_SDA_GPIO_Pin                    GPIO_PIN_7
#define MX_I2C1_SDA_GPIO_AF                     GPIO_AF4_I2C1
#define MX_I2C1_SDA_GPIO_Mode                   GPIO_MODE_AF_OD

/*-------------------------------- I2C2       --------------------------------*/

#define MX_I2C2                                 1

/* GPIO Configuration */

/* Pin PB11 */
#define MX_I2C2_SDA_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_I2C2_SDA_Pin                         PB11
#define MX_I2C2_SDA_GPIOx                       GPIOB
#define MX_I2C2_SDA_GPIO_PuPdOD                 GPIO_PULLUP
#define MX_I2C2_SDA_GPIO_Pin                    GPIO_PIN_11
#define MX_I2C2_SDA_GPIO_AF                     GPIO_AF4_I2C2
#define MX_I2C2_SDA_GPIO_Mode                   GPIO_MODE_AF_OD

/* Pin PB10 */
#define MX_I2C2_SCL_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_I2C2_SCL_Pin                         PB10
#define MX_I2C2_SCL_GPIOx                       GPIOB
#define MX_I2C2_SCL_GPIO_PuPdOD                 GPIO_PULLUP
#define MX_I2C2_SCL_GPIO_Pin                    GPIO_PIN_10
#define MX_I2C2_SCL_GPIO_AF                     GPIO_AF4_I2C2
#define MX_I2C2_SCL_GPIO_Mode                   GPIO_MODE_AF_OD

/*-------------------------------- SPI1       --------------------------------*/

#define MX_SPI1                                 1

/* GPIO Configuration */

/* Pin PB4 */
#define MX_SPI1_MISO_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI1_MISO_Pin                        PB4
#define MX_SPI1_MISO_GPIOx                      GPIOB
#define MX_SPI1_MISO_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI1_MISO_GPIO_Pin                   GPIO_PIN_4
#define MX_SPI1_MISO_GPIO_AF                    GPIO_AF5_SPI1
#define MX_SPI1_MISO_GPIO_Mode                  GPIO_MODE_AF_PP

/* Pin PB5 */
#define MX_SPI1_MOSI_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI1_MOSI_Pin                        PB5
#define MX_SPI1_MOSI_GPIOx                      GPIOB
#define MX_SPI1_MOSI_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI1_MOSI_GPIO_Pin                   GPIO_PIN_5
#define MX_SPI1_MOSI_GPIO_AF                    GPIO_AF5_SPI1
#define MX_SPI1_MOSI_GPIO_Mode                  GPIO_MODE_AF_PP

/* Pin PB3 */
#define MX_SPI1_SCK_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI1_SCK_Pin                         PB3
#define MX_SPI1_SCK_GPIOx                       GPIOB
#define MX_SPI1_SCK_GPIO_PuPd                   GPIO_NOPULL
#define MX_SPI1_SCK_GPIO_Pin                    GPIO_PIN_3
#define MX_SPI1_SCK_GPIO_AF                     GPIO_AF5_SPI1
#define MX_SPI1_SCK_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PB12 */
#define MX_SPI1_NSS_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI1_NSS_Pin                         PB12
#define MX_SPI1_NSS_GPIOx                       GPIOB
#define MX_SPI1_NSS_GPIO_PuPd                   GPIO_NOPULL
#define MX_SPI1_NSS_GPIO_Pin                    GPIO_PIN_12
#define MX_SPI1_NSS_GPIO_AF                     GPIO_AF5_SPI2
#define MX_SPI1_NSS_GPIO_Mode                   GPIO_MODE_AF_PP


/*-------------------------------- SPI2       --------------------------------*/

#define MX_SPI2                                 1

/* GPIO Configuration */

/* Pin PB13 */
#define MX_SPI2_SCK_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI2_SCK_Pin                         PB13
#define MX_SPI2_SCK_GPIOx                       GPIOB
#define MX_SPI2_SCK_GPIO_PuPd                   GPIO_NOPULL
#define MX_SPI2_SCK_GPIO_Pin                    GPIO_PIN_13
#define MX_SPI2_SCK_GPIO_AF                     GPIO_AF5_SPI2
#define MX_SPI2_SCK_GPIO_Mode                   GPIO_MODE_AF_PP



/* Pin PB14 */
#define MX_SPI2_MISO_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI2_MISO_Pin                        PB14
#define MX_SPI2_MISO_GPIOx                      GPIOB
#define MX_SPI2_MISO_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI2_MISO_GPIO_Pin                   GPIO_PIN_14
#define MX_SPI2_MISO_GPIO_AF                    GPIO_AF5_SPI2
#define MX_SPI2_MISO_GPIO_Mode                  GPIO_MODE_AF_PP

/* Pin PB15 */
#define MX_SPI2_MOSI_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI2_MOSI_Pin                        PB15
#define MX_SPI2_MOSI_GPIOx                      GPIOB
#define MX_SPI2_MOSI_GPIO_PuPd                  GPIO_PULLUP
#define MX_SPI2_MOSI_GPIO_Pin                   GPIO_PIN_15
#define MX_SPI2_MOSI_GPIO_AF                    GPIO_AF5_SPI2
#define MX_SPI2_MOSI_GPIO_Mode                  GPIO_MODE_AF_PP

/*-------------------------------- SPI3       --------------------------------*/

#define MX_SPI3                                 1

/* GPIO Configuration */

/* Pin PC10 */
#define MX_SPI3_SCK_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI3_SCK_Pin                         PC10
#define MX_SPI3_SCK_GPIOx                       GPIOC
#define MX_SPI3_SCK_GPIO_PuPd                   GPIO_NOPULL
#define MX_CLK_A                                SPI3_SCK
#define MX_SPI3_SCK_GPIO_Pin                    GPIO_PIN_10
#define MX_SPI3_SCK_GPIO_AF                     GPIO_AF6_SPI3
#define MX_SPI3_SCK_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PC11 */
#define MX_SPI3_MISO_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI3_MISO_Pin                        PC11
#define MX_SPI3_MISO_GPIOx                      GPIOC
#define MX_SPI3_MISO_GPIO_PuPd                  GPIO_NOPULL
#define MX_MISO_A                               SPI3_MISO
#define MX_SPI3_MISO_GPIO_Pin                   GPIO_PIN_11
#define MX_SPI3_MISO_GPIO_AF                    GPIO_AF6_SPI3
#define MX_SPI3_MISO_GPIO_Mode                  GPIO_MODE_AF_PP

/* Pin PC12 */
#define MX_SPI3_MOSI_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI3_MOSI_Pin                        PC12
#define MX_SPI3_MOSI_GPIOx                      GPIOC
#define MX_SPI3_MOSI_GPIO_PuPd                  GPIO_NOPULL
#define MX_MOSI_A                               SPI3_MOSI
#define MX_SPI3_MOSI_GPIO_Pin                   GPIO_PIN_12
#define MX_SPI3_MOSI_GPIO_AF                    GPIO_AF6_SPI3
#define MX_SPI3_MOSI_GPIO_Mode                  GPIO_MODE_AF_PP

/*-------------------------------- SYS        --------------------------------*/

#define MX_SYS                                  1

/* GPIO Configuration */

/*-------------------------------- TIM6       --------------------------------*/

#define MX_TIM6                                 1

/* GPIO Configuration */

/*-------------------------------- TIM7       --------------------------------*/

#define MX_TIM7                                 1

/* GPIO Configuration */

/*-------------------------------- TIM10      --------------------------------*/

#define MX_TIM10                                1

/* GPIO Configuration */

/*-------------------------------- TIM13      --------------------------------*/

#define MX_TIM13                                1

/* GPIO Configuration */

/*-------------------------------- TIM14      --------------------------------*/

#define MX_TIM14                                1

/* GPIO Configuration */

/*-------------------------------- USART1     --------------------------------*/

#define MX_USART1                               1

#define MX_USART1_VM                            VM_ASYNC

/* GPIO Configuration */

/* Pin PA9 */
#define MX_USART1_TX_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_USART1_TX_Pin                        PA9
#define MX_USART1_TX_GPIOx                      GPIOA
#define MX_USART1_TX_GPIO_PuPd                  GPIO_PULLUP
#define MX_USART1_TX_GPIO_Pin                   GPIO_PIN_9
#define MX_USART1_TX_GPIO_AF                    GPIO_AF7_USART1
#define MX_USART1_TX_GPIO_Mode                  GPIO_MODE_AF_PP

/* Pin PA10 */
#define MX_USART1_RX_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_USART1_RX_Pin                        PA10
#define MX_USART1_RX_GPIOx                      GPIOA
#define MX_USART1_RX_GPIO_PuPd                  GPIO_PULLUP
#define MX_USART1_RX_GPIO_Pin                   GPIO_PIN_10
#define MX_USART1_RX_GPIO_AF                    GPIO_AF7_USART1
#define MX_USART1_RX_GPIO_Mode                  GPIO_MODE_AF_PP

/*-------------------------------- USB_OTG_FS --------------------------------*/

#define MX_USB_OTG_FS                           1

#define MX_USB_OTG_FS_DEVICE                    1

/* GPIO Configuration */

/* Pin PA11 */
#define MX_USB_OTG_FS_DM_GPIO_Speed             GPIO_SPEED_FREQ_VERY_HIGH
#define MX_USB_OTG_FS_DM_Pin                    PA11
#define MX_USB_OTG_FS_DM_GPIOx                  GPIOA
#define MX_USB_OTG_FS_DM_GPIO_PuPd              GPIO_NOPULL
#define MX_USB_OTG_FS_DM_GPIO_Pin               GPIO_PIN_11
#define MX_USB_OTG_FS_DM_GPIO_AF                GPIO_AF10_OTG_FS
#define MX_USB_OTG_FS_DM_GPIO_Mode              GPIO_MODE_AF_PP

/* Pin PA12 */
#define MX_USB_OTG_FS_DP_GPIO_Speed             GPIO_SPEED_FREQ_VERY_HIGH
#define MX_USB_OTG_FS_DP_Pin                    PA12
#define MX_USB_OTG_FS_DP_GPIOx                  GPIOA
#define MX_USB_OTG_FS_DP_GPIO_PuPd              GPIO_NOPULL
#define MX_USB_OTG_FS_DP_GPIO_Pin               GPIO_PIN_12
#define MX_USB_OTG_FS_DP_GPIO_AF                GPIO_AF10_OTG_FS
#define MX_USB_OTG_FS_DP_GPIO_Mode              GPIO_MODE_AF_PP

/*-------------------------------- NVIC       --------------------------------*/

#define MX_NVIC                                 1

/*-------------------------------- GPIO       --------------------------------*/

#define MX_GPIO                                 1

/* GPIO Configuration */

/* Pin PD5 */
#define MX_PD5_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PD5_Pin                              PD5
#define MX_PD5_GPIOx                            GPIOD
#define MX_PD5_PinState                         GPIO_PIN_RESET
#define MX_PD5_GPIO_PuPd                        GPIO_PULLUP
#define MX_IO1_A                                PD5
#define MX_PD5_GPIO_Pin                         GPIO_PIN_5
#define MX_PD5_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PD7 */
#define MX_PD7_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PD7_Pin                              PD7
#define MX_PD7_GPIOx                            GPIOD
#define MX_PD7_PinState                         GPIO_PIN_RESET
#define MX_PD7_GPIO_PuPd                        GPIO_PULLUP
#define MX_IO3_A                                PD7
#define MX_PD7_GPIO_Pin                         GPIO_PIN_7
#define MX_PD7_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PD6 */
#define MX_PD6_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PD6_Pin                              PD6
#define MX_PD6_GPIOx                            GPIOD
#define MX_PD6_PinState                         GPIO_PIN_RESET
#define MX_PD6_GPIO_PuPd                        GPIO_PULLUP
#define MX_IO2_A                                PD6
#define MX_PD6_GPIO_Pin                         GPIO_PIN_6
#define MX_PD6_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PD9 */
#define MX_PD9_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PD9_Pin                              PD9
#define MX_PD9_GPIOx                            GPIOD
#define MX_PD9_PinState                         GPIO_PIN_RESET
#define MX_PD9_GPIO_PuPd                        GPIO_PULLUP
#define MX_D7                                   PD9
#define MX_PD9_GPIO_Pin                         GPIO_PIN_9
#define MX_PD9_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PD8 */
#define MX_PD8_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PD8_Pin                              PD8
#define MX_PD8_GPIOx                            GPIOD
#define MX_PD8_PinState                         GPIO_PIN_RESET
#define MX_PD8_GPIO_PuPd                        GPIO_PULLUP
#define MX_D6                                   PD8
#define MX_PD8_GPIO_Pin                         GPIO_PIN_8
#define MX_PD8_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PC13-ANTI_TAMP */
#define MX_PC13_ANTI_TAMP_GPIO_Speed            GPIO_SPEED_FREQ_LOW
#define MX_PC13_ANTI_TAMP_Pin                   PC13_ANTI_TAMP
#define MX_PC13_ANTI_TAMP_GPIOx                 GPIOC
#define MX_PC13_ANTI_TAMP_PinState              GPIO_PIN_RESET
#define MX_PC13_ANTI_TAMP_GPIO_PuPd             GPIO_PULLUP
#define MX_BUZZER                               PC13_ANTI_TAMP
#define MX_PC13_ANTI_TAMP_GPIO_Pin              GPIO_PIN_13
#define MX_PC13_ANTI_TAMP_GPIO_ModeDefaultOutputPP GPIO_MODE_OUTPUT_PP

/* Pin PC14-OSC32_IN */
#define MX_PC14_OSC32_IN_Pin                    PC14_OSC32_IN
#define MX_PC14_OSC32_IN_GPIOx                  GPIOC
#define MX_PC14_OSC32_IN_GPIO_PuPd              GPIO_PULLDOWN
#define MX_KEY_MODE                             PC14_OSC32_IN
#define MX_PC14_OSC32_IN_GPIO_Pin               GPIO_PIN_14
#define MX_PC14_OSC32_IN_GPIO_ModeDefaultEXTI   GPIO_MODE_IT_RISING_FALLING

/* Pin PE2 */
#define MX_PE2_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PE2_Pin                              PE2
#define MX_PE2_GPIOx                            GPIOE
#define MX_PE2_PinState                         GPIO_PIN_RESET
#define MX_PE2_GPIO_PuPd                        GPIO_PULLUP
#define MX_OE                                   PE2
#define MX_PE2_GPIO_Pin                         GPIO_PIN_2
#define MX_PE2_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PE4 */
#define MX_PE4_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PE4_Pin                              PE4
#define MX_PE4_GPIOx                            GPIOE
#define MX_PE4_PinState                         GPIO_PIN_RESET
#define MX_PE4_GPIO_PuPd                        GPIO_PULLUP
#define MX_VS_ON                                PE4
#define MX_PE4_GPIO_Pin                         GPIO_PIN_4
#define MX_PE4_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PE3 */
#define MX_PE3_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PE3_Pin                              PE3
#define MX_PE3_GPIOx                            GPIOE
#define MX_PE3_PinState                         GPIO_PIN_RESET
#define MX_PE3_GPIO_PuPd                        GPIO_PULLUP
#define MX_RESET_A                              PE3
#define MX_PE3_GPIO_Pin                         GPIO_PIN_3
#define MX_PE3_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PE6 */
#define MX_PE6_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PE6_Pin                              PE6
#define MX_PE6_GPIOx                            GPIOE
#define MX_PE6_PinState                         GPIO_PIN_RESET
#define MX_PE6_GPIO_PuPd                        GPIO_PULLUP
#define MX_VPP_ON                               PE6
#define MX_PE6_GPIO_Pin                         GPIO_PIN_6
#define MX_PE6_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PE5 */
#define MX_PE5_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PE5_Pin                              PE5
#define MX_PE5_GPIOx                            GPIOE
#define MX_PE5_PinState                         GPIO_PIN_RESET
#define MX_PE5_GPIO_PuPd                        GPIO_PULLUP
#define MX_VDD_ON                               PE5
#define MX_PE5_GPIO_Pin                         GPIO_PIN_5
#define MX_PE5_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PE8 */
#define MX_PE8_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PE8_Pin                              PE8
#define MX_PE8_GPIOx                            GPIOE
#define MX_PE8_PinState                         GPIO_PIN_RESET
#define MX_PE8_GPIO_PuPd                        GPIO_PULLUP
#define MX_VS_ON_B                              PE8
#define MX_PE8_GPIO_Pin                         GPIO_PIN_8
#define MX_PE8_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PC6 */
#define MX_PC6_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PC6_Pin                              PC6
#define MX_PC6_GPIOx                            GPIOC
#define MX_PC6_PinState                         GPIO_PIN_RESET
#define MX_PC6_GPIO_PuPd                        GPIO_PULLDOWN
#define MX_BUSY                                 PC6
#define MX_PC6_GPIO_Pin                         GPIO_PIN_6
#define MX_PC6_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PE7 */
#define MX_PE7_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PE7_Pin                              PE7
#define MX_PE7_GPIOx                            GPIOE
#define MX_PE7_PinState                         GPIO_PIN_SET
#define MX_PE7_GPIO_PuPd                        GPIO_PULLUP
#define MX_GND_CTRL                             PE7
#define MX_PE7_GPIO_Pin                         GPIO_PIN_7
#define MX_PE7_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PC8 */
#define MX_PC8_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PC8_Pin                              PC8
#define MX_PC8_GPIOx                            GPIOC
#define MX_PC8_PinState                         GPIO_PIN_RESET
#define MX_PC8_GPIO_PuPd                        GPIO_PULLDOWN
#define MX_OK                                   PC8
#define MX_PC8_GPIO_Pin                         GPIO_PIN_8
#define MX_PC8_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PC7 */
#define MX_PC7_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PC7_Pin                              PC7
#define MX_PC7_GPIOx                            GPIOC
#define MX_PC7_PinState                         GPIO_PIN_RESET
#define MX_PC7_GPIO_PuPd                        GPIO_PULLDOWN
#define MX_NG                                   PC7
#define MX_PC7_GPIO_Pin                         GPIO_PIN_7
#define MX_PC7_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PA8 */
#define MX_PA8_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PA8_Pin                              PA8
#define MX_PA8_GPIOx                            GPIOA
#define MX_PA8_PinState                         GPIO_PIN_RESET
#define MX_PA8_GPIO_PuPd                        GPIO_PULLUP
#define MX_D4                                   PA8
#define MX_PA8_GPIO_Pin                         GPIO_PIN_8
#define MX_PA8_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PC9 */
#define MX_PC9_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PC9_Pin                              PC9
#define MX_PC9_GPIOx                            GPIOC
#define MX_PC9_PinState                         GPIO_PIN_RESET
#define MX_PC9_GPIO_PuPd                        GPIO_PULLUP
#define MX_D5                                   PC9
#define MX_PC9_GPIO_Pin                         GPIO_PIN_9
#define MX_PC9_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PD12 */
#define MX_PD12_GPIO_Speed                      GPIO_SPEED_FREQ_LOW
#define MX_PD12_Pin                             PD12
#define MX_PD12_GPIOx                           GPIOD
#define MX_PD12_PinState                        GPIO_PIN_RESET
#define MX_PD12_GPIO_PuPd                       GPIO_PULLUP
#define MX_LCD_DCX                              PD12
#define MX_PD12_GPIO_Pin                        GPIO_PIN_12
#define MX_PD12_GPIO_ModeDefaultOutputPP        GPIO_MODE_OUTPUT_PP

/* Pin PA15 */
#define MX_PA15_GPIO_Speed                      GPIO_SPEED_FREQ_LOW
#define MX_PA15_Pin                             PA15
#define MX_PA15_GPIOx                           GPIOA
#define MX_PA15_PinState                        GPIO_PIN_SET
#define MX_PA15_GPIO_PuPd                       GPIO_PULLUP
#define MX_USB_EN                               PA15
#define MX_PA15_GPIO_Pin                        GPIO_PIN_15
#define MX_PA15_GPIO_ModeDefaultOutputPP        GPIO_MODE_OUTPUT_PP

/* Pin PD11 */
#define MX_PD11_GPIO_Speed                      GPIO_SPEED_FREQ_LOW
#define MX_PD11_Pin                             PD11
#define MX_PD11_GPIOx                           GPIOD
#define MX_PD11_PinState                        GPIO_PIN_SET
#define MX_PD11_GPIO_PuPd                       GPIO_PULLUP
#define MX_LCD_CS                               PD11
#define MX_PD11_GPIO_Pin                        GPIO_PIN_11
#define MX_PD11_GPIO_ModeDefaultOutputPP        GPIO_MODE_OUTPUT_PP

/* Pin PD14 */
#define MX_PD14_GPIO_Speed                      GPIO_SPEED_FREQ_LOW
#define MX_PD14_Pin                             PD14
#define MX_PD14_GPIOx                           GPIOD
#define MX_PD14_PinState                        GPIO_PIN_RESET
#define MX_PD14_GPIO_PuPd                       GPIO_PULLUP
#define MX_BACK_LIGHT                           PD14
#define MX_PD14_GPIO_Pin                        GPIO_PIN_14
#define MX_PD14_GPIO_ModeDefaultOutputPP        GPIO_MODE_OUTPUT_PP

/* Pin PD13 */
#define MX_PD13_GPIO_Speed                      GPIO_SPEED_FREQ_LOW
#define MX_PD13_Pin                             PD13
#define MX_PD13_GPIOx                           GPIOD
#define MX_PD13_PinState                        GPIO_PIN_SET
#define MX_PD13_GPIO_PuPd                       GPIO_PULLUP
#define MX_LCD_RESET                            PD13
#define MX_PD13_GPIO_Pin                        GPIO_PIN_13
#define MX_PD13_GPIO_ModeDefaultOutputPP        GPIO_MODE_OUTPUT_PP

/* Pin PD1 */
#define MX_PD1_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PD1_Pin                              PD1
#define MX_PD1_GPIOx                            GPIOD
#define MX_PD1_PinState                         GPIO_PIN_RESET
#define MX_PD1_GPIO_PuPd                        GPIO_PULLUP
#define MX_D1                                   PD1
#define MX_PD1_GPIO_Pin                         GPIO_PIN_1
#define MX_PD1_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PD15 */
#define MX_PD15_Pin                             PD15
#define MX_PD15_GPIOx                           GPIOD
#define MX_PD15_GPIO_PuPd                       GPIO_PULLUP
#define MX_START                                PD15
#define MX_PD15_GPIO_Pin                        GPIO_PIN_15
#define MX_PD15_GPIO_ModeDefaultEXTI            GPIO_MODE_IT_RISING_FALLING

/* Pin PD0 */
#define MX_PD0_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PD0_Pin                              PD0
#define MX_PD0_GPIOx                            GPIOD
#define MX_PD0_PinState                         GPIO_PIN_RESET
#define MX_PD0_GPIO_PuPd                        GPIO_PULLUP
#define MX_D0                                   PD0
#define MX_PD0_GPIO_Pin                         GPIO_PIN_0
#define MX_PD0_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PD3 */
#define MX_PD3_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PD3_Pin                              PD3
#define MX_PD3_GPIOx                            GPIOD
#define MX_PD3_PinState                         GPIO_PIN_RESET
#define MX_PD3_GPIO_PuPd                        GPIO_PULLUP
#define MX_D3                                   PD3
#define MX_PD3_GPIO_Pin                         GPIO_PIN_3
#define MX_PD3_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

/* Pin PD2 */
#define MX_PD2_GPIO_Speed                       GPIO_SPEED_FREQ_LOW
#define MX_PD2_Pin                              PD2
#define MX_PD2_GPIOx                            GPIOD
#define MX_PD2_PinState                         GPIO_PIN_RESET
#define MX_PD2_GPIO_PuPd                        GPIO_PULLUP
#define MX_D2                                   PD2
#define MX_PD2_GPIO_Pin                         GPIO_PIN_2
#define MX_PD2_GPIO_ModeDefaultOutputPP         GPIO_MODE_OUTPUT_PP

#endif  /* __MX_DEVICE_H */

