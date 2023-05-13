/*
 *	bg77.c
 *	Driver for Quectel BG77 Narrowband IoT module
 *  Created on: Mar 27, 2023
 *      Author: slimakk
 */
#include "bg77.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern BG77 module;

uint8_t rx_data = 0;
uint8_t rx_buffer[100] = {0};

static uint8_t wake_up(void);
static void clear_rx_buff(void);
static void power_on(void);
static void power_off(void);

/*
 *	@brief Module intialization
 *	Turns on the module with powerkey and checks if it is responding
 *	@param module	pointer to a BG77 struct
 */
uint8_t module_init(BG77 *module)
{
	uint8_t repeat = 0;
	power_on();
	while(send_command("AT\r\n", "OK", DEFAULT_TIMEOUT, NB) != (TRUE))
	{
		if(repeat > MAX_REPEAT)
		{
			return (FALSE);
		}
		repeat++;
	}
	if(send_command("ATE0\r\n", "OK", DEFAULT_TIMEOUT, NB))
	{
		return (TRUE);
	}
	return (FALSE);
}
/*
 *	@brief Sends AT command and checks the reply
 *	@param command		AT command to send
 *	@param reply		Expected reply
 *	@param timeout		Maximum time to send the command
 *	@param interface	UART interface handle
 *	@retval (TRUE) if reply matches the expected reply
 */
uint8_t send_command(char *command, char *reply, uint16_t timeout, UART_HandleTypeDef *interface)
{
	module.received = 0;
	clear_rx_buff();
//	__HAL_UART_SEND_REQ(NB, UART_RXDATA_FLUSH_REQUEST);
	uint8_t length = strlen(command);
	HAL_UART_Receive_IT(interface, &rx_data, 1);
	HAL_UART_Transmit(interface, (unsigned char *)command, length, timeout);
//	HAL_TIM_Base_Start_IT(NB_TIMER);
	__HAL_TIM_CLEAR_FLAG(UART_TIMER, TIM_SR_UIF);
	HAL_TIM_Base_Start_IT(UART_TIMER);
	while(module.received == 0)
	{
		__NOP();
	}
	if(module.received != 1)
	{
		return (FALSE);
	}
	if(strstr((char *)rx_buffer, reply) != NULL)
	{
		return (TRUE);
	}
	return (FALSE);
}
/*
 *	@brief Module response Callback
 *	@retval	none
 */
void nb_rx_callback(void)
{
	rx_buffer[module.rx_index++] = rx_data;
	HAL_TIM_Base_Stop_IT(UART_TIMER);
	__HAL_TIM_CLEAR_FLAG(UART_TIMER, TIM_SR_UIF);
	HAL_TIM_Base_Start_IT(UART_TIMER);
	HAL_UART_Receive_IT(NB, &rx_data, 1);
}
/*
 *	@brief	Checks status of NB module
 *	@param	module pointer to a BG77 struct
 *	@retval	status according to  Quectel AT Commands Manual V2.0
 */
//TODO
uint8_t check_status(BG77 *module)
{
	wake_up();
	if(!(send_command("AT+CEREG?\r\n", "OK", DEFAULT_TIMEOUT, NB)))
	{
		return (FALSE);
	}
	char *token = strtok((char *)rx_buffer," ");
	if(!token)
	{
		return (FALSE);
	}
	token = strtok(NULL,",");
	if(token)
	{
		token = strtok(NULL,"\n");
		if(token)
		{
			uint8_t status;
			char *ptr;
			status = strtol(token, &ptr, 10);
			return (status);
		}
	}
	return (9);
}
/*
 *	@brief	Checks the RSSI for NB signal
 *	@retval	rssi or (FALSE) if the the is no response
 */
uint8_t check_signal(void)
{
	if(!(send_command("AT+CSQ\r\n","OK",DEFAULT_TIMEOUT, NB)))
	{
		return (FALSE);
	}
	char *token = strtok((char *)rx_buffer, " ");
	if(token)
	{
		token = strtok(NULL,",");
		if(token)
		{
			char *ptr;
			uint8_t rssi = strtol(token, &ptr, 10);
			return (rssi);
		}
		return (FALSE);
	}
	return (FALSE);
}
/*
 *	@brief	PSM setup
 *	@param	tau	 		Requested extended periodic TAU value (T3412) in binary (One byte in 8 bits)
 *	@param	active_time	Requested Active Time value (T3324) in binary (One byte in 8 bits)
 *	@param	mode		Enable or Disable (1 or 0)
 *	@retval	(TRUE) or (FALSE) depending on the response
 */
uint8_t set_psm(const char* tau, const char* active_time, uint8_t mode)
{
	char command[COMMAND_SIZE];
	sprintf(command, "AT+CPSMS=%d,,,\"%s\",\"%s\"",mode, tau, active_time);
	if(send_command(command, "OK", DEFAULT_TIMEOUT, NB))
	{
		return (TRUE);
	}
	return (FALSE);
}
/*********************************************************************************************/
/****************************************MQTT*************************************************/
/*********************************************************************************************/
/*
 *	@brief	Opens UDP connection to MQTT broker
 *	@param	broker_address	IP Adress of desired MQTT broker
 *	@param	port			Port of desired MQTT broker
 *	@param	id				Connection ID (0-5)
 *	@retval	(TRUE) or (FALSE) depending on the response
 * */
uint8_t mqtt_open(const char* broker_address, uint16_t port, uint8_t id)
{
//	wake_up();
	char command [COMMAND_SIZE];
	uint8_t ret [2] = {255};
	uint8_t i = 0;
	sprintf(command, "AT+QMTOPEN=%d,\"%s\",%d\r\n", id, broker_address, port);
	if(!(send_command(command, "OK", DEFAULT_TIMEOUT, NB)))
	{
		return (FALSE);
	}
	char *token = strtok((char*)rx_buffer, " ");
	if(!token)
	{
		return (FALSE);
	}
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
		return (TRUE);
	}
	return (ret[1]);
}
/*
 * @brief	Connects to the MQTT broker
 * @param	id			Connection ID (0-5)
 * @param	client_id	Clients ID for the MQTT broker
 * @param	module		pointer to a BG77 struct
 * @retval	return code for the command
 */
//TODO
uint8_t mqtt_connect(uint8_t id, const char* client_id, BG77 *module)
{
	char command [COMMAND_SIZE];
	uint8_t ret [3] = {255};
	uint8_t i = 0;
	sprintf(command, "AT+QMTCONN=%d,\"%s\"\r\n",id,client_id);
	if(!(send_command(command, "OK", DEFAULT_TIMEOUT, NB)))
	{
		return (FALSE);
	}
	char *token = strtok((char *)rx_buffer, " ");
	if(!token)
	{
		return (FALSE);
	}
	token = strtok(NULL, ",");
	while(token != NULL)
	{
		char *ptr;
		ret[i] = strtol(token, &ptr, 10);
		i++;
		token = strtok(NULL, ",");
	}
	if(ret[1] == (0 | 1))
	{
		switch(ret[2])
		{
			case 0:
				return (TRUE);
				break;
			default:
				module->error = ret[2];
				return (FALSE);
		}
	}
	return (FALSE);
}
/*
 *	@brief	Disconnects for MQTT broker
 *	@param	id	 	Connection ID (0-5)
 *	@retval	(TRUE) if the disconnect is successful
 */
uint8_t mqtt_disconnect(uint8_t id)
{
	char command [COMMAND_SIZE];
	uint8_t ret [2] = {255};
	uint8_t i = 0;
	sprintf(command,"AT+QMTDISC=%d\r\n",id);
	if(!(send_command(command, "OK", DEFAULT_TIMEOUT, NB)))
	{
		return (FALSE);
	}
	char *token = strtok((char *)rx_buffer, " ");
	if(!token)
	{
		return (FALSE);
	}
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
		return (TRUE);
	}
	return (FALSE);
}
/*
 *	@brief	Closes the TCP connection to the MQTT broker
 *	@param	id	 	Connection ID (0-5)
 *	@param	module	BG77 struct
 *	@retval	0 if the closure is successful, else or 1
 */
uint8_t mqtt_close(uint8_t id, BG77 module)
{
	char command [COMMAND_SIZE];
	uint8_t i = 0;
	uint8_t ret = 255;
	sprintf(command,"AT+QMTCLOSE=%d\r\n",id);
	if((send_command(command, "OK", DEFAULT_TIMEOUT, NB)))
	{
		return (1);
	}
	char *token = strtok((char *)rx_buffer, " ");
	if(!token)
	{
		return (1);
	}
	token = strtok(NULL, ",");
	while(token != NULL)
	{
		if(i == 1)
		{
			char *ptr;
			ret = strtol(token, &ptr, 10);
		}
		token = strtok(NULL, ",");
		i++;
	}
	if(ret == 0)
	{
		return (0);
	}
	return (1);
}
/*
 *	@brief	Subscribes to certain topic
 *	@param	id	 	Connection ID (0-5)
 *	@param	msg_id	Message ID (1-65535)
 *	@param	topic	MQTT topic
 *	@param	qos		MQTT QoS level (0-2)
 *	@retval	(TRUE) if the subscription is successful
 */
uint8_t mqtt_subscribe(uint8_t id, uint8_t msg_id, const char *topic, uint8_t qos)
{
	char command[COMMAND_SIZE];
	sprintf(command, "AT+QMTSUB=%d,%d,\"%s\",%d\r\n",id, msg_id, topic, qos);
	if(send_command(command, "OK", DEFAULT_TIMEOUT, NB))
	{
		return (TRUE);
	}
	return (FALSE);
}
/*
 *	@brief	Unsubscribes from certain topic
 *	@param	id	 	Connection ID (0-5)
 *	@param	msg_id	Message ID (1-65535)
 *	@param	topic	MQTT topic
 *	@retval	(TRUE) if the unsubscription is successful
 */
uint8_t mqtt_unsubscribe(uint8_t id, uint8_t msg_id, const char *topic)
{
	char command[COMMAND_SIZE];
	sprintf(command, "AT+QMTUNS=%d,%d,\"%s\"\r\n",id, msg_id, topic);
	if(send_command(command, "OK", DEFAULT_TIMEOUT, NB))
	{
		return (TRUE);
	}
	return (FALSE);
}
/*
 *	@brief	Publishes data to MQTT broker
 *	@param	id	 	Connection ID (0-5)
 *	@param	msg_id	Message ID (1-65535)
 *	@param	qos		MQTT QoS level (0-2)
 *	@param	retain	Data retention on the MQTT broker
 *	@param	topic	MQTT topic
 *	@param	msg		data to be published
 *	@retval	(TRUE) if the publication is successful
 */
uint8_t mqtt_publish(uint8_t id, uint8_t msg_id, uint8_t qos, uint8_t retain, const char *topic, const char *msg)
{
	char command[COMMAND_SIZE];
	sprintf(command, "AT+QMTPUBEX=%d,%d,%d,%d,\"%s\",\"%s\"\r\n",id, msg_id, qos, retain, topic, msg);
	if(send_command(command, "OK", DEFAULT_TIMEOUT, NB))
	{
		return (TRUE);
	}
	return (FALSE);
}

/*********************************************************************************************/
/****************************************GNSS*************************************************/
/*********************************************************************************************/
/*
 *	@brief	Acquires positional data from the GNSS portion of the module
 *	@param	module	BG77 struct
 *	@retval	(TRUE) if GNSS has a fix and responds with positional data
 */
uint8_t acquire_position(BG77 module)
{
	wake_up();
	if(!(send_command("AT+QGPS=1\r\n","OK", DEFAULT_TIMEOUT, NB)))
	{
		return (FALSE);
	}
	if(send_command("AT+QGPSLOC?\r\n", "OK", DEFAULT_TIMEOUT, NB))
	{
		parse_location(module);
		return (TRUE);
	}
	return (FALSE);
}
/*
 *	@brief Splits received location string into individual variables from location struct
 *	@param	module	BG77 struct
 *	@retval	None
 */
void parse_location(BG77 module)
{
	char *token = strtok((char *)rx_buffer, " ");
	if(token)
	{
		token = strtok(NULL, ",");
		uint8_t i = 0;
		while(token != NULL)
		{
			char *ptr;
			switch(i)
			{
				case 0:
					module.pos.time = strtol(token, &ptr, 10);
					break;
				case 1:
					module.pos.latitude = strtod(token, &ptr);
					module.pos.lat_ort = ptr;
					break;
				case 2:
					module.pos.longitude = strtod(token, &ptr);
					module.pos.long_ort = ptr;
					break;
				case 4:
					module.pos.altitude = strtod(token, &ptr);
					break;
				case 7:
					module.pos.speed = strtod(token, &ptr);
					break;
				case 9:
					module.pos.date = strtol(token, &ptr, 10);
					break;
				default:
					break;
			}
			token = strtok(NULL, ",");
			i++;
		}
	}
}
/*********************************************************************************************/
/***************************************STATIC************************************************/
/*********************************************************************************************/
/*
 *	@brief	Wakes up the module from sleep and checks its connection
 *	@param	None
 *	@retval	TRUE if the module is responding
 */
//TODO
static uint8_t wake_up(void)
{
	uint8_t repeat = 0;
	if(module.sleep == 1)
	{
		HAL_GPIO_WritePin(PON_TRIG_GPIO_Port, PON_TRIG_Pin, GPIO_PIN_SET);
		HAL_Delay(40);
		HAL_GPIO_WritePin(PON_TRIG_GPIO_Port, PON_TRIG_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
	}
	while((send_command("AT\r\n", "OK", DEFAULT_TIMEOUT, NB)) != TRUE)
	{
		if(repeat < MAX_REPEAT)
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
/*
 * @brief	Powers on the module
 * @param 	None
 * @retval	None
 */
static void power_on(void)
{
	HAL_GPIO_WritePin(IoT_PWR_GPIO_Port, IoT_PWR_Pin, GPIO_PIN_SET);
	HAL_Delay(600);
	HAL_GPIO_WritePin(IoT_PWR_GPIO_Port, IoT_PWR_Pin, GPIO_PIN_RESET);
}
/*
 * @brief	Powers off the module
 * @param 	None
 * @retval	None
 */
static void power_off(void)
{
	HAL_GPIO_WritePin(IoT_PWR_GPIO_Port, IoT_PWR_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(IoT_PWR_GPIO_Port, IoT_PWR_Pin, GPIO_PIN_RESET);
}
/*
 * @brief	Clears the RX buffer before new reception
 * @param 	None
 * @retval	None
 */
static void clear_rx_buff(void)
{
	module.rx_index = 0;
	memset(rx_buffer, 0, sizeof(rx_buffer));
}
