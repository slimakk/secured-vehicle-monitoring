/*
 * KLine.h
 *
 *  Created on: Nov 4, 2022
 *      Author: miros
 */

#ifndef INC_OBD_KLINE_H_
#define INC_OBD_KLINE_H_
#include "OBD.h"
#include "main.h"

extern UART_HandleTypeDef huart1;

obd_protocol KLine_Init(void);
obd_protocol KWP2000_Fast_Init(void);

void MX_GPIO_KLineUART_Init(void);
void UART_PIN_State(int state);

void KLine_SEND_MESSAGE(uint8_t* txFrame);

void KWP2000_SEND_MESSAGE(uint8_t* txFrame);

int Verify_Checksum (uint8_t *data, uint8_t lenght);

void MX_USART1_UART_Init(int baudRate);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);


#endif /* INC_OBD_KLINE_H_ */