#include "ModbusTcpPort.h"

using namespace ModbusPort;
using namespace std;

ModbusTcpPort::ModbusTcpPort(std::string ipAddress, int port)
{
	const char* ip = ipAddress.c_str();
	tcpServer = modbus_new_tcp(ip, port);

	if (tcpServer == NULL) {
		cout << "Unable to allocate libmodbus context\n" << endl;
	}

	/*struct timeval response_timeout;
	response_timeout.tv_sec = 1;
	response_timeout.tv_usec = 0;*/
	//modbus_set_response_timeout(tcpServer, 1, 0);
}

void ModbusTcpPort::setDebugMode(int value)
{
	modbus_set_debug(tcpServer, TRUE);
}

int ModbusTcpPort::startServer(int numberOfConnections)
{
	socket = modbus_tcp_listen(tcpServer, numberOfConnections);
	int s = modbus_tcp_accept(tcpServer, &socket);
	return s;
}

void ModbusTcpPort::setTimeout(int seconds, int milliSeconds)
{
	modbus_set_response_timeout(tcpServer, seconds, milliSeconds * 1000);
}

int ModbusTcpPort::waitRequest(uint8_t* request)
{
	return modbus_receive(tcpServer, request);
}

int ModbusTcpPort::getHeaderLenght()
{
	return modbus_get_header_length(tcpServer);
}

int ModbusTcpPort::sendResponse(const uint8_t* request, int req_length, modbus_mapping_t* mb_mapping)
{
	return modbus_reply(tcpServer, request, req_length, mb_mapping);
}

void ModbusTcpPort::closeConnection()
{
	if (socket != -1) {
		close(socket);
	}
}
