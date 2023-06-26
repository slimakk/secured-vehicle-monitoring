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
//extern IWDG_HandleTypeDef hiwdg;
uint8_t kline_rx_buf[16];

uint8_t rx_frame[7];

static uint8_t ecu_addr;
static uint8_t pid_length;

static void MX_GPIO_KLineUART_Init(void);
static void uart_pin_state(uint8_t state);
static uint8_t verify_checksum (uint8_t *data, uint8_t lenght);
/*
 * @brief	ISO 9141 and ISO 14230 slow initialization procedure
 * @param	None
 * @retval	OBD_PROTO_ISO9141 or OBD_PROTO_KWP2000_SLOW if the initialization is successful, OBD_NONE if there is no response
 * */
obd_protocol kline_init(void){
	MX_GPIO_KLineUART_Init();
//	5 Baud address 0x33
	HAL_Delay(3000);
	uart_pin_state(0); //0
	HAL_Delay(200);
	uart_pin_state(1);//11
	HAL_Delay(400);
	uart_pin_state(0);//00
	HAL_Delay(400);
	uart_pin_state(1);//11
	HAL_Delay(400);
	uart_pin_state(0);//00
	HAL_Delay(400);
	uart_pin_state(1);//1

	MX_USART1_UART_Init();

	HAL_UART_Receive(KLINE, uartBuf, 3, 500);

	if(uartBuf[0] != 0x55)
	{
		return (OBD_NONE);
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
			return (OBD_PROTO_ISO9141);
		}
	}
	else
	{
		uint8_t inv_kb = ~uartBuf[1];
		HAL_Delay(25);
		HAL_UART_Transmit(KLINE, &inv_kb, 1, 50);
		__HAL_UART_SEND_REQ(KLINE, UART_RXDATA_FLUSH_REQUEST);
		HAL_UART_Receive(KLINE, &ecu_addr, 1, 100);
		return (OBD_PROTO_KWP2000_SLOW);
	}
	return (OBD_NONE);
}
/*
 * @brief	ISO 14230 fast initialization procedure
 * @param	None
 * @retval	OBD_PROTO_KWP2000_FAST if the initialization is successful, OBD_NONE if there is no response
 * */
obd_protocol kwp2000_fast_init(void)
{
	uint8_t start_msg[5]={0xC1, 0x33, 0xF1, 0x81, 0x66};
//	uint8_t resp_msg[7]={0};
	uint8_t checksum = 0;
	obd_comm.msg_type = 1;

	HAL_UART_DeInit(KLINE);
	HAL_Delay(3000);
	MX_GPIO_KLineUART_Init();
	uart_pin_state(0);
	HAL_Delay(25);
	uart_pin_state(1);
	HAL_Delay(25);
	if (HAL_UART_Init(KLINE) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_UART_Transmit(KLINE, start_msg, 5, 10);
//	__HAL_UART_SEND_REQ(KLINE, UART_RXDATA_FLUSH_REQUEST);

	HAL_UART_Receive_DMA(KLINE, uartBuf, 8);
	HAL_TIM_Base_Start_IT(KLINE_TIMER);

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
			return (OBD_PROTO_KWP2000_FAST);
		}
		else
			return (OBD_NONE);
	}
	else
		return (OBD_NONE);
}
/*
 * @brief	Initialization function for GPIO pins used by KLine UART interface
 * @param	None
 * @retval	None
 * */
static void MX_GPIO_KLineUART_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = K_Line_TX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(K_Line_TX_GPIO_Port, &GPIO_InitStruct);
}
/*
 * @brief	Toggles K and L lines for slow address transmission -- inverted logic
 * @param	state	Requested state 1 or 0
 * @retval	None
 * */
static void uart_pin_state(uint8_t state)
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
/*
 * @brief	Sends OBD II message via ISO 9141 protocol and checks the response.
 * @param	*tx_frame	Pointer to the requested TX frame
 * @retval	TRUE if valid message has been received, else FALSE
 * */
uint8_t kline_send_msg(uint8_t *tx_frame)
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

	HAL_TIM_Base_Start_IT(MSG_TIMER);

	while((obd_comm.msg_type != 0) && (obd_comm.timeout != 1))
	{
		__NOP();
	}
	if(obd_comm.timeout == 1)
	{
		return (FALSE);
	}
	if(verify_checksum(kline_rx_buf, pid_length + 5))
	{
		uint8_t j = 0;
		for(uint8_t i = 2; i <= pid_length + 4; i++)
		{
			rx_frame[j] = kline_rx_buf[i];
			j++;
		}
		obd_comm.current_value = obd2_pid_parse(rx_frame);
		return (TRUE);
	}
		return (FALSE);
}
/*
 * @brief	Sends OBD II message via ISO 14230 protocol and checks the response.
 * @param	*tx_frame	Pointer to the requested TX frame
 * @retval	TRUE if valid message has been received, else FALSE
 * */
uint8_t kwp2000_send_msg(uint8_t *tx_frame)
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

	HAL_TIM_Base_Start_IT(MSG_TIMER);

	while((obd_comm.msg_type != 0) && (obd_comm.timeout != 1))
	{
		__NOP();
	}
	if(obd_comm.timeout == 1)
	{
		return (FALSE);
	}
	if(verify_checksum(kline_rx_buf, pid_length + 5))
	{
		uint8_t j = 0;
		for(uint8_t i = 2; i <= pid_length + 4; i++)
		{
			rx_frame[j] = kline_rx_buf[i];
			j++;
		}
		obd_comm.current_value = obd2_pid_parse(rx_frame);
		return (TRUE);
	}
	return (FALSE);
}
/*
 * @brief	Calculates checksum of received message and checks it against received checksum
 * @param	*data	Received message
 * @param	length	Expected lenght of the message based on its PID
 * @retval	TRUE if the message has valid checksum
 * */
static uint8_t verify_checksum (uint8_t *data, uint8_t length)
{
	uint8_t checksum = 0;
	for(int i = 0; i < length - 1; i++)
	{
		checksum += data[i];
	}
	checksum = checksum % 256;
	if(data[length - 1] == checksum)
	{
		return (TRUE);
	}
	return (FALSE);
}
/*
 * @brief	K line UART receive callback function
 * @param	None
 * @retval	None
 * */
void kline_rx_callback(void)
{
	if(obd_comm.msg_type == 1)
	{
		HAL_TIM_Base_Stop_IT(KLINE_TIMER);
		obd_comm.msg_type = 0;
	}
	else if (obd_comm.msg_type == 2)
	{
		HAL_TIM_Base_Stop_IT(MSG_TIMER);
		obd_comm.msg_type = 0;
	}
}
