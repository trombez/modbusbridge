#ifndef MODBUSSETTINGS_HPP
#define MODBUSSETTINGS_HPP

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "ModbusSerialSettings.hpp"
#include <vector>

namespace ModbusPort {
	class ModbusSettings;
}

using namespace std;

enum class CacheType : int { Static = 0, Dynamic = 1, Hybrid = 2 };

class ModbusPort::ModbusSettings
{
public:
	CacheType SlaveIdCacheType;
	string IpAddress;
	int IpPort;
	int TimeoutSeconds;
	int TimeoutMilliseconds;
	vector<ModbusSerialSettings> SerialPorts;
	bool load(string path);
};
#endif

