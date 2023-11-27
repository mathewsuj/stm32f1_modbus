#include <cstddef>
#include "console.h"

#include "hal.h"
#include "hal_uart.h"
#include "manager.h"

static const std::size_t sensor_telegram_size = 50;
static const int max_usart_ports = 3;

extern UART_HandleTypeDef huart1, huart2, huart3;

uint8_t console_key_input;
uint8_t primary_sensor_buf[sensor_telegram_size];
uint8_t secondary_sensor_buf[sensor_telegram_size];

port_config cfg_debug_port = {debug_port, &huart2, &console_key_input, 1};
port_config cfg_primary_sensor_port{primary_port, &huart3, primary_sensor_buf, sensor_telegram_size};
port_config cfg_secondary_sensor_port{secondary_port, &huart1, secondary_sensor_buf, sensor_telegram_size};

port_config port_list[max_usart_ports] = {cfg_primary_sensor_port, cfg_secondary_sensor_port, cfg_debug_port};

hal_uart::uart_device console(cfg_debug_port);
hal_uart::uart_device primary_sensor(cfg_primary_sensor_port);
hal_uart::uart_device secondary_sensor(cfg_secondary_sensor_port);

void console_putchar(uint8_t msg)
{
    console.send_byte(msg);
}
void sensor_sendbytes(uart_port_id id, const uint8_t msg[])
{
    if (cfg_primary_sensor_port.id == id)
    {
        for (int i = 0; msg[i] != 0; i++)
        {
            primary_sensor.send_byte(msg[i]);
        }
    }
}
void console_rx_init()
{
    console.init();
}
void sensor_init(uart_port_id id, int phy_port)
{
    port_config config{id, (phy_port == 1 ? &huart3 : &huart1), (phy_port == 1 ? primary_sensor_buf : secondary_sensor_buf), sensor_telegram_size};

    if (cfg_primary_sensor_port.id == id)
    {
        primary_sensor.init(config);
    }
    else if (cfg_secondary_sensor_port.id == id)
    {
        secondary_sensor.init(config);
    }
}

extern "C" port_config *get_uart_config(UART_HandleTypeDef *handle)
{
    for (int i = 0; i < max_usart_ports; i++)
    {
        if (handle == port_list[i].handle)
            return &port_list[i];
    }
    return NULL;
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    port_config *port = get_uart_config(huart);
    if (port->id != undefined_port)
    {
        if (port->id == debug_port)
            consoleKeyRxd(*(port->bufptr));
        if (port->id == cfg_primary_sensor_port.id || port->id == cfg_secondary_sensor_port.id)
            sensorRxdData(port->id, port->bufptr, port->rx_buf_size);
        HAL_UART_Receive_IT(port->handle, port->bufptr, port->rx_buf_size);
    }
}