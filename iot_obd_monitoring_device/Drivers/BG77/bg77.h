/*
 * bg77.h
 *
 *  Created on: Mar 27, 2023
 *      Author: miros
 */

#ifndef BG77_BG77_H_
#define BG77_BG77_H_

#include "main.h"
#include "stm32l4xx_hal.h"


#define BAUD 115200
#define NB &huart2
#define GPS &huart3

typedef struct {
	uint8_t initialized;
}BG77;

uint8_t module_init(BG77 module);
uint8_t send_command(char *command, char *reply, uint16_t timeout, UART_HandleTypeDef *interface);
void nb_rx_callback(void);
void gps_rx_callback(void);

#endif /* BG77_BG77_H_ */
