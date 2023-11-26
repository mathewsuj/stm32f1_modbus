#pragma
#include <cstdint>
#include "hal.h"

namespace hal_uart
{

    class uart_device
    {
    public:
        static constexpr int default_timeout = 100;
        uart_device(port_config p, int timeout = default_timeout) : port(p), timeout(timeout) {}
        void set_handle(UART_HandleTypeDef *hal_handle)
        {
            port.handle = hal_handle;
        }
        void send_byte(uint8_t data)
        {
            if (data != '\0')
                HAL_UART_Transmit(port.handle, &data, 1, timeout);
        }
        void init()
        {
            if (HAL_UART_Receive_IT(port.handle, port.bufptr, port.rx_buf_size) != HAL_OK)
            {
                debugLog("rx int uart2 error\r\n");
            }
        }

    private:
        port_config port;
        int timeout;
    };
}