#include "ModbusRtuPort.h"

using namespace ModbusPort;

ModbusRtuPort::ModbusRtuPort(std::string portName, int baudRate, char parity, int dataBit, int stopBit)
{
	const char* port = portName.c_str();
	ctx = modbus_new_rtu(port, baudRate, parity, dataBit, stopBit);
}

int ModbusRtuPort::checkConnection()
{
	if (ctx == NULL) {
		fprintf(stderr, "Unable to allocate libmodbus context\n");
		return -1;
	}

	if (ctx && modbus_connect(ctx) == -1)
	{
		fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}
}

void ModbusRtuPort::setDebugMode(int value)
{
	modbus_set_debug(ctx, TRUE);
}

void ModbusRtuPort::setTimeout(int seconds, int milliSeconds)
{
	modbus_set_response_timeout(ctx, seconds, milliSeconds * 1000);
}

void ModbusRtuPort::close()
{
	modbus_close(ctx);
	modbus_free(ctx);
}

int ModbusRtuPort::readHoldingRegisters(int slaveId, int startAddress, int numberOfRegister, uint16_t* registers)
{
	modbus_set_slave(ctx, slaveId);
	int result = modbus_read_registers(ctx, startAddress, numberOfRegister, registers);
	return result;
}

int ModbusRtuPort::readInputRegisters(int slaveId, int startAddress, int numberOfRegister, uint16_t* registers)
{
	modbus_set_slave(ctx, slaveId);
	int result = modbus_read_input_registers(ctx, startAddress, numberOfRegister, registers);
	return result;
}