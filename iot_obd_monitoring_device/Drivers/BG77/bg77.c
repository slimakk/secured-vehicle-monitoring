/*
 * bg77.c
 *
 *  Created on: Mar 27, 2023
 *      Author: miros
 */
#include "bg77.h"
#include "gpio.h"
#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern BG77 module;

uint8_t rx_data = 0;
uint8_t rx_buff[1500] = {0};
uint8_t rx_index = 0;

static uint8_t wake_up(void);
static void clear_rx_buff(void);
static void power_on(void);
static void power_off(void);
//TODO
uint8_t module_init(void)
{
	power_on();
	send_command("AT\r\n", "OK\r\n", 1000, NB);
	send_command("ATI\r\n", "OK\r\n", 1000, NB);
//	power_off();
	return 1;
}

uint8_t send_command(char *command, char *reply, uint16_t timeout, UART_HandleTypeDef *interface)
{
	module.received = 0;
//	wake_up();
	clear_rx_buff();
	uint8_t length = strlen(command);
	HAL_UARTEx_ReceiveToIdle_DMA(interface, rx_buff, 200);
	HAL_UART_Transmit(interface, (unsigned char *)command, length, timeout);

	while(module.received == 0)
	{
		__NOP();
	}
	if(strstr((char *)rx_buff, reply) != NULL)
	{
		return TRUE;
	}
	return FALSE;

}

void nb_rx_callback(void)
{
	module.received = 1;
}
//TODO
void gps_rx_callback(void)
{

}
uint8_t check_status(void)
{
	wake_up();
	if(send_command("AT+CEREG?\r\n", "OK\r\n", 1000, NB))
	{
		char *token = strtok((char *)rx_buff," ");
		if(token)
		{
			token = strtok(NULL,",");
			if(token)
			{
				token = strtok(NULL,"\n");
				if(token)
				{
					uint8_t status;
					char *ptr;
					status = strtol(token, &ptr, 10);
					return status;
				}
			}
		}
	}
	return 9;
}
uint8_t check_signal(void)
{
	if(send_command("AT+CSQ\r\n","OK\r\n",1000,NB))
	{
		char *token = strtok((char *)rx_buff, " ");
		if(token)
		{
			token = strtok(NULL,",");
			if(token)
			{
				char *ptr;
				uint8_t rssi = strtol(token, &ptr, 10);
				return rssi;
			}
		}
	}
	return FALSE;
}

/*********************************************************************************************/
/****************************************MQTT*************************************************/
/*********************************************************************************************/

uint8_t mqtt_open(const char* broker_address, uint8_t port, uint8_t id)
{
	wake_up();
	char command [255];
	sprintf(command, "AT+QMTOPEN=%d,\"%s\",%d\r\n", id, broker_address, port);
	if(send_command(command, "OK\r\n", 1000, NB))
	{
		char *token = strtok((char *)rx_buff, " ");
		if(token)
		{
			token = strtok(NULL,",");
			if(token)
			{
				token = strtok(NULL, "\n");
				if(token)
				{
					char *ptr;
					uint8_t status;
					status = strtol(token, &ptr, 10);
					if(status == 0)
					{
						return TRUE;
					}
					else
					{
						return FALSE;
					}
				}
			}
		}
	}
	return FALSE;
}

uint8_t mqtt_connect(uint8_t id, const char* client_id, BG77 module)
{
	char command [255];
	uint8_t ret [3];
	sprintf(command, "AT+QMTCONN=%d,\"%s\"\r\n",id,client_id);
	if(send_command(command, "OK\r\n", 1000, NB))
	{
		char *token = strtok((char *)rx_buff, " ");
		if(token)
		{
			token = strtok(NULL, ",");
			uint8_t i = 0;
			while(token != NULL)
			{
				char* ptr;
				ret[i] = strtol(token, &ptr, 10);
				token = strtok(NULL,",");
				i++;
			}
			switch(ret[1])
			{
				case 0:
					module.error = ret[2];
					return TRUE;
				case 1:
					module.error = ret[2];
					if(get_reply((char *)rx_buff, 3))
					{
						return TRUE;
					}
					else
					{
						return FALSE;
					}
				case 2:
					module.error = ret[2];
					return FALSE;
			}

		}
	}
	return FALSE;
}

uint8_t mqtt_disconnect(uint8_t id)
{
	char command [255];
	uint8_t ret [2];
	uint8_t i = 0;
	sprintf(command,"AT+QMTDISC=%d\r\n",id);
	if(send_command(command, "OK\r\n", 1000, NB))
	{
		char *token = strtok((char *)rx_buff, " ");
		if(token)
		{
			token = strtok(NULL, ",");
			while(token != NULL)
			{
				char *ptr;
				ret[i] = strtol(token, &ptr, 10);
				token = strtok(NULL, ",");
				i++;
			}
			if(ret[1] == 0)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
	return FALSE;
}

uint8_t mqtt_close(uint8_t id)
{
	char command [255];
	sprintf(command,"AT+QMTCLOSE=%d\r\n",id);
	if(send_command(command, "OK\r\n", 1000, NB))
	{
		return TRUE;
	}
	return FALSE;
}

uint8_t mqtt_subscribe(uint8_t id, uint8_t msg_id, const char *topic, uint8_t qos)
{
	char command[255];
	sprintf(command, "AT+QMTSUB=%d,%d,\"%s\",%d\r\n",id, msg_id, topic, qos);
	if(send_command(command, "OK\r\n", 1000, NB))
	{
		return TRUE;
	}
	return FALSE;
}

uint8_t mqtt_unsubscribe(uint8_t id, uint8_t msg_id, const char *topic, uint8_t qos)
{
	char command[255];
	sprintf(command, "AT+QMTUNS=%d,%d,\"%s\",%d\r\n",id, msg_id, topic, qos);
	if(send_command(command, "OK\r\n", 1000, NB))
	{
		return TRUE;
	}
	return FALSE;
}

uint8_t mqtt_publish(uint8_t id, uint8_t msg_id, uint8_t qos, uint8_t retain, const char *topic, const char *msg)
{
	char command[255];
	uint8_t msg_length = strlen(msg);
	sprintf(command, "AT+QMTPUB=%d,%d,%d,%d,\"%s\",%d,\"%s\"\r\n",id, msg_id, qos, retain, topic, msg_length, msg);
	if(send_command(command, "OK\r\n", 1000, NB))
	{
		return TRUE;
	}
	return FALSE;
}

/*********************************************************************************************/
/****************************************GNSS*************************************************/
/*********************************************************************************************/
uint8_t acquire_position(BG77 module)
{
	if(send_command("AT+QGPS=1\r\n","OK\r\n", 1000, NB))
	{
		if(send_command("AT+QGPSLOC?\r\n", "OK\r\n", 1000, NB))
		{

		}
		else
			return FALSE;
	}
	return FALSE;
}

void parse_location(uint8_t buff[])
{

}
/*********************************************************************************************/
/***************************************STATIC************************************************/
/*********************************************************************************************/

//TODO
static uint8_t wake_up(void)
{
	if(module.sleep == 1)
	{
		HAL_GPIO_WritePin(PON_TRIG_GPIO_Port, PON_TRIG_Pin, GPIO_PIN_SET);
		HAL_Delay(40);
		HAL_GPIO_WritePin(PON_TRIG_GPIO_Port, PON_TRIG_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
	}
	if(send_command("AT\r\n", "OK\r\n", 1000, NB))
	{
		return TRUE;
	}
	return FALSE;
}

static void power_on(void)
{
	HAL_GPIO_WritePin(IoT_PWR_GPIO_Port, IoT_PWR_Pin, GPIO_PIN_SET);
	HAL_Delay(600);
	HAL_GPIO_WritePin(IoT_PWR_GPIO_Port, IoT_PWR_Pin, GPIO_PIN_RESET);
}

static void power_off(void)
{
	HAL_GPIO_WritePin(IoT_PWR_GPIO_Port, IoT_PWR_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(IoT_PWR_GPIO_Port, IoT_PWR_Pin, GPIO_PIN_RESET);
}

static void clear_rx_buff(void)
{
	rx_index = 0;
	memset(rx_buff, 0, sizeof(rx_buff));
}
