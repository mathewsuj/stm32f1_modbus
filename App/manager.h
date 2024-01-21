#pragma once
#include <cstddef>
#include "stm32f1xx_hal.h"

#include "model.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void initializeManager();
    extern "C" void sensorRxdData(const UART_HandleTypeDef *huart);
    void console_putchar(const char data);
    void console_putstr(const char *data, const std::size_t size);
    char *console_getcommand();
    void console_dumpmodel();
    void console_dumpports();
    void uart_setPortConfiguration(const uint8_t port, const Communication::SerialPort &setting);

#ifdef __cplusplus
}
#endif