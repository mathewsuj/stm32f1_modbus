#include <cstdint>
#include <array>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <string>

#include "manager.h"
#include "model.h"
#include "console.h"

#include "uart_device.h"
#include "protocolbase.h"
// #define debug
#ifdef debug
#include "task.h"
#endif

extern UART_HandleTypeDef huart1, huart2, huart3;
// uart port configuration
portData<uart_primary_sensor> primary_sensor(&huart2);
// portData<uart_secondary_sensor> secondary_sensor(&huart4);
portData<uart_debug_port> debug_port(&huart3);
portData<uart_PC> PC_port(&huart1);

uart_device<ProtocolBase::SC400, char> port_primary_sensor(primary_sensor.hnd, 45);
// uart_device<uart_secondary_sensor> port_secondary_sensor;
uart_device<ProtocolBase::NONE, char> port_debug(debug_port.hnd, 1);
uart_device<ProtocolBase::SC400, char> port_PC(PC_port.hnd, 20);
//-------------------------------------------------------------------------------

SensorData sensor_data;

osThreadId_t MgrThreadID;

void managerThread(void *argument)
{
    (void)argument;

    port_primary_sensor.Init();

    // port_secondary_sensor.init(secondary_sensor.hnd, 50, 10);
    port_debug.Init();

    port_PC.Init();

#ifdef debug
    UBaseType_t uxHighWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
#endif

    while (1)
    {
        // get data from gauge
        port_primary_sensor.SendRequestPacket(302);
        osDelay(500);
        if (const char *inp = port_primary_sensor.GetString(0x3); inp)
        {
            const char crc = port_primary_sensor.Read();

            if (*inp)
                if (port_primary_sensor.CheckCrc(inp, crc))
                {
                    logMessage("new packet from gauge received\n\r", true);

                    Configurations newConfig;
                    port_primary_sensor.UpdateModel(inp, newConfig);
                    sensor_data.updateData(newConfig);
                }
        }
        if (const char *inp = port_PC.GetString(0x3); inp)
        {
            const char crc = port_PC.Read();

            if (*inp)
                if (port_PC.CheckCrc(inp, crc))
                {
                    auto id = atoi(inp + 1);
                    logMessage("new packet from PC received\n\r", true);
                    char buf[100];
                    sensor_data.GetValues(302, buf);
                    port_PC.SendResponsePacket(id, buf);
                }
        }
#ifdef debug
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}
void console_putchar(const uint8_t data)
{
    if (data != '\0')
        port_debug.SendByte(&data, 1);
}
void console_putstr(const uint8_t *data, std::size_t size)
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
void initializeManager(osThreadAttr_t thread_attr)
{
    // Create the manager thread
    MgrThreadID = osThreadNew(managerThread, NULL, &thread_attr);
}
extern "C" void sensorRxdData(UART_HandleTypeDef *huart)
{
    if (huart == primary_sensor.hnd)
        port_primary_sensor.DataRdy();
    if (huart == debug_port.hnd)
        port_debug.DataRdy();
    if (huart == PC_port.hnd)
        port_PC.DataRdy();
}