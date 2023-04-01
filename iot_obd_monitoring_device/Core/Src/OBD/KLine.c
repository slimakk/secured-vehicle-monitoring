/*
 * KLine.c
 *
 *  Created on: Nov 4, 2022
 *      Author: miros
 */
#include "KLine.h"
#include "OBD_PID.h"

extern uint8_t uartBuf[10];
extern OBD obd_comm;
extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim6;
uint8_t kline_rx_buf[16];

uint8_t rx_frame[7];

static uint8_t ecu_addr;
static uint8_t pid_length;

static void MX_GPIO_KLineUART_Init(void);
static void UART_PIN_State(uint8_t state);
static uint8_t Verify_Checksum (uint8_t *data, uint8_t lenght);

obd_protocol KLine_Init(void){
	MX_GPIO_KLineUART_Init();
//	5 Baud address 0x33
	HAL_Delay(3000);
	UART_PIN_State(0); //0
	HAL_Delay(200);
	UART_PIN_State(1);//11
	HAL_Delay(400);
	UART_PIN_State(0);//00
	HAL_Delay(400);
	UART_PIN_State(1);//11
	HAL_Delay(400);
	UART_PIN_State(0);//00
	HAL_Delay(400);
	UART_PIN_State(1);//1

	MX_USART1_UART_Init();

	HAL_UART_Receive(KLINE, uartBuf, 3, 500);

	if(uartBuf[0] != 0x55)
	{
		return OBD_NONE;
	}
	else if(uartBuf[1] == uartBuf[2])
	{
		if(uartBuf[1] == 0x08 || 0x94)
		{
			uint8_t inv_kb = ~uartBuf[1];
			HAL_Delay(25);
			HAL_UART_Transmit(KLINE, &inv_kb, 1, 50);
			HAL_Delay(25);
			__HAL_UART_SEND_REQ(KLINE, UART_RXDATA_FLUSH_REQUEST);
			HAL_UART_Receive(KLINE, &ecu_addr, 1, 100);
//			__HAL_UART_SEND_REQ(KLINE, UART_RXDATA_FLUSH_REQUEST);
			return OBD_PROTO_ISO9141;
		}
	}
	else
	{
		uint8_t inv_kb = ~uartBuf[1];
		HAL_Delay(25);
		HAL_UART_Transmit(KLINE, &inv_kb, 1, 50);
		__HAL_UART_SEND_REQ(KLINE, UART_RXDATA_FLUSH_REQUEST);
		HAL_UART_Receive(KLINE, &ecu_addr, 1, 100);
		return OBD_PROTO_KWP2000_SLOW;
	}
}

obd_protocol KWP2000_Fast_Init(void)
{
	uint8_t start_msg[5]={0xC1, 0x33, 0xF1, 0x81, 0x66};
//	uint8_t resp_msg[7]={0};
	uint8_t checksum = 0;
	obd_comm.msg_type = 1;

	HAL_UART_DeInit(KLINE);
	HAL_Delay(3000);
	MX_GPIO_KLineUART_Init();
	UART_PIN_State(0);
	HAL_Delay(25);
	UART_PIN_State(1);
	HAL_Delay(25);
	if (HAL_UART_Init(KLINE) != HAL_OK)
	{
		Error_Handler();
	}
//	MX_USART1_UART_Init(10400);
	HAL_UART_Transmit(KLINE, start_msg, 5, 10);
//	__HAL_UART_SEND_REQ(KLINE, UART_RXDATA_FLUSH_REQUEST);
//	HAL_Delay(20);

	HAL_UART_Receive_DMA(KLINE, uartBuf, 8);
	HAL_TIM_Base_Start_IT(&htim6);

	while((obd_comm.msg_type != 0) && (obd_comm.msg_type != 3))
	{
		__NOP();
	}
	if(obd_comm.msg_type == 0)
	{
		for(int i = 1; i < 7; i++)
		{
			checksum = checksum + uartBuf[i];
		}
		checksum = checksum % 256;
		if(checksum == uartBuf[7] && checksum != 0)
		{
			ecu_addr = uartBuf[3];
			return OBD_PROTO_KWP2000_FAST;
		}
		else
			return OBD_NONE;
	}
	else
		return OBD_NONE;
}

static void MX_GPIO_KLineUART_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = K_Line_TX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(K_Line_TX_GPIO_Port, &GPIO_InitStruct);
}

static void UART_PIN_State(uint8_t state)
{
	/*KLine has inverted logic, HIGH = 0, LOW = 1*/
	if(state == 1)
	{
		HAL_GPIO_WritePin(K_Line_TX_GPIO_Port, K_Line_TX_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(L_Line_GPIO_Port, L_Line_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(K_Line_TX_GPIO_Port, K_Line_TX_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(L_Line_GPIO_Port, L_Line_Pin, GPIO_PIN_SET);
	}
}

void KLine_SEND_MESSAGE(uint8_t *tx_frame)
{
	uint8_t kline_msg[6] = {0x68, 0x6A, 0xF1, tx_frame[0], tx_frame[1], 0};

	pid_length = PID_Get_Lenght(tx_frame[1]);
	uint8_t checksum = 0;

	for(int i = 0; i < sizeof(kline_msg) - 1; i++)
	{
		checksum += kline_msg[i];
	}

	kline_msg[sizeof(kline_msg)-1] = checksum;

	HAL_UART_Transmit(KLINE, kline_msg, sizeof(kline_msg), 12);

	__HAL_UART_SEND_REQ(KLINE, UART_RXDATA_FLUSH_REQUEST);

	HAL_UART_Receive_DMA(KLINE, kline_rx_buf, pid_length + 4);

	HAL_Delay(60);
}

void KWP2000_SEND_MESSAGE(uint8_t *tx_frame)
{
	uint8_t kwp_msg[] = {0xC2, 0x33, 0xF1, tx_frame[0], tx_frame[1], 0};

	pid_length = PID_Get_Lenght(tx_frame[1]);
	uint8_t checksum = 0;
	obd_comm.msg_type = 2;

	for(int i = 0; i < sizeof(kwp_msg) - 1; i++)
	{
		checksum += kwp_msg[i];
	}

	kwp_msg[sizeof(kwp_msg)-1] = checksum;

	HAL_Delay(20);

	HAL_UART_Transmit(KLINE, kwp_msg, sizeof(kwp_msg), 12);

	__HAL_UART_SEND_REQ(KLINE, UART_RXDATA_FLUSH_REQUEST);

	HAL_UART_Receive_DMA(KLINE, kline_rx_buf, pid_length + 5);

	HAL_Delay(60);
}

static uint8_t Verify_Checksum (uint8_t *data, uint8_t lenght)
{
	uint8_t checksum = 0;
	for(int i = 0; i < lenght - 1; i++)
	{
		checksum += data[i];
	}
	checksum = checksum % 256;
	if(data[lenght - 1] == checksum)
	{
		return 1;
	}
	return 0;
}

void kline_rx_callback(void)
{
	if(obd_comm.msg_type == 1)
	{
		HAL_TIM_Base_Stop_IT(&htim6);
		obd_comm.msg_type = 0;
	}
	else if (obd_comm.msg_type == 2)
	{
		if(Verify_Checksum(kline_rx_buf, pid_length + 5))
		{
			uint8_t j = 0;
			for(uint8_t i = 2; i <= pid_length + 4; i++)
			{
				rx_frame[j] = kline_rx_buf[i];
				j++;
			}
			obd_comm.msg_type = 0;
			obd_comm.current_value = OBD2_PID_Parse(rx_frame);
			OBD2_ShowOnDisplay(obd_comm.current_value);
			HAL_IWDG_Refresh(&hiwdg);
		}
	}
}
