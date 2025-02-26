# SECURED VEHICLE MONITORING USING OBDII AND LPWAN NETWORKS

This repository contains hardware design files and source code for bachelor thesis at FEEC BUT, which was sucessfully defended in 2023. The main goal behind the creation of this thesis was to create a device that would be capable of reporting important engine parameters to an external server for further processing.

[Link to a full thesis](https://www.vut.cz/studenti/zav-prace?zp_id=151037)
---------------------------------------------------------------------------
## Hardware specs
* MCU -- STM32 L431
* LPWAN module -- Quectel BG77, supports NB-IoT, LTE Cat M, GPS
* Power -- Synchronous Step-Down Converter by Texas Instruments
* Vehicle comms -- CAN Bus, K-Line, KWP2000
## Software
* OBD II support -- only mode 02
* Data transmission -- UDP, TCP, MQTT
