/*
 * OBD.h
 *
 *  Created on: Nov 4, 2022
 *      Author: miros
 */

#ifndef INC_OBD_OBD_H_
#define INC_OBD_OBD_H_

typedef enum{
	OBD_NONE = 0,
	OBD_PROTO_ISO9141 = 1,
	OBD_PROTO_KWP2000_SLOW = 2,
	OBD_PROTO_KWP2000_FAST = 3,
	OBD_PROTO_CAN = 4,
} obd_protocol;

//void obd_init(obd_protocol first_protocol);

#include "CAN.h"
#include "main.h"

#define DEVICE_CAN_ID (0x7DF)

#define TX_DATA_LENGTH    (8)
#define RX_DATA_LENGTH    (8)

void OBD2_PrintResponse(uint8_t* rxFrame);

void OBD2_Request(obd_protocol obd, uint8_t pid);



#endif /* INC_OBD_OBD_H_ */
