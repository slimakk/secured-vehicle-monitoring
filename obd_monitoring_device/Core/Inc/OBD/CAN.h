/*
 * CAN.h
 *
 *  Created on: 23. 11. 2022
 *      Author: miros
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#define DEVICE_CAN_ID (0x7DF)

#include "main.h"

void MX_CAN1_Init(void);

void canConfig(void);

void CAN_SEND_MESSAGE(uint8_t* tx_frame);

#endif /* INC_CAN_H_ */
