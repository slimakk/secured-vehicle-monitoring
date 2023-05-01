/*
 * OBD.h
 *
 *  Created on: Nov 4, 2022
 *      Author: miros
 */

#ifndef INC_OBD_OBD_H_
#define INC_OBD_OBD_H_

#define TX_DATA_LENGTH    (8)
#define RX_DATA_LENGTH    (8)

#include "main.h"

typedef enum{
	OBD_NONE = 0,
	OBD_PROTO_ISO9141 = 1,
	OBD_PROTO_KWP2000_SLOW = 2,
	OBD_PROTO_KWP2000_FAST = 3,
	OBD_PROTO_CAN = 4,
} obd_protocol;

typedef struct {
	obd_protocol used_protocol;
	uint8_t pid;
	uint8_t pid_index;
	uint8_t available_pids[96];
//	uint8_t available_pids_2 [32];
//	uint8_t available_pids_3 [32];
	uint8_t pid_count;
	uint8_t pids[96];
	uint8_t msg_type;
	float current_value;
	float voltage;
} OBD;

void OBD2_Request(OBD obd);

float OBD2_PID_Parse(uint8_t *rx_frame);

obd_protocol OBD2_Init(void);

void OBD2_ShowOnDisplay(float value);

void OBD2_pid_check(OBD obd);


#endif /* INC_OBD_OBD_H_ */
