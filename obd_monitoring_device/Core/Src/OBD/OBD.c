/*
 * OBD.c
 *
 *  Created on: 23. 11. 2022
 *      Author: miros
 */
#include "OBD.h"
#include "OBD_PID.h"
#include "CAN.h"
#include "KLine.h"
#include <stdio.h>

static obd_protocol used_protocol;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern OBD obd_comm;
extern char *pid_names[90];

static void OBD2_PID_Decode(uint8_t* rx_frame);

//void OBD2_PrintResponse(uint8_t* rx_frame)
//{
//	char txTest2 [TX_DATA_LENGTH] = {0};
//
//	for(int i = 0; i < sizeof(rx_frame); i++)
//	{
//		char value[2];
//		snprintf(value, 2, "%x", rx_frame[i]);
//		strcat(txTest2, value);
//	}
//	strcat(txTest2, "\r\n");
//	HAL_UART_Transmit(&huart2, (uint8_t *)txTest2, sizeof(txTest2),10);
//}

static void OBD2_PID_Decode(uint8_t* rx_frame)
{
	int number = (rx_frame[3] << 24) | (rx_frame[4] << 16) | (rx_frame[5] << 8) | rx_frame[6];
	int j = 0;
	for(int i = 31; i >= 0; i--)
	{
		int digit = number >> i;
		digit &= 1;
		if(obd_comm.pid == 0x00)
		{
			obd_comm.available_pids_1[j] = digit;
		}
		else if(obd_comm.pid == 0x20)
		{
			obd_comm.available_pids_2[j] = digit;
		}
		else
		{
			obd_comm.available_pids_3[j] = digit;
		}
		j++;
	}
}

void OBD2_Request(OBD obd)
{
	HAL_Delay(10);
	if(obd.used_protocol == OBD_PROTO_CAN)
	{
		uint8_t tx_data_CAN[TX_DATA_LENGTH] = {0x02, 0x01, obd.pid, 0x00, 0x00, 0x00, 0x00, 0x00};
		CAN_SEND_MESSAGE(tx_data_CAN);
	}
	else if(obd.used_protocol == OBD_PROTO_ISO9141 || OBD_PROTO_KWP2000_SLOW || OBD_PROTO_KWP2000_FAST)
	{
		uint8_t tx_data_ISO[2] = {0x01, obd.pid};
		if(obd.used_protocol == OBD_PROTO_ISO9141)
		{
			KLine_SEND_MESSAGE(tx_data_ISO);
		}
		else
		{
			KWP2000_SEND_MESSAGE(tx_data_ISO);
		}
	}
}

float OBD2_PID_Parse(uint8_t* rx_frame)
{
	float value = 0;
	switch(rx_frame[2])
	{
	case 0x00:
		OBD2_PID_Decode(rx_frame);
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
		OBD2_PID_Decode(rx_frame);
		value = 0;
		break;
	case 0x22:
		value = ((rx_frame[3] << 8) | rx_frame[4])*0.079;
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
		OBD2_PID_Decode(rx_frame);
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
	}
	return value;
}

obd_protocol OBD2_Init(void)
{
	used_protocol = KLine_Init();
	if(used_protocol == OBD_NONE)
	{
		used_protocol = KWP2000_Fast_Init();
		if(used_protocol == OBD_NONE)
		{
			used_protocol = OBD_PROTO_CAN;
			HAL_DMA_DeInit(&hdma_usart1_rx);
			MX_CAN1_Init();
			canConfig();
		}
	}
	return used_protocol;
}

void OBD2_ShowOnDisplay(float value)
{
	char str[10];
	snprintf(str, 10, "%f", value);
	ssd1306_SetCursor(0,0);
	ssd1306_Fill(Black);
	ssd1306_WriteString(pid_names[obd_comm.pid_index], Font_7x10, White);
	ssd1306_SetCursor(40, 20);
	ssd1306_WriteString(str, Font_16x26, White);
	ssd1306_UpdateScreen();
}
