#ifndef MODBUSTCPPORT_H
#define MODBUSTCPPORT_H

#include "modbus-tcp.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <iostream>
#include <chrono>

namespace ModbusPort {
	class ModbusTcpPort;
}

using namespace std;

class ModbusPort::ModbusTcpPort
{
protected:
	modbus_t* tcpServer;
	int nb_points = 65536;
	int socket = -1;

public:
	ModbusTcpPort(std::string ipAddress, int port);

	int startServer(int numberOfConnections);
	void setDebugMode(int value);
	void setTimeout(int seconds, int milliSeconds);
	int waitRequest(uint8_t* request);
	int getHeaderLenght();
	int sendResponse(const uint8_t* req, int req_length, modbus_mapping_t* mb_mapping);
	void closeConnection();
};

#endif

