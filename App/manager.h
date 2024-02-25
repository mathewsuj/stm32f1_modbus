#pragma once
#include "stm32f1xx_hal.h"
#include <cstddef>

#include "model.h"

#ifdef __cplusplus
extern "C" {
#endif

void initializeManager();
extern "C" void sensorRxdData(const UART_HandleTypeDef *huart);
void consolePutchar(const char data);
void consolePutstr(const char *data, const std::size_t size);
char *consoleGetcommand();
void consoleDumpmodel();
void consoleDumpports();
void uartSetPortConfiguration(const uint8_t port,
                              const db::Communication::SerialPort &setting);

#ifdef __cplusplus
}
#endif