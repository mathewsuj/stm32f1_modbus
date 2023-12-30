#pragma once
#include <cstddef>
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "model.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void initializeManager(osThreadAttr_t thread_attr);
    extern "C" void sensorRxdData(UART_HandleTypeDef *huart);
    void console_putchar(const char data);
    void console_putstr(const char *data, std::size_t size);
    char *console_getcommand();
    void console_dumpmodel();
    void console_dumpports();
    void uart_setPortConfiguration(uint8_t port, const Communication::SerialPort &setting);

#ifdef __cplusplus
}
#endif