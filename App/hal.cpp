void console_putchar(char msg)
{
    if (msg != '\0')
        HAL_UART_Transmit(&huart2, &msg, 1, USART_TIMEOUT);
}
console_rx_init()
{
    if (HAL_UART_Receive_IT(&huart2, &console_key_input, 1) != HAL_OK)
    {
        debugLog("rx int uart2 error\r\n");
        status |= 0x2;
    }
}