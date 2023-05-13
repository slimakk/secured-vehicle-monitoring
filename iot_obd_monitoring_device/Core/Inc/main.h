/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OBD.h"
#include "bg77.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define HSE_IN_Pin GPIO_PIN_0
#define HSE_IN_GPIO_Port GPIOH
#define HSE_OUT_Pin GPIO_PIN_1
#define HSE_OUT_GPIO_Port GPIOH
#define AP_READY_Pin GPIO_PIN_2
#define AP_READY_GPIO_Port GPIOC
#define PON_TRIG_Pin GPIO_PIN_3
#define PON_TRIG_GPIO_Port GPIOC
#define IoT_CTS_Pin GPIO_PIN_0
#define IoT_CTS_GPIO_Port GPIOA
#define IoT_RTS_Pin GPIO_PIN_1
#define IoT_RTS_GPIO_Port GPIOA
#define IoT_TX_Pin GPIO_PIN_2
#define IoT_TX_GPIO_Port GPIOA
#define IoT_RX_Pin GPIO_PIN_3
#define IoT_RX_GPIO_Port GPIOA
#define IoT_RI_Pin GPIO_PIN_4
#define IoT_RI_GPIO_Port GPIOA
#define IoT_PWR_Pin GPIO_PIN_5
#define IoT_PWR_GPIO_Port GPIOA
#define IoT_DTR_Pin GPIO_PIN_6
#define IoT_DTR_GPIO_Port GPIOA
#define BAT_V_Pin GPIO_PIN_5
#define BAT_V_GPIO_Port GPIOC
#define L_Line_Pin GPIO_PIN_12
#define L_Line_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define GNSS_TX_Pin GPIO_PIN_10
#define GNSS_TX_GPIO_Port GPIOC
#define GNSS_RX_Pin GPIO_PIN_11
#define GNSS_RX_GPIO_Port GPIOC
#define CAN_S_Pin GPIO_PIN_2
#define CAN_S_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define K_Line_TX_Pin GPIO_PIN_6
#define K_Line_TX_GPIO_Port GPIOB
#define K_Line_RX_Pin GPIO_PIN_7
#define K_Line_RX_GPIO_Port GPIOB
#define BOOT0_Pin GPIO_PIN_3
#define BOOT0_GPIO_Port GPIOH

/* USER CODE BEGIN Private defines */
#define TRUE 1
#define FALSE 0
#define MQTT_IP "1"
#define MQTT_PORT 1883
#define OBD_TOPIC "topic/obd"

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
