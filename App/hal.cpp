
#include "console.h"
#include "main.h"
#include "hal.h"
#include "hal_uart.h"

#define USART_TIMEOUT 100

extern UART_HandleTypeDef huart2;

uint8_t console_key_input;

port_config debug_port = {console_port, &huart2, &console_key_input, 1};
port_config sensor_port1{undefined_port, NULL, (uint8_t *)NULL};
port_config sensor_port2{undefined_port, NULL, (uint8_t *)NULL};
#define MAX_UART_PORTS 3
port_config port_list[MAX_UART_PORTS] = {sensor_port1, sensor_port2, debug_port};

hal_uart::uart_device console(debug_port);

void console_putchar(uint8_t msg)
{
    console.send_byte(msg);
}
void console_rx_init()
{
    console.init();
}

extern "C" port_config *get_uart_config(UART_HandleTypeDef *handle)
{
    for (int i = 0; i < MAX_UART_PORTS; i++)
    {
        if (handle == port_list[i].handle)
            return &port_list[i];
    }
    return NULL;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    port_config *port = get_uart_config(huart);
    if (port->id != undefined_port)
    {
        if (port->id == console_port)
            consoleKeyRxd(*(port->bufptr));
        HAL_UART_Receive_IT(port->handle, port->bufptr, port->rx_buf_size);
    }
}