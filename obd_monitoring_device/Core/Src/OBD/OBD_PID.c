/*
 * OBD_PID.c
 *
 *  Created on: Dec 1, 2022
 *      Author: miros
 */
#include "OBD_PID.h"

static const uint8_t PID_LUT[256][1] = {
		[0x4]  = { 1 }, //Calculated engine load
		[0x5]  = { 2 }, //Engine coolant temperature
		[0x6]  = { 1 }, //Short term fuel trim—Bank 1
		[0x7]  = { 1 }, //Long term fuel trim—Bank 1
		[0x8]  = { 1 }, //Short term fuel trim—Bank 2
		[0x9]  = { 1 }, //Long term fuel trim—Bank 2
		[0x0A] = { 1 }, //Fuel pressure (gauge pressure)
		[0x0B] = { 1 }, //Intake manifold absolute pressure
		[0x0C] = { 2 }, //Engine RPM
		[0x0D] = { 1 }, //Vehicle speed
		[0x0E] = { 1 }, //Timing advance
		[0x0F] = { 1 }, //Intake air temperature
		[0x10] = { 2 }, //MAF air flow rate
		[0x11] = { 1 }, //Throttle position
		[0x14] = { 2 }, //"Oxygen Sensor 1 A: Voltage B: Short term fuel trim"
		[0x15] = { 2 }, //"Oxygen Sensor 2 A: Voltage B: Short term fuel trim"
		[0x16] = { 2 }, //"Oxygen Sensor 3 A: Voltage B: Short term fuel trim"
		[0x17] = { 2 }, //"Oxygen Sensor 4 A: Voltage B: Short term fuel trim"
		[0x18] = { 2 }, //"Oxygen Sensor 5 A: Voltage B: Short term fuel trim"
		[0x19] = { 2 }, //"Oxygen Sensor 6 A: Voltage B: Short term fuel trim"
		[0x1A] = { 2 }, //"Oxygen Sensor 7 A: Voltage B: Short term fuel trim"
		[0x1B] = { 2 }, //"Oxygen Sensor 8 A: Voltage B: Short term fuel trim"
		[0x1F] = { 2 }, //Run time since engine start
		[0x21] = { 2 }, //Distance traveled with malfunction indicator lamp (MIL) on
		[0x22] = { 2 }, //Fuel Rail Pressure (relative to manifold vacuum)
		[0x23] = { 2 }, //Fuel Rail Gauge Pressure (diesel, or gasoline direct injection)
		[0x24] = { 4 }, //"Oxygen Sensor 1 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x25] = { 4 }, //"Oxygen Sensor 2 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x26] = { 4 }, //"Oxygen Sensor 3 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x27] = { 4 }, //"Oxygen Sensor 4 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x28] = { 4 }, //"Oxygen Sensor 5 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x29] = { 4 }, //"Oxygen Sensor 6 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x2A] = { 4 }, //"Oxygen Sensor 7 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x2B] = { 4 }, //"Oxygen Sensor 8 AB: Fuel–Air Equivalence Ratio CD: Voltage"
		[0x2C] = { 1 }, //Commanded EGR
		[0x2D] = { 1 }, //EGR Error
		[0x2E] = { 1 }, //Commanded evaporative purge
		[0x2F] = { 1 }, //Fuel Tank Level Input
		[0x30] = { 1 }, //Warm-ups since codes cleared
		[0x31] = { 2 }, //Distance traveled since codes cleared
		[0x32] = { 2 }, //Evap. System Vapor Pressure
		[0x33] = { 1 }, //Absolute Barometric Pressure
		[0x34] = { 4 }, //"Oxygen Sensor 1 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x35] = { 4 }, //"Oxygen Sensor 2 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x36] = { 4 }, //"Oxygen Sensor 3 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x37] = { 4 }, //"Oxygen Sensor 4 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x38] = { 4 }, //"Oxygen Sensor 5 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x39] = { 4 }, //"Oxygen Sensor 6 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x3A] = { 4 }, //"Oxygen Sensor 7 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x3B] = { 4 }, //"Oxygen Sensor 8 AB: Fuel–Air Equivalence Ratio CD: Current"
		[0x3C] = { 2 }, //Catalyst Temperature: Bank 1, Sensor 1
		[0x3D] = { 2 }, //Catalyst Temperature: Bank 2, Sensor 1
		[0x3E] = { 2 }, //Catalyst Temperature: Bank 1, Sensor 2
		[0x3F] = { 2 }, //Catalyst Temperature: Bank 2, Sensor 2
		[0x42] = { 2 }, //Control module voltage
		[0x43] = { 2 }, //Absolute load value
		[0x44] = { 2 }, //Fuel–Air commanded equivalence ratio
		[0x45] = { 1 }, //Relative throttle position
		[0x46] = { 1 }, //Ambient air temperature
		[0x47] = { 1 }, //Absolute throttle position B
		[0x48] = { 1 }, //Absolute throttle position C
		[0x49] = { 1 }, //Accelerator pedal position D
		[0x4A] = { 1 }, //Accelerator pedal position E
		[0x4B] = { 1 }, //Accelerator pedal position F
		[0x4C] = { 1 }, //Commanded throttle actuator
		[0x4D] = { 2 }, //Time run with MIL on
		[0x4E] = { 2 }, //Time since trouble codes cleared
		[0x4F] = { 1 }, //Maximum value for Fuel–Air equivalence ratio, oxygen sensor voltage, oxygen sensor current, and intake manifold absolute pressure
		[0x52] = { 1 }, //Ethanol fuel %
		[0x53] = { 2 }, //Absolute Evap system Vapor Pressure
		[0x54] = { 2 }, //Evap system vapor pressure
		[0x55] = { 1 }, //Short term secondary oxygen sensor trim, A: bank 1, B: bank 3
		[0x56] = { 1 }, //Long term secondary oxygen sensor trim, A: bank 1, B: bank 3
		[0x57] = { 1 }, //Short term secondary oxygen sensor trim, A: bank 2, B: bank 4
		[0x58] = { 1 }, //Long term secondary oxygen sensor trim, A: bank 2, B: bank 4
		[0x59] = { 2 }, //Fuel rail absolute pressure
		[0x5A] = { 1 }, //Relative accelerator pedal position
		[0x5B] = { 1 }, //Hybrid battery pack remaining life
		[0x5C] = { 1 }, //Engine oil temperature
		[0x5D] = { 1 }, //Fuel injection timing
		[0x5E] = { 2 }, //Engine fuel rate
		[0x61] = { 1 }, //Driver's demand engine - percent torque
		[0x62] = { 1 }, //Actual engine - percent torque
		[0x63] = { 2 }, //Engine reference torque
		[0x64] = { 1 }, //Engine percent torque data
		[0x66] = { 2 }, //Mass air flow sensor
		[0x67] = { 1 }, //Engine coolant temperature
		[0x68] = { 1 }, //Intake air temperature sensor
		[0x7C] = { 2 }, //Diesel Particulate filter (DPF) temperature
		[0xA6] = { 4 }, //Odometer
};

uint8_t PID_Get_Lenght(uint8_t pid)
{
	return PID_LUT[pid][0];
}


