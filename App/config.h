#pragma once

#define MODBUS_ON

#ifdef MODBUS_ON
#define MODBUS_SLAVE
#define modbus_ascii
#define MODBUS_ID_MIN 10
#define MODBUS_ID_MAX 20
#endif

#define baudrate 19200
