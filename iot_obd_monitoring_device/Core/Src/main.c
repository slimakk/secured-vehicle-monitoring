/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t uartBuf[10] = {0};

uint16_t adc_buffer[64] = {0};

OBD obd_comm;

BG77 module;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
static void acquire_vehicle_data(float buffer[][2]);
static uint8_t mqtt_start(BG77 module);
static uint8_t mqtt_stop(BG77 module);
static void createJson(char buff[1000], float array[][2], float array_cpy[][2], int num_of_values);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void acquire_vehicle_data(float buffer[][2])
{
	uint8_t pids [3] = {0x5, 0xc, 0xd};
	for(uint8_t i = 0; i < sizeof(pids); i++)
	{
		obd_comm.pid = pids[i];
		obd2_request(obd_comm);
		if(obd2_request(obd_comm))
		{
			buffer[i][0] = obd_comm.pid;
			buffer[i][1] = obd_comm.current_value;
		}

	}
}

static void createJson(char buff[1000], float array[][2], float array_cpy[][2], int num_of_values)
{
    char json_string [10] = "{\n";
    char temp[100];
    sprintf(buff, json_string);
    for (uint8_t i = 0; i <= num_of_values - 1; i++)
    {
        if(i == num_of_values - 1)
        {
        	sprintf(temp, "\"0x%x\":%.2f}\n", (uint8_t)array[i][0], array[i][1]);
        }
        else
        {
        	sprintf(temp, "\"0x%x\":%.2f,\n", (uint8_t)array[i][0], array[i][1]);
        }
        strcat(buff, temp);
        array_cpy[i][0] = array[i][0];
        array_cpy[i][1] = array[i][1];
    }
}

static uint8_t mqtt_start(BG77 module)
{
	module.mqtt_status = mqtt_open(MQTT_IP,  MQTT_PORT, 0);
	if(module.mqtt_status != 0)
	{
		return (FALSE);
	}
	HAL_Delay(10000);
	if(mqtt_connect(0,"obd5", &module))
	{
		return (TRUE);
	}
	return (TRUE);
}

static uint8_t mqtt_stop(BG77 module)
{
	if(mqtt_disconnect(0))
	{
		if(mqtt_close(0, module))
		{
			return (FALSE);
		}
		else
		{
			return (TRUE);
		}
	}
	else
	{
		return (TRUE);
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	float obd_buf[99][2];
	float obd_buf_cpy[99][2];
	char buffer[1000];
	uint32_t timer = 0;
//	uint32_t timer_t = 0;
	uint16_t counter = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  MX_TIM7_Init();
  MX_TIM16_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
//  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
//  HAL_ADC_Start_DMA(&hadc1, adc_buffer, 64);
//
//  obd_comm.voltage = adc_avg(adc_buffer);

  module.initialized = module_init(&module);
  module.rssi = check_signal();
  obd_comm.used_protocol = obd2_init();

  acquire_vehicle_data(obd_buf);
  module.connected = mqtt_start(module);

  createJson(buffer, obd_buf, obd_buf_cpy,3);

  mqtt_publish(0,0,0,0,OBD_TOPIC, buffer);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if((HAL_GetTick() - timer) >= 5000)
	  {
		  acquire_vehicle_data(obd_buf);
		  createJson(buffer, obd_buf, obd_buf_cpy,3);
		  mqtt_publish(0,0,0,0,OBD_TOPIC, buffer);
		  counter++;
		  timer = HAL_GetTick();
	  }
	  if(counter >= 1000)
	  {
		  module.connected = mqtt_stop(module);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
