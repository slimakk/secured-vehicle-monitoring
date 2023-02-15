/*
 * button_handler.c
 *
 *  Created on: Feb 8, 2023
 *      Author: Miro
 */
#include "button_handler.h"

extern TIM_HandleTypeDef htim1;
extern OBD obd_comm;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == MENU_BTN_Pin)
	{
		HAL_TIM_Base_Start_IT(&htim1);
		obd_comm.button_state = 0;
	}
	else
	{
		__NOP();
	}
}



