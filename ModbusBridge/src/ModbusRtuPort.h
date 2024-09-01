#ifndef MODBUSRTUPORT_H
#define MODBUSRTUPORT_H

#include "modbus-rtu.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <iostream>
#include <chrono>

namespace ModbusPort {
	class ModbusRtuPort;
}

class ModbusPort::ModbusRtuPort
{
protected:
	modbus_t* ctx;
	int nb_points = 65536;

public:
	ModbusRtuPort(std::string portName, int baudRate, char parity, int dataBit, int stopBit);

	int checkConnection();
	void setDebugMode(int value);
	void setTimeout(int seconds, int milliSeconds);
	int readHoldingRegisters(int slaveId, int startAddress, int numberOfRegister, uint16_t* registers);
	int readInputRegisters(int slaveId, int startAddress, int numberOfRegister, uint16_t* registers);
	void close();
};

#endif