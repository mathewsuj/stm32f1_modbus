#include <cstdint>
#include <array>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <string>

#include "cmsis_os.h"

#include "manager.h"
#include "model.h"
#include "console.h"
#include "uart_device.h"
#include "protocolbase.h"

#define debug
#ifdef debug
#include "task.h"
#endif

extern UART_HandleTypeDef huart1, huart2, huart3;
// uart port configuration
portData<uart_primary_sensor> primary_sensor(&huart2);
// portData<uart_secondary_sensor> secondary_sensor(&huart4);
portData<uart_debug_port> debug_port(&huart3);
portData<uart_PC> PC_port(&huart1);

UartDevice<ProtocolBase::ProtocolId::SC400, char> port_primary_sensor(primary_sensor.hnd, 45);
// uart_device<uart_secondary_sensor> port_secondary_sensor;
UartDevice<ProtocolBase::ProtocolId::NONE, char> port_debug(debug_port.hnd, 1);
UartDevice<ProtocolBase::ProtocolId::SC400, char> port_PC(PC_port.hnd, 20);
//-------------------------------------------------------------------------------

SensorData sensor_data;

osThreadId_t MgrThreadID;

template <typename T>
void processData(UartDevice<ProtocolBase::ProtocolId::SC400, char> &port, char delimiter);

template <>
void processData<uart_primary_sensor>(UartDevice<ProtocolBase::ProtocolId::SC400, char> &port, const char delimiter)
{
    port.SendRequestPacket(302);
    osDelay(500);
    if (const char *inp = port.GetString(delimiter); inp)
    {
        const char crc = port.Read();
        if (*inp && port.CheckCrc(inp, crc))
        {
            logMessage("New packet from gauge received\n\r", true);
            Configurations newConfig;
            port.UpdateModel(inp, newConfig);
            sensor_data.updateData(newConfig);
        }
    }
}

template <>
void processData<uart_PC>(UartDevice<ProtocolBase::ProtocolId::SC400, char> &port, const char delimiter)
{
    if (const char *inp = port.GetString(delimiter); inp)
    {
        const char crc = port.Read();
        if (*inp && port.CheckCrc(inp, crc))
        {
            auto id = atoi(inp + 1);
            logMessage("New packet from PC received\n\r", true);
            char buf[100];
            sensor_data.GetValues<sc400>(buf);
            port.SendResponsePacket(id, buf);
        }
    }
}

void managerThread(void *argument)
{
    (void)argument;

    port_primary_sensor.Init(sensor_data.getPortSettings<uart_primary_sensor>());

    // port_secondary_sensor.init(secondary_sensor.hnd, 50, 10);
    port_debug.Init(sensor_data.getPortSettings<uart_secondary_sensor>());

    port_PC.Init(sensor_data.getPortSettings<uart_PC>());
    osDelay(100);

#ifdef debug
    UBaseType_t uxHighWaterMark, minWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    minWaterMark = uxHighWaterMark;
    const char *taskName = osThreadGetName(osThreadGetId());
    debugLog("HighWaterMark - %s: %ld\r\n", taskName, minWaterMark);
#endif

    while (1)
    {
        // get data from gauge
        processData<uart_primary_sensor>(port_primary_sensor, Sc400::ETX);

        // response to query from PC
        processData<uart_PC>(port_PC, 0x3);

#ifdef debug
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        if (minWaterMark > uxHighWaterMark)
        {
            minWaterMark = uxHighWaterMark;
            debugLog("HighWaterMark - %s: %ld\r\n", taskName, minWaterMark);
        }
#endif
    }
}

void uart_setPortConfiguration(const uint8_t port, const Communication::SerialPort &setting)
{
    switch (port)
    {
    case 1:
        sensor_data.setPortSettings<uart_primary_sensor>(setting);
        break;
    case 2:
        sensor_data.setPortSettings<uart_secondary_sensor>(setting);
        break;
    case 3:
        sensor_data.setPortSettings<uart_PC>(setting);
        break;
    default:
        // Handle invalid port number (optional)
        break;
    }
}

void console_putchar(const char data)
{
    if (data != '\0')
        port_debug.SendByte(&data, 1);
}

void console_putstr(const char *data, const std::size_t size)
{
    if ((size > 0) && (*data != '\0'))
        port_debug.SendByte(data, size);
}

char *console_getcommand()
{
    return port_debug.GetString('\r');
}

void console_dumpmodel()
{
    sensor_data.dumpModel();
}

void console_dumpports()
{
    sensor_data.dumpPorts();
}

void initializeManager()
{
    osThreadAttr_t thread_attr_task;
    thread_attr_task.name = "managerTask";
    thread_attr_task.stack_size = 128 * 9;
    thread_attr_task.priority = (osPriority_t)osPriorityNormal;
    // Create the manager thread
    MgrThreadID = osThreadNew(managerThread, NULL, &thread_attr_task);
}

extern "C" void sensorRxdData(const UART_HandleTypeDef *huart)
{
    if (huart == primary_sensor.hnd)
        port_primary_sensor.DataRdy();
    if (huart == debug_port.hnd)
        port_debug.DataRdy();
    if (huart == PC_port.hnd)
        port_PC.DataRdy();
}