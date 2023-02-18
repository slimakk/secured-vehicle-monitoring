/*
 * CAN.c
 *
 *  Created on: Nov 5, 2022
 *      Author: miros
 */
#include "CAN.h"
#include "OBD.h"

extern OBD obd_comm;
extern IWDG_HandleTypeDef hiwdg;
extern CAN_HandleTypeDef hcan1;
uint32_t tx_mailbox;

void MX_CAN1_Init(void)
{
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 16;
	hcan1.Init.Mode = CAN_MODE_NORMAL;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = DISABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.AutoRetransmission = DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan1) != HAL_OK)
	{
	  Error_Handler();
	}
}

void canConfig(void)
{
	CAN_FilterTypeDef can_filter;
	can_filter.FilterBank = 0;
	can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
	can_filter.FilterIdHigh = 0x7E8 << 5;
	can_filter.FilterIdLow = 0x0000;
	can_filter.FilterMaskIdHigh = 0x7F8 << 5;
	can_filter.FilterMaskIdLow = 0x0000;
	can_filter.FilterFIFOAssignment = CAN_RX_FIFO0;
	can_filter.FilterActivation = ENABLE;

	if(HAL_CAN_ConfigFilter(&hcan1, &can_filter) != HAL_OK)
	{
		Error_Handler();
	}

	if(HAL_CAN_Start(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef rx_header;
	uint8_t rx_data[RX_DATA_LENGTH];

	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header, rx_data);

	obd_comm.current_value = OBD2_PID_Parse(rx_data);

	OBD2_ShowOnDisplay(obd_comm.current_value);

//	HAL_IWDG_Refresh(&hiwdg);
}

void CAN_SEND_MESSAGE(uint8_t *tx_frame)
{
	CAN_TxHeaderTypeDef tx_header;
	tx_header.StdId = DEVICE_CAN_ID;
	tx_header.DLC = TX_DATA_LENGTH;
	tx_header.IDE = CAN_ID_STD;
	tx_header.RTR = CAN_RTR_DATA;

	if(HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_frame, &tx_mailbox) != HAL_OK)
	{
		Error_Handler();
	}

}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	__HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FOV0);
//	__HAL_CAN_Receive_IT(hcan, CAN_RX_FIFO0);
}
