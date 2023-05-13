/*
 * OBD.c
 *
 *  Created on: 23. 11. 2022
 *      Author: miros
 */
#include "OBD.h"
#include "OBD_PID.h"
#include "can.h"
#include "KLine.h"
#include <stdio.h>

extern OBD obd_comm;

static void obd2_pid_decode(uint8_t* rx_frame);

static void obd2_pid_decode(uint8_t* rx_frame)
{
	uint8_t number = (rx_frame[3] << 24) | (rx_frame[4] << 16) | (rx_frame[5] << 8) | rx_frame[6];
	uint8_t j = 0;
	uint8_t k = 32;
	uint8_t l = 64;
	for(uint8_t i = 31; i >= 0; i--)
	{
		uint8_t digit = number >> i;
		digit &= 1;
		if(obd_comm.pid == 0x00)
		{
			obd_comm.available_pids[j] = digit;
			j++;
		}
		else if(obd_comm.pid == 0x20)
		{
			obd_comm.available_pids[k] = digit;
			k++;
		}
		else
		{
			obd_comm.available_pids[l] = digit;
			l++;
		}
	}
}

uint8_t obd2_request(OBD obd)
{
	HAL_Delay(10);
	uint8_t repeat = 0;
	if(obd.used_protocol == OBD_PROTO_CAN)
	{
		uint8_t tx_data_CAN[TX_DATA_LENGTH] = {0x02, 0x01, obd.pid, 0x00, 0x00, 0x00, 0x00, 0x00};
		while (!(can_send_msg(tx_data_CAN)))
		{
			if(repeat < MAX_REQ_REPEAT)
			{
				repeat++;
			}
			else
			{
				return (FALSE);
			}
		}
		return (TRUE);
	}
	else if(obd.used_protocol == OBD_PROTO_ISO9141 || OBD_PROTO_KWP2000_SLOW || OBD_PROTO_KWP2000_FAST)
	{
		uint8_t tx_data_ISO[2] = {0x01, obd.pid};
		if(obd.used_protocol == OBD_PROTO_ISO9141)
		{
			while (!(kline_send_msg(tx_data_ISO)))
			{
			if(repeat < MAX_REQ_REPEAT)
			{
				repeat++;
			}
			else
			{
				return (FALSE);
			}
			}
			return (TRUE);
		}
		else
		{
			while (!(kwp2000_send_msg(tx_data_ISO)))
			{
			if(repeat < MAX_REQ_REPEAT)
			{
				repeat++;
			}
			else
			{
				return (FALSE);
			}
			}
			return (TRUE);
		}
		return (FALSE);
	}
	return (FALSE);
}

float obd2_pid_parse(uint8_t* rx_frame)
{
	float value = 0;
	switch(rx_frame[2])
	{
	case 0x00:
		obd2_pid_decode(rx_frame);
		value = 0;
		break;
	case 0x04:
		value = (100/255)*rx_frame[3];
		break;
	case 0x05:
		value = rx_frame[3] - 40;
		break;
	case 0x06: case 0x07: case 0x08: case 0x09:
		value = (100/128)*rx_frame[3] - 100;
		break;
	case 0x0A:
		value = 3 * rx_frame[3];
		break;
	case 0x0B:
		value = rx_frame[3];
		break;
	case 0x0C:
		value = ((rx_frame[3] << 8) | rx_frame[4]) / 4;
		break;
	case 0x0D:
		value = rx_frame[3];
		break;
	case 0x0E:
		value = (rx_frame[3] / 2) - 64;
		break;
	case 0x0F:
		value = rx_frame[3] - 40;
		break;
	case 0x10:
		value = ((rx_frame[3] << 8) | rx_frame[4]) / 100;
		break;
	case 0x11:
		value = (100/255)*rx_frame[3];
		break;
	case 0x14: case 0x15: case 0x16: case 0x17: case 0x18: case 0x19: case 0x1A: case 0x1B:
		value = rx_frame[3] / 200;
		break;
	case 0x1F: case 0x21:
		value = (rx_frame[3] << 8) | rx_frame[4];
		break;
	case 0x20:
		obd2_pid_decode(rx_frame);
		value = 0;
		break;
	case 0x22:
		value = ((rx_frame[3] << 8) | rx_frame[4])*0.079;
		break;
	case 0x23:
		value = 10*((rx_frame[3] << 8) | rx_frame[4]);
		break;
	case 0x24: case 0x25: case 0x26: case 0x27: case 0x28: case 0x29: case 0x2A: case 0x2B:
		value = (1/32768)*((rx_frame[3] << 8) | rx_frame[4]);
		break;
	case 0x2C:
		value = (100/255)*rx_frame[3];
		break;
	case 0x2D:
		value = (100/128)*rx_frame[3] - 100;
		break;
	case 0x2E: case 0x2F:
		value = (100/255)*rx_frame[3];
		break;
	case 0x30:
		value = rx_frame[3];
		break;
	case 0x31:
		value = (rx_frame[3] << 8) | rx_frame[4];
		break;
	case 0x32:
		value = ((rx_frame[3] << 8) | rx_frame[4]) / 4;
		break;
	case 0x33:
		value = rx_frame[3];
		break;
	case 0x34: case 0x35: case 0x36: case 0x37: case 0x38: case 0x39: case 0x3A: case 0x3B:
		value = (1/32768)*((rx_frame[3] << 8) | rx_frame[4]);
		break;
	case 0x3C: case 0x3D: case 0x3E: case 0x3F:
		value = (((rx_frame[3] << 8) | rx_frame[4]) / 100) - 40;
		break;
	case 0x40:
		obd2_pid_decode(rx_frame);
		value = 0;
		break;
	case 0x42:
		value = ((rx_frame[3] << 8) | rx_frame[4]) / 1000;
		break;
	case 0x43:
		value = (100/255)* ((rx_frame[3] << 8) | rx_frame[4]);
		break;
	case 0x44:
		value = (1/32768)*((rx_frame[3] << 8) | rx_frame[4]);
		break;
	case 0x45:
		value = (100/255)*rx_frame[3];
		break;
	case 0x46:
		value = rx_frame[3] - 40;
		break;
	case 0x47: case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C:
		value = (100/255)*rx_frame[3];
		break;
	case 0x4D: case 0x4E:
		value = (rx_frame[3] << 8) | rx_frame[4];
		break;
	case 0x4F:
		value = rx_frame[3];
		break;
	case 0x52:
		value = (100/255)*rx_frame[3];
		break;
	case 0x53:
		value = ((rx_frame[3] << 8) | rx_frame[4]) * 0.005;
		break;
	case 0x54:
		value = ((rx_frame[3] << 8) | rx_frame[4]) - 32767;
		break;
	case 0x55: case 0x56: case 0x57: case 0x58:
		value = ((100/128)*rx_frame[3]) - 100;
		break;
	case 0x59:
		value = (rx_frame[3] << 8) | rx_frame[4];
		break;
	case 0x5A: case 0x5B:
		value = (100/255)*rx_frame[3];
		break;
	case 0x5C:
		value =  rx_frame[3] - 40;
		break;
	case 0x5D:
		value = ((100/128)*rx_frame[3]) - 210;
		break;
	case 0x5E:
		value = ((rx_frame[3] << 8) | rx_frame[4]) * 0.05;
		break;
	case 0x61: case 0x62:
		value = rx_frame[3] - 125;
		break;
	case 0x63:
		value = (rx_frame[3] << 8) | rx_frame[4];
		break;
	case 0x64:
		value = rx_frame[3] - 125;
		break;
	case 0x66:
		value = ((rx_frame[3] << 8) | rx_frame[4]) * (1/32);
		break;
	case 0x67: case 0x68:
		value = rx_frame[3] - 40;
		break;
	case 0x7C:
		value = (((rx_frame[3] << 8) | rx_frame[4]) / 100) - 40;
		break;
	case 0xA6:
		value = ((rx_frame[3] << 24) | (rx_frame[4] << 16) | (rx_frame[5] << 8) | rx_frame[6]) * 0.1;
		break;
	default:
		value = 255;
		break;
	}
	return (value);
}

obd_protocol obd2_init(void)
{
	obd_protocol used_protocol = kline_init();
	if(used_protocol == OBD_NONE)
	{
		used_protocol = kwp2000_fast_init();
		if(used_protocol == OBD_NONE)
		{
			used_protocol = OBD_PROTO_CAN;
			MX_CAN1_Init();
			can_config();
		}
	}
	return (used_protocol);
}

void obd2_pid_check(OBD obd)
{
	obd.pid = 0x00;
	obd2_request(obd);
	obd.pid = 0x20;
	obd2_request(obd);
	obd.pid = 0x40;
	obd2_request(obd);
	uint8_t j = 1;
	for(uint8_t i = 0; i < 96; i++)
	{
		if(obd.available_pids[i] == 1)
		{
			obd.pids[obd.pid_count] = j;
			obd.pid_count++;
		}
		j++;
	}
}
