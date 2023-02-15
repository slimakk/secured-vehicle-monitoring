/*
 * OBD_PID.c
 *
 *  Created on: Dec 1, 2022
 *      Author: miros
 */
#include "OBD_PID.h"

static const uint8_t PID_list[90] = {
		0x5, 0x4, 0x42, 0x7, 0x8, 0x9, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1F,
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B,
		0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
		0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x06, 0x43,
		0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
		0x4F, 0x52, 0x53, 0x54, 0x55,0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B,
		0x5C, 0x5D, 0x5E, 0x61, 0x62, 0x63, 0x64, 0x66, 0x67, 0x68, 0x7C,
		0xA6
};

const char *pid_names[90] = {
		"Engine Temp",
		"Engine Load",
		"Voltage",
		"Long-term Trim 1",
		"Short-term Trim 2",
		"Long-term Trim 2",
		"Fuel Pressure",
		"Intake abs Pressure",
		"Engine RPM",
		"Speed",
		"Timing Advance",
		"Intake Temp",
		"MAF Flow Rate",
		"Throttle Position",
		"Lambda 1",
		"Lambda 2",
		"Lambda 3",
		"Lambda 4",
		"Lambda 5",
		"Lambda 6",
		"Lambda 7",
		"Lambda 8",
		"Engine run time",
		"Distance with fault",
		"Fuel Rail Pressure",
		"Fuel Rail Gauge Pressure",
		"Lambda 1 Ratio",
		"Lambda 2 Ratio",
		"Lambda 3 Ratio",
		"Lambda 4 Ratio",
		"Lambda 5 Ratio",
		"Lambda 6 Ratio",
		"Lambda 7 Ratio",
		"Lambda 8 Ratio",
		"Commanded EGR",
		"EGR Error",
		"Commanded Evap",
		"Fuel Level",
		"Warm-ups",
		"Distance since clearing",
		"Evap. Pressure",
		"Abs Pressure",
		"Lambda 1 Ratio",
		"Lambda 2 Ratio",
		"Lambda 3 Ratio",
		"Lambda 4 Ratio",
		"Lambda 5 Ratio",
		"Lambda 6 Ratio",
		"Lambda 7 Ratio",
		"Lambda 8 Ratio",
		"Cat Temp Bank 1",
		"Cat Temp Bank 2",
		"Cat Temp Bank 1",
		"Cat Temp Bank 2",
		"Short-term Trim 1",
		"Absolute Load",
		"Commanded Fuel-Air Ratio",
		"Relative Throttle",
		"Absolute Throttle B",
		"Absolute Throttle C",
		"Absolute Throttle D",
		"Absolute Throttle E",
		"Absolute Throttle F",
		"Commanded Throttle",
		"Run Time with Fault",
		"Time since clearing",
		"Max Fuel-Air Ratio",
		"Ethanol %",
		"Absolute Evap Pressure",
		"Evap Pressure",
		"Short-term Lambda Trim Bank 1",
		"Long-term Lambda Trim Bank 1",
		"Short-term Lambda Trim Bank 2",
		"Long-term Lambda Trim Bank 2",
		"Abs Fuel Rail Pressure",
		"Relative Throttle Pedal Position",
		"Hybrid Battery Pack - Life",
		"Engine Oil Temp",
		"Injection Timing",
		"Fuel Consumption",
		"Demanded Torque",
		"Engine Torque",
		"Engine Torque - %",
		"MAF",
		"Engine Coolant Temp",
		"Intake Air Temp",
		"DPF Temp",
		"Odometer"
};

static const uint8_t PID_LUT[256][1] = {
		[0x0]  = { 4 },
		[0x4]  = { 2 }, //Calculated engine load
		[0x5]  = { 2 }, //Engine coolant temperature
		[0x6]  = { 2 }, //Short term fuel trim—Bank 1
		[0x7]  = { 2 }, //Long term fuel trim—Bank 1
		[0x8]  = { 2 }, //Short term fuel trim—Bank 2
		[0x9]  = { 2 }, //Long term fuel trim—Bank 2
		[0x0A] = { 2 }, //Fuel pressure (gauge pressure)
		[0x0B] = { 2 }, //Intake manifold absolute pressure
		[0x0C] = { 3 }, //Engine RPM
		[0x0D] = { 2 }, //Vehicle speed
		[0x0E] = { 2 }, //Timing advance
		[0x0F] = { 2 }, //Intake air temperature
		[0x10] = { 3 }, //MAF air flow rate
		[0x11] = { 2 }, //Throttle position
		[0x14] = { 3 }, //"Oxygen Sensor 1 A: Voltage B: Short term fuel trim"
		[0x15] = { 3 }, //"Oxygen Sensor 2 A: Voltage B: Short term fuel trim"
		[0x16] = { 3 }, //"Oxygen Sensor 3 A: Voltage B: Short term fuel trim"
		[0x17] = { 3 }, //"Oxygen Sensor 4 A: Voltage B: Short term fuel trim"
		[0x18] = { 3 }, //"Oxygen Sensor 5 A: Voltage B: Short term fuel trim"
		[0x19] = { 3 }, //"Oxygen Sensor 6 A: Voltage B: Short term fuel trim"
		[0x1A] = { 3 }, //"Oxygen Sensor 7 A: Voltage B: Short term fuel trim"
		[0x1B] = { 3 }, //"Oxygen Sensor 8 A: Voltage B: Short term fuel trim"
		[0x1F] = { 3 }, //Run time since engine start
		[0x20] = { 5 },
		[0x21] = { 3 }, //Distance traveled with malfunction indicator lamp (MIL) on
		[0x22] = { 3 }, //Fuel Rail Pressure (relative to manifold vacuum)
		[0x23] = { 3 }, //Fuel Rail Gauge Pressure (diesel, or gasoline direct injection)
		[0x24] = { 5 }, //"Oxygen Sensor 1 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x25] = { 5 }, //"Oxygen Sensor 2 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x26] = { 5 }, //"Oxygen Sensor 3 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x27] = { 5 }, //"Oxygen Sensor 4 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x28] = { 5 }, //"Oxygen Sensor 5 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x29] = { 5 }, //"Oxygen Sensor 6 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x2A] = { 5 }, //"Oxygen Sensor 7 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x2B] = { 5 }, //"Oxygen Sensor 8 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x2C] = { 2 }, //Commanded EGR
		[0x2D] = { 2 }, //EGR Error
		[0x2E] = { 2 }, //Commanded evaporative purge
		[0x2F] = { 2 }, //Fuel Tank Level Input
		[0x30] = { 2 }, //Warm-ups since codes cleared
		[0x31] = { 3 }, //Distance traveled since codes cleared
		[0x32] = { 3 }, //Evap. System Vapor Pressure
		[0x33] = { 2 }, //Absolute Barometric Pressure
		[0x34] = { 5 }, //"Oxygen Sensor 1 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x35] = { 5 }, //"Oxygen Sensor 2 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x36] = { 5 }, //"Oxygen Sensor 3 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x37] = { 5 }, //"Oxygen Sensor 4 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x38] = { 5 }, //"Oxygen Sensor 5 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x39] = { 5 }, //"Oxygen Sensor 6 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x3A] = { 5 }, //"Oxygen Sensor 7 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x3B] = { 5 }, //"Oxygen Sensor 8 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x3C] = { 3 }, //Catalyst Temperature: Bank 1, Sensor 1
		[0x3D] = { 3 }, //Catalyst Temperature: Bank 2, Sensor 1
		[0x3E] = { 3 }, //Catalyst Temperature: Bank 1, Sensor 2
		[0x3F] = { 3 }, //Catalyst Temperature: Bank 2, Sensor 2
		[0x40] = { 5 },
		[0x42] = { 3 }, //Control module voltage
		[0x43] = { 3 }, //Absolute load value
		[0x44] = { 3 }, //Fuel–Air commanded equivalence ratio
		[0x45] = { 2 }, //Relative throttle position
		[0x46] = { 2 }, //Ambient air temperature
		[0x47] = { 2 }, //Absolute throttle position B
		[0x48] = { 2 }, //Absolute throttle position C
		[0x49] = { 2 }, //Accelerator pedal position D
		[0x4A] = { 2 }, //Accelerator pedal position E
		[0x4B] = { 2 }, //Accelerator pedal position F
		[0x4C] = { 2 }, //Commanded throttle actuator
		[0x4D] = { 3 }, //Time run with MIL on
		[0x4E] = { 3 }, //Time since trouble codes cleared
		[0x4F] = { 2 }, //Maximum value for Fuel–Air equivalence ratio, oxygen sensor voltage, oxygen sensor current, and intake manifold absolute pressure
		[0x52] = { 2 }, //Ethanol fuel %
		[0x53] = { 3 }, //Absolute Evap system Vapor Pressure
		[0x54] = { 3 }, //Evap system vapor pressure
		[0x55] = { 2 }, //Short term secondary oxygen sensor trim, A: bank 1, B: bank 3
		[0x56] = { 2 }, //Long term secondary oxygen sensor trim, A: bank 1, B: bank 3
		[0x57] = { 2 }, //Short term secondary oxygen sensor trim, A: bank 2, B: bank 4
		[0x58] = { 2 }, //Long term secondary oxygen sensor trim, A: bank 2, B: bank 4
		[0x59] = { 3 }, //Fuel rail absolute pressure
		[0x5A] = { 2 }, //Relative accelerator pedal position
		[0x5B] = { 2 }, //Hybrid battery pack remaining life
		[0x5C] = { 2 }, //Engine oil temperature
		[0x5D] = { 2 }, //Fuel injection timing
		[0x5E] = { 3 }, //Engine fuel rate
		[0x61] = { 2 }, //Driver's demand engine - percent torque
		[0x62] = { 2 }, //Actual engine - percent torque
		[0x63] = { 3 }, //Engine reference torque
		[0x64] = { 2 }, //Engine percent torque data
		[0x66] = { 3 }, //Mass air flow sensor
		[0x67] = { 2 }, //Engine coolant temperature
		[0x68] = { 2 }, //Intake air temperature sensor
		[0x7C] = { 3 }, //Diesel Particulate filter (DPF) temperature
		[0xA6] = { 5 }, //Odometer
};

uint8_t PID_Get_Lenght(uint8_t pid)
{
	return PID_LUT[pid][0];
}

uint8_t Get_PID(uint8_t index)
{
	return PID_list[index];
}
