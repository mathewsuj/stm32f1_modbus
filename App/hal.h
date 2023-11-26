#pragma once
#include <cstdint>
#include "stm32f1xx_hal.h"

void console_rx_init();
void console_putchar(uint8_t msg);

void port_rx_init();

enum uart_port_id
{
    undefined_port = -1,
    console_port,
    D2_port,
    D1_port,
    D3_port,
    uart_max
};
typedef struct
{
    uart_port_id id;
    UART_HandleTypeDef *handle;
    uint8_t *bufptr;
    int rx_buf_size;
} port_config;