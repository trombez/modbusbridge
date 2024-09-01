#ifndef MODBUSSERIALSETTINGS_HPP
#define MODBUSSERIALSETTINGS_HPP

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

namespace ModbusPort {
	class ModbusSerialSettings;
}

using namespace std;

class ModbusPort::ModbusSerialSettings
{
public:
	std::string PortName;
	int BaudRate;
	char Parity;
	int DataBit;
	int StopBit;
	int TimeoutSeconds;
	int TimeoutMilliseconds;
};

#endif
