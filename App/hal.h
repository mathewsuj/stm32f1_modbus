#pragma once
#include <cstdint>
#include "stm32f1xx_hal.h"

enum uart_port_id
{
    undefined_port = -1,
    debug_port,
    primary_port,
    secondary_port,
    uart_max
};
typedef struct
{
    uart_port_id id;
    UART_HandleTypeDef *handle;
    uint8_t *bufptr;
    int rx_buf_size;
} port_config;

void console_rx_init();
void console_putchar(uint8_t msg);
void sensor_init(uart_port_id id, int phy_port);
void port_rx_init();