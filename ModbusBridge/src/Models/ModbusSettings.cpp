#include "ModbusSettings.hpp"
#include "../../include/toml.hpp"

using namespace ModbusPort;
using namespace std;

bool ModbusSettings::load(std::string path)
{
	const auto data = toml::parse(path);
	this->IpAddress = toml::find<std::string>(data, "IpAddress");
	this->IpPort = toml::find<int>(data, "IpPort");
	this->TimeoutSeconds = toml::find<int>(data, "TimeoutSeconds");
	this->TimeoutMilliseconds = toml::find<int>(data, "TimeoutMilliseconds");
	this->SlaveIdCacheType = static_cast<CacheType>(toml::find<int>(data,"SlaveIdCacheType"));

	const auto modbusSerials = toml::find<std::vector<toml::table>>(data, "ModbusSerialSettings");
	this->SerialPorts.reserve(modbusSerials.size());

	for (toml::table portSetting : modbusSerials)
	{
		ModbusSerialSettings port;
		const auto portname = toml::get<std::string>(portSetting["PortName"]);
		port.PortName = toml::get<std::string>(portSetting["PortName"]);
		port.BaudRate = toml::get<int>(portSetting["BaudRate"]);
		port.Parity = toml::get<std::string>(portSetting["Parity"]).at(0);
		port.DataBit = toml::get<int>(portSetting["DataBit"]);
		port.StopBit = toml::get<int>(portSetting["StopBit"]);
		port.TimeoutSeconds = toml::get<int>(portSetting["TimeoutSeconds"]);
		port.TimeoutMilliseconds = toml::get<int>(portSetting["TimeoutMilliseconds"]);

		this->SerialPorts.push_back(port);
	}
}
