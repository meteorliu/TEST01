/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PGA_A2_Pin GPIO_PIN_2
#define PGA_A2_GPIO_Port GPIOE
#define PGA_A1_Pin GPIO_PIN_3
#define PGA_A1_GPIO_Port GPIOE
#define PGA_A0_Pin GPIO_PIN_4
#define PGA_A0_GPIO_Port GPIOE
#define IPGA_CS_Pin GPIO_PIN_5
#define IPGA_CS_GPIO_Port GPIOE
#define CAP_SW3_Pin GPIO_PIN_6
#define CAP_SW3_GPIO_Port GPIOE
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOC
#define PGA_SD_Pin GPIO_PIN_14
#define PGA_SD_GPIO_Port GPIOC
#define VPGA_CS_Pin GPIO_PIN_15
#define VPGA_CS_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOC
#define CAP_SW2_Pin GPIO_PIN_2
#define CAP_SW2_GPIO_Port GPIOC
#define CAP_SW1_Pin GPIO_PIN_3
#define CAP_SW1_GPIO_Port GPIOC
#define DAC_WAVE_Pin GPIO_PIN_4
#define DAC_WAVE_GPIO_Port GPIOA
#define DAC_CURREF_Pin GPIO_PIN_5
#define DAC_CURREF_GPIO_Port GPIOA
#define ADC2_IN7_LCUR2_Pin GPIO_PIN_7
#define ADC2_IN7_LCUR2_GPIO_Port GPIOA
#define ADC2_IN14_LCUR1_Pin GPIO_PIN_4
#define ADC2_IN14_LCUR1_GPIO_Port GPIOC
#define ADC2_IN15_HCUR_Pin GPIO_PIN_5
#define ADC2_IN15_HCUR_GPIO_Port GPIOC
#define ADC2_IN8_INJPV_Pin GPIO_PIN_0
#define ADC2_IN8_INJPV_GPIO_Port GPIOB
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define SW_INJ_Pin GPIO_PIN_9
#define SW_INJ_GPIO_Port GPIOD
#define INJ_EN_Pin GPIO_PIN_10
#define INJ_EN_GPIO_Port GPIOD
#define INJ_DPOW_SW_Pin GPIO_PIN_11
#define INJ_DPOW_SW_GPIO_Port GPIOD
#define TIM8_CH1_SET_Pin GPIO_PIN_6
#define TIM8_CH1_SET_GPIO_Port GPIOC
#define TIM8_CH2_HOLD_Pin GPIO_PIN_7
#define TIM8_CH2_HOLD_GPIO_Port GPIOC
#define OVER_CUR_Pin GPIO_PIN_8
#define OVER_CUR_GPIO_Port GPIOC
#define INJ_LSIDE_Pin GPIO_PIN_9
#define INJ_LSIDE_GPIO_Port GPIOC
#define TIM1_CH1_VSETPWM_Pin GPIO_PIN_8
#define TIM1_CH1_VSETPWM_GPIO_Port GPIOA
#define RS485_TX_Pin GPIO_PIN_9
#define RS485_TX_GPIO_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_10
#define RS485_RX_GPIO_Port GPIOA
#define RS485_EN_Pin GPIO_PIN_11
#define RS485_EN_GPIO_Port GPIOA
#define SW_LOAD2_Pin GPIO_PIN_6
#define SW_LOAD2_GPIO_Port GPIOB
#define SW_LOAD3_Pin GPIO_PIN_7
#define SW_LOAD3_GPIO_Port GPIOB
#define SW_LOAD1_Pin GPIO_PIN_8
#define SW_LOAD1_GPIO_Port GPIOB
#define RELAY3_IR_Pin GPIO_PIN_9
#define RELAY3_IR_GPIO_Port GPIOB
#define RELAY1_LCR_IR_Pin GPIO_PIN_0
#define RELAY1_LCR_IR_GPIO_Port GPIOE
#define RELAY2_LCR_Pin GPIO_PIN_1
#define RELAY2_LCR_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
#define RS485_DIR_Pin GPIO_PIN_11
#define RS485_DIR_GPIO_Port GPIOA
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
