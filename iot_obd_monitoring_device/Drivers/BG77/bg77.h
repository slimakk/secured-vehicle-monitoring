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
#define NB_TIMER &htim2
#define UART_TIMER &htim7
#define COMMAND_SIZE 255
#define DEFAULT_TIMEOUT 1000
#define MAX_REPEAT 10


typedef struct{
	double latitude;
	char* lat_ort;
	double longitude;
	char* long_ort;
	double altitude;
	double speed;
	uint16_t time;
	uint16_t date;
}location;

typedef struct {
	uint8_t rx_index;
	uint8_t timeout;
	uint8_t connected;
	uint8_t initialized;
	uint8_t received;
	uint8_t sleep;
	uint8_t status;
	uint8_t error;
	uint8_t rssi;
	uint8_t mqtt_status;
	location pos;
}BG77;

uint8_t module_init(BG77 *module);
uint8_t send_command(char *command, char *reply, uint16_t timeout, UART_HandleTypeDef *interface);
uint8_t check_status(BG77 *module);
uint8_t check_signal(void);
uint8_t set_psm(const char* tau, const char* active_time, uint8_t mode);
void nb_rx_callback(void);

uint8_t mqtt_open(const char* broker_address, uint16_t port, uint8_t id);
uint8_t mqtt_connect(uint8_t id, const char* client_id, BG77 *module);
uint8_t mqtt_disconnect(uint8_t id);
uint8_t mqtt_close(uint8_t id, BG77 module);
uint8_t mqtt_subscribe(uint8_t id, uint8_t msg_id, const char *topic, uint8_t qos);
uint8_t mqtt_unsubscribe(uint8_t id, uint8_t msg_id, const char *topic);
uint8_t mqtt_publish(uint8_t id, uint8_t msg_id, uint8_t qos, uint8_t retain, const char *topic, const char *msg);

uint8_t acquire_position(BG77 module);
void parse_location(BG77 module);

#endif /* BG77_BG77_H_ */
