#pragma
#include <cstdint>
#include "hal.h"

namespace hal_uart
{

    class uart_device
    {
    public:
        static constexpr int default_timeout = 100;
        uart_device(port_config p, int timeout = default_timeout) : m_port(p), m_timeout(timeout) {}

        void set_handle(UART_HandleTypeDef *hal_handle)
        {
            m_port.handle = hal_handle;
        }
        void send_byte(uint8_t data)
        {
            if (data != '\0')
                HAL_UART_Transmit(m_port.handle, &data, 1, m_timeout);
        }
        void init(port_config p, int timeout = default_timeout)
        {
            m_port = p;
            m_timeout = timeout;
            init();
        }
        void init()
        {
            if (HAL_UART_Receive_IT(m_port.handle, m_port.bufptr, m_port.rx_buf_size) != HAL_OK)
            {
                debugLog("rx int uart2 error\r\n");
            }
        }

    private:
        port_config m_port;
        int m_timeout;
    };
}