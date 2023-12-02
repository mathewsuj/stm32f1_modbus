#include <cstdint>
#include <array>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <string>

#include "manager.h"
#include "model.h"
#include "hal.h"
#include "circualrbuf.h"
#include "uart_device.h"
#include "sc400.h"

extern UART_HandleTypeDef huart1, huart2, huart3;

template <typename portId>
struct portData
{
    UART_HandleTypeDef *hnd;
    portData(UART_HandleTypeDef *handler) : hnd(handler) {}
};

portData<uart_primary_sensor> primary_sensor(&huart2);
// portData<uart_secondary_sensor> secondary_sensor;
portData<uart_debug_port> debug_port(&huart3);

osThreadId_t MgrThreadID;

Sc400 sc400;

uart_device<uart_primary_sensor, char> port_primary_sensor(primary_sensor.hnd, 45);
// uart_device<uart_secondary_sensor> port_secondary_sensor;
uart_device<uart_debug_port, char> port_debug(debug_port.hnd, 1);

void managerThread(void *argument)
{
    (void)argument;

    port_primary_sensor.init();
    // port_secondary_sensor.init(secondary_sensor.hnd, 50, 10);
    port_debug.init();

    while (1)
    {
        auto cmd = sc400.makeRequestPacket(302);
        const uint8_t *cmd_array = reinterpret_cast<const uint8_t *>(&cmd[0]);
        if (auto size = cmd.length(); size > 0)
            port_primary_sensor.send_byte(cmd_array, size);
        osDelay(1000);
    }
}
void console_putchar(const uint8_t data)
{
    if (data != '\0')
        port_debug.send_byte(&data, 1);
}
char *console_getcommand()
{
    return port_debug.getString('\r');
}
void initializeManager(osThreadAttr_t thread_attr)
{
    model_init();
    // Create the manager thread
    MgrThreadID = osThreadNew(managerThread, NULL, &thread_attr);
}
extern "C" void sensorRxdData(UART_HandleTypeDef *huart)
{
    if (huart == primary_sensor.hnd)
        port_primary_sensor.datardy();
    if (huart == debug_port.hnd)
        port_debug.datardy();
}