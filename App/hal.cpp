#include "stm32f1xx_hal.h"
#include "console.h"
#include "main.h"

extern UART_HandleTypeDef huart2;
#define USART_TIMEOUT 100
static uint8_t console_key_input;

void console_putchar(uint8_t msg)
{
    if (msg != '\0')
        HAL_UART_Transmit(&huart2, &msg, 1, USART_TIMEOUT);
}
void console_rx_init()
{

    if (HAL_UART_Receive_IT(&huart2, &console_key_input, 1) != HAL_OK)
    {
        debugLog("rx int uart2 error\r\n");
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {

        // HAL_UART_Receive_IT(&huart1, uart_rx, UART_MSG_LEN);
    }
    if (huart->Instance == USART2)
    {

        consoleKeyRxd(console_key_input);
        HAL_GPIO_TogglePin(USER_LED1_GPIO_Port, USER_LED1_Pin);
        //   debug_log("key rxd: %c", console_key_input);
        HAL_UART_Receive_IT(&huart2, &console_key_input, 1);
    }
}