/*
 * CAN.h
 *
 *  Created on: Nov 5, 2022
 *      Author: miros
 */

#ifndef INC_OBD_CAN_H_
#define INC_OBD_CAN_H_

#include "stm32l4xx_hal.h"

void CAN_RESPONSE(uint8_t serviceID, uint8_t lenght, uint8_t PID, CAN_HandleTypeDef *hcan);

uint8_t* CAN_REQUEST_HANDLE(CAN_HandleTypeDef *hcan);


#endif /* INC_OBD_CAN_H_ */