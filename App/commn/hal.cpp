
#include "stm32f1xx_hal.h"
#include "manager.h"

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    sensorRxdData(huart);
}
