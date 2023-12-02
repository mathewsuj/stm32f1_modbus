#include <cstddef>

#include "hal.h"
#include "manager.h"

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    sensorRxdData(huart);
}
