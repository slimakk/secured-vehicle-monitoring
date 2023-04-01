/*
 * KLine.h
 *
 *  Created on: Nov 4, 2022
 *      Author: miros
 */

#ifndef INC_OBD_KLINE_H_
#define INC_OBD_KLINE_H_

#define KLINE &huart1

#include "main.h"
#include "OBD.h"
#include "usart.h"

obd_protocol KLine_Init(void);

obd_protocol KWP2000_Fast_Init(void);

void KLine_SEND_MESSAGE(uint8_t* tx_frame);

void KWP2000_SEND_MESSAGE(uint8_t* tx_frame);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void kline_rx_callback(void);

#endif /* INC_OBD_KLINE_H_ */
