/*
 * button_handler.c
 *
 *  Created on: Feb 8, 2023
 *      Author: Miro
 */
#include "button_handler.h"
#include "OBD_PID.h"

extern TIM_HandleTypeDef htim1;
extern OBD obd_comm;

static uint8_t state = 1;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == MENU_BTN_Pin)
	{
		HAL_TIM_Base_Start_IT(&htim1);
		state = 0;
//		obd_comm.pid_index++;
//		obd_comm.pid = Get_PID(obd_comm.pid_index);
	}
	else
	{
		__NOP();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(HAL_GPIO_ReadPin(MENU_BTN_GPIO_Port, MENU_BTN_Pin)== GPIO_PIN_SET)
	{
		state = 1;
		obd_comm.pid_index++;
		obd_comm.pid = Get_PID(obd_comm.pid_index);
		HAL_TIM_Base_Stop_IT(&htim1);
	}
}

