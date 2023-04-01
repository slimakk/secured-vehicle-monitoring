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
#include <string.h>

uint8_t rx_data = 0;
uint8_t rx_buff[1500] = {0};
uint8_t rx_index = 0;

static uint8_t wake_up(void);
static void clear_rx_buff(void);
static void power_on(void);
//TODO
uint8_t module_init(BG77 module)
{
	power_on();
	send_command("AT\r\n", "OK\r\n", 1000, NB);
	send_command("ATE0\r\n", "OK\r\n", 1000, NB);
	return 1;
}

uint8_t send_command(char *command, char *reply, uint16_t timeout, UART_HandleTypeDef *interface)
{
	wake_up();
	HAL_UART_Transmit_DMA(interface, (unsigned char *)command, (uint16_t)strlen(command));
	HAL_Delay(timeout);
	HAL_UART_Receive_IT(interface, &rx_data, 1);
	if(strstr((char *)rx_buff, reply) != NULL)
	{
		clear_rx_buff();
		return TRUE;
	}
	clear_rx_buff();
	return FALSE;

}

void nb_rx_callback(void)
{
	rx_buff[rx_index++] = rx_data;
	HAL_UART_Receive_IT(NB, &rx_data, 1);
}
//TODO
void gps_rx_callback(void)
{

}

//TODO
static uint8_t wake_up(void)
{
	HAL_GPIO_WritePin(PON_TRIG_GPIO_Port, PON_TRIG_Pin, GPIO_PIN_SET);
	HAL_Delay(40);
	HAL_GPIO_WritePin(PON_TRIG_GPIO_Port, PON_TRIG_Pin, GPIO_PIN_RESET);
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

static void clear_rx_buff(void)
{
	rx_index = 0;
	memset(rx_buff, 0, sizeof(rx_buff));
}
