/*
 * it_callbacks.c
 *
 *  Created on: Feb 15, 2023
 *      Author: Miro
 */
#include "it_callbacks.h"
#include "OBD_PID.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim6;
extern OBD obd_comm;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		if(obd_comm.msg_type == 0)
		{
			__NOP();
		}
		else
			obd_comm.msg_type = 3;
		HAL_TIM_Base_Stop_IT(&htim6);
	}
}

