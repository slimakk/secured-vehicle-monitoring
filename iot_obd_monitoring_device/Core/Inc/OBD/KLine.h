/*
 * KLine.h
 *
 *  Created on: Nov 4, 2022
 *      Author: miros
 */

#ifndef INC_OBD_KLINE_H_
#define INC_OBD_KLINE_H_

#define KLINE &huart1
#define KLINE_TIMER &htim6

#include "main.h"
#include "OBD.h"
#include "usart.h"
#include "tim.h"

obd_protocol kline_init(void);

obd_protocol kwp2000_fast_init(void);

uint8_t kline_send_msg(uint8_t* tx_frame);

uint8_t kwp2000_send_msg(uint8_t* tx_frame);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void kline_rx_callback(void);

#endif /* INC_OBD_KLINE_H_ */
