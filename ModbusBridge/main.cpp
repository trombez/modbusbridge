/*
Guidelines: http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
*/

#include "src/ModbusRtuPort.h"
#include "src/ModbusTcpPort.h"
#include "src/Models/ModbusSettings.hpp"
#include "include/toml.hpp"
#include <vector>

using namespace std;
using namespace std::chrono;
using namespace ModbusPort;

/*
MODBUS TCP REQUEST FORMAT
[2 Bytes] TRANSACTION ID
[2 Bytes] PROTOCOL (default 0)
[2 Bytes] LENGHT
[1 Byte ] SLAVE ID / UNIT ID
[1 Byte ] Function Code
[2 Bytes] Starting Address
[2 Bytes] Quantity of Registers
*/
int main(void)
{
	int s = -1;
	int nb_points = 65536;
	int rc;
	int const REGISTER_QTA_OFFSET = 4;
	int const REGISTER_START_ADDRESS_OFFSET = 2;
	int const REGISTER_SLAVE_ID_OFFSET = -1;
	int timeout = 10;
	int header_length;
	uint8_t tcpRequest[MODBUS_TCP_MAX_ADU_LENGTH];
	uint16_t* tab_rp_registers = NULL;
	tab_rp_registers = (uint16_t*)malloc(nb_points * sizeof(uint16_t));
	memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));
	modbus_mapping_t* mb_mapping = modbus_mapping_new(0, 1, 65536, 65536);
	ModbusSettings settings;
	vector<ModbusRtuPort> modbusRtuPorts; //Vector auto delete

	//Load Configuration
	settings.load("appsettings.toml");

	//CREATE RTU PORTS
	modbusRtuPorts.reserve(settings.SerialPorts.size());
	for (auto& portSetting : settings.SerialPorts)
	{
		ModbusRtuPort port(portSetting.PortName, portSetting.BaudRate, portSetting.Parity, portSetting.DataBit, portSetting.StopBit);

		int result = port.checkConnection();
		if (result == -1)
		{
			cout << "Connection failed" << endl;
			fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));//TODO: Change with better alternative
			port.close();
			return -1;
		}

		port.setDebugMode(TRUE);
		port.setTimeout(portSetting.TimeoutSeconds, portSetting.TimeoutMilliseconds);
		modbusRtuPorts.push_back(port);
		cout << "Created port: " << portSetting.PortName << endl;
		cout.flush();
	}

	//Create TCP Port
	ModbusTcpPort modbusTcpPort(settings.IpAddress, settings.IpPort);
	modbusTcpPort.setDebugMode(TRUE);
	modbusTcpPort.setTimeout(settings.TimeoutSeconds, settings.TimeoutMilliseconds);
	modbusTcpPort.startServer(1); //TODO: Read number of connections from settings

	int numberOfRegister = 0;
	int startAddress = 0;
	int readResult = 0;

	//MAIN LOOP
	for (;;) {
		rc = modbusTcpPort.waitRequest(tcpRequest); //Read TCP communication
		if (rc == -1 && errno != EMBBADCRC) {
			cout << "Errore TCP" << endl;
			cout.flush();
			break;
		}

		high_resolution_clock::time_point t1 = high_resolution_clock::now();

		header_length = modbusTcpPort.getHeaderLenght();
		int slaveId = tcpRequest[header_length + REGISTER_SLAVE_ID_OFFSET];

		switch (tcpRequest[header_length]) //Modbus Function Code
		{
		case (0x03):
			std::cout << "READ HOLDING REGISTER" << endl;
			numberOfRegister = tcpRequest[header_length + REGISTER_QTA_OFFSET];
			startAddress = tcpRequest[header_length + REGISTER_START_ADDRESS_OFFSET];

			for (auto& portRtu : modbusRtuPorts)
			{
				readResult = portRtu.readHoldingRegisters(slaveId, startAddress, numberOfRegister, tab_rp_registers);
				cout << "Read Result: " << readResult << endl;
				if (readResult == 1)
					break;
			}

			//readResult = modbusRtuPorts[0].readHoldingRegisters(slaveId, startAddress, numberOfRegister, tab_rp_registers);
			//cout << "Read Result: " << readResult << endl;
			//cout << "End Read" << endl;
			for (int i = 0; i < numberOfRegister; i++)
				mb_mapping->tab_registers[startAddress + i] = tab_rp_registers[i];
			break;
		case (0x04):
			std::cout << "READ INPUT REGISTER" << endl;
			numberOfRegister = tcpRequest[header_length + REGISTER_QTA_OFFSET];
			startAddress = tcpRequest[header_length + REGISTER_START_ADDRESS_OFFSET];
			modbusRtuPorts[0].readInputRegisters(slaveId, startAddress, numberOfRegister, tab_rp_registers);
			for (int i = 0; i < numberOfRegister; i++)
				mb_mapping->tab_input_registers[startAddress + i] = tab_rp_registers[i];
			break;
		default:
			break;
		}

		/*if (req[header_length] == 0x03) {
			cout << "HOLDING REGISTER" << endl;
			int numberOfRegister = req[header_length + REGISTER_QTA_OFFSET];
			int startAddress = req[header_length + REGISTER_START_ADDRESS_OFFSET];
			modbusRtuPort->readHoldingRegisters(slaveId, startAddress, numberOfRegister, tab_rp_registers);
			for (int i = 0; i < numberOfRegister; i++)
			{
				mb_mapping->tab_registers[startAddress + i] = tab_rp_registers[i];
			}
		}*/
		rc = modbusTcpPort.sendResponse(tcpRequest, rc, mb_mapping);
		if (rc == -1) {
			break;
		}

		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(t2 - t1).count();
		std::cout << "DURATA: " << duration << endl;
	}

	modbus_mapping_free(mb_mapping);
	modbusTcpPort.closeConnection();
	modbusRtuPorts[0].close();
	return 0;
}
