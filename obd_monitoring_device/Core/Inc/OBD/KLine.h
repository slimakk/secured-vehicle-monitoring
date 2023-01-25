/*
 * KLine.h
 *
 *  Created on: Nov 4, 2022
 *      Author: miros
 */

#ifndef INC_OBD_KLINE_H_
#define INC_OBD_KLINE_H_

#include "main.h"
#include "OBD.h"

obd_protocol KLine_Init(void);

obd_protocol KWP2000_Fast_Init(void);

void KLine_SEND_MESSAGE(uint8_t* txFrame);

void KWP2000_SEND_MESSAGE(uint8_t* txFrame);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_OBD_KLINE_H_ */
