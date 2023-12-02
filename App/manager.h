#pragma once
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct uart_debug_port
    {
    };
    struct uart_primary_sensor
    {
    };
    struct uart_secondary_sensor
    {
    };

    struct sc400
    {
    };

    void initializeManager(osThreadAttr_t thread_attr);
    extern "C" void sensorRxdData(UART_HandleTypeDef *huart);
    void console_putchar(const uint8_t data);
    char *console_getcommand();

#ifdef __cplusplus
}
#endif