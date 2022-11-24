/*
 * CAN.h
 *
 *  Created on: 23. 11. 2022
 *      Author: miros
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include "main.h"
#include "OBD.h"

extern CAN_HandleTypeDef hcan1;

void MX_CAN1_Init(void);

void canConfig(void);

void CAN_SEND_MESSAGE(uint8_t* txFrame);

#endif /* INC_CAN_H_ */
