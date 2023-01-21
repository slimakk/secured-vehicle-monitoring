/*
 * OBD.c
 *
 *  Created on: 23. 11. 2022
 *      Author: miros
 */
#include "OBD.h"

UART_HandleTypeDef huart2;
static obd_protocol usedProtocol;

//void OBD2_PrintResponse(uint8_t* rxFrame)
//{
//	char txTest2 [TX_DATA_LENGTH] = {0};
//
//	for(int i = 0; i < sizeof(rxFrame); i++)
//	{
//		char value[2];
//		snprintf(value, 2, "%x", rxFrame[i]);
//		strcat(txTest2, value);
//	}
//	strcat(txTest2, "\r\n");
//	HAL_UART_Transmit(&huart2, (uint8_t *)txTest2, sizeof(txTest2),10);
//}

void OBD2_Request(obd_protocol obd, uint8_t pid)
{
	if(obd == OBD_PROTO_CAN)
	{
		uint8_t txDataCAN[TX_DATA_LENGTH] = {0x02, 0x01, pid, 0x00, 0x00, 0x00, 0x00, 0x00};
		CAN_SEND_MESSAGE(txDataCAN);
	}
	else if(obd == OBD_PROTO_ISO9141 || OBD_PROTO_KWP2000_SLOW || OBD_PROTO_KWP2000_FAST)
	{
		uint8_t txDataISO[2] = {0x01, pid};
		if(obd == OBD_PROTO_ISO9141)
		{
			KLine_SEND_MESSAGE(txDataISO);
		}
		else
		{
			KWP2000_SEND_MESSAGE(txDataISO);
		}
	}
}

float OBD2_PID_Parse(uint8_t* rxFrame)
{
	float value = 0;
	switch(rxFrame[2])
	{
	case 0x04:
		value = (100/255)*rxFrame[3];
		break;
	case 0x05:
		value = rxFrame[3] - 40;
		break;
	case 0x06: case 0x07: case 0x08: case 0x09:
		value = (100/128)*rxFrame[3] - 100;
		break;
	case 0x0A:
		value = 3 * rxFrame[3];
		break;
	case 0x0B:
		value = rxFrame[3];
		break;
	case 0x0C:
		value = ((rxFrame[3] << 8) | rxFrame[4]) / 4;
		break;
	case 0x0D:
		value = rxFrame[3];
		break;
	case 0x0E:
		value = (rxFrame[3] / 2) - 64;
		break;
	case 0x0F:
		value = rxFrame[3] - 40;
		break;
	case 0x10:
		value = ((rxFrame[3] << 8) | rxFrame[4]) / 100;
		break;
	case 0x11:
		value = (100/255)*rxFrame[3];
		break;
	case 0x14: case 0x15: case 0x16: case 0x17: case 0x18: case 0x19: case 0x1A: case 0x1B:
		value = rxFrame[3] / 200;
		break;
	case 0x1F: case 0x21:
		value = (rxFrame[3] << 8) | rxFrame[4];
		break;
	case 0x22:
		value = ((rxFrame[3] << 8) | rxFrame[4])*0.079;
	case 0x23:
		value = 10*((rxFrame[3] << 8) | rxFrame[4]);
		break;
	case 0x24: case 0x25: case 0x26: case 0x27: case 0x28: case 0x29: case 0x2A: case 0x2B:
		value = (1/32768)*((rxFrame[3] << 8) | rxFrame[4]);
		break;
	case 0x2C:
		value = (100/255)*rxFrame[3];
		break;
	case 0x2D:
		value = (100/128)*rxFrame[3] - 100;
		break;
	case 0x2E: case 0x2F:
		value = (100/255)*rxFrame[3];
		break;
	case 0x30:
		value = rxFrame[3];
		break;
	case 0x31:
		value = (rxFrame[3] << 8) | rxFrame[4];
		break;
	case 0x32:
		value = ((rxFrame[3] << 8) | rxFrame[4]) / 4;
		break;
	case 0x33:
		value = rxFrame[3];
		break;
	case 0x34: case 0x35: case 0x36: case 0x37: case 0x38: case 0x39: case 0x3A: case 0x3B:
		value = (1/32768)*((rxFrame[3] << 8) | rxFrame[4]);
		break;
	case 0x3C: case 0x3D: case 0x3E: case 0x3F:
		value = (((rxFrame[3] << 8) | rxFrame[4]) / 100) - 40;
		break;
	case 0x42:
		value = ((rxFrame[3] << 8) | rxFrame[4]) / 1000;
		break;
	case 0x43:
		value = (100/255)* ((rxFrame[3] << 8) | rxFrame[4]);
		break;
	case 0x44:
		value = (1/32768)*((rxFrame[3] << 8) | rxFrame[4]);
		break;
	case 0x45:
		value = (100/255)*rxFrame[3];
		break;
	case 0x46:
		value = rxFrame[3] - 40;
		break;
	case 0x47: case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C:
		value = (100/255)*rxFrame[3];
		break;
	case 0x4D: case 0x4E:
		value = (rxFrame[3] << 8) | rxFrame[4];
		break;
	case 0x4F:
		value = rxFrame[3];
		break;
	case 0x52:
		value = (100/255)*rxFrame[3];
		break;
	case 0x53:
		value = ((rxFrame[3] << 8) | rxFrame[4]) * 0.005;
		break;
	case 0x54:
		value = ((rxFrame[3] << 8) | rxFrame[4]) - 32767;
		break;
	case 0x55: case 0x56: case 0x57: case 0x58:
		value = ((100/128)*rxFrame[3]) - 100;
		break;
	case 0x59:
		value = (rxFrame[3] << 8) | rxFrame[4];
		break;
	case 0x5A: case 0x5B:
		value = (100/255)*rxFrame[3];
		break;
	case 0x5C:
		value =  rxFrame[3] - 40;
		break;
	case 0x5D:
		value = ((100/128)*rxFrame[3]) - 210;
		break;
	case 0x5E:
		value = ((rxFrame[3] << 8) | rxFrame[4]) * 0.05;
		break;
	case 0x61: case 0x62:
		value = rxFrame[3] - 125;
		break;
	case 0x63:
		value = (rxFrame[3] << 8) | rxFrame[4];
		break;
	case 0x64:
		value = rxFrame[3] - 125;
		break;
	case 0x66:
		value = ((rxFrame[3] << 8) | rxFrame[4]) * (1/32);
		break;
	case 0x67: case 0x68:
		value = rxFrame[3] - 40;
		break;
	case 0x7C:
		value = (((rxFrame[3] << 8) | rxFrame[4]) / 100) - 40;
		break;
	case 0xA6:
		value = ((rxFrame[3] << 24) | (rxFrame[4] << 16) | (rxFrame[5] << 8) | rxFrame[6]) * 0.1;
		break;
	}
	return value;
}

obd_protocol OBD2_Init(void)
{
	usedProtocol = KLine_Init();
	if(usedProtocol == OBD_NONE)
	{
		usedProtocol = KWP2000_Fast_Init();
		if(usedProtocol == OBD_NONE)
		{
			usedProtocol = OBD_PROTO_CAN;
			MX_CAN1_Init();
			canConfig();
		}
	}
	return usedProtocol;
}

void OBD2_ShowOnDisplay(float value)
{
	char str[10];
	snprintf(str, 10, "%f", value);
	ssd1306_SetCursor(0,0);
	ssd1306_Fill(Black);
	ssd1306_WriteString("Teplota", Font_7x10, White);
	ssd1306_SetCursor(40, 20);
	ssd1306_WriteString(str, Font_16x26, White);
	ssd1306_UpdateScreen();
}
