/*
 * CAN.c
 *
 *  Created on: Nov 5, 2022
 *      Author: miros
 */
#include "CAN.h"
#include "main.h"
#include "stm32l4xx_hal.h"

uint32_t txMailbox;


void CAN_RESPONSE(uint8_t serviceID, uint8_t lenght, uint8_t PID, CAN_HandleTypeDef *hcan)
{
	CAN_TxHeaderTypeDef responseHeader;
	uint8_t responseFrame[8] = {lenght, serviceID, PID, 35, 0x00, 0x00, 0x00, 0x00};
	responseHeader.DLC = 8;
	responseHeader.StdId = 0x7E8;
	responseHeader.IDE = CAN_ID_STD;
	responseHeader.RTR = CAN_RTR_REMOTE;

	HAL_CAN_AddTxMessage(hcan, &responseHeader, responseFrame, &txMailbox);
}

uint8_t* CAN_REQUEST_HANDLE(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef responseHeader;
	static uint8_t responseFrame[8] = {0};

	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &responseHeader, responseFrame);

	return responseFrame;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

	uint8_t* engineTemp = CAN_REQUEST_HANDLE(hcan);

	CAN_RESPONSE(engineTemp[1], 3, engineTemp[2], hcan);
}
