#pragma once
#include "cmsis_os.h"
#include "circularbuf.h"
#include "protocolbase.h"

template <typename PortId, typename T>
class uart_device
{
    static constexpr size_t LOG_SIZE = 100;
    static constexpr size_t msg_size = 50;
    static constexpr int default_timeout = 150;

public:
    uart_device(UART_HandleTypeDef *hnd_port, size_t buf_size, int timeout = default_timeout) : m_hnd_port(hnd_port), m_buf_size(buf_size), m_timeout(timeout) {}
    inline void init()
    {
        if (HAL_UART_Receive_IT(m_hnd_port, m_rx_buffer, m_buf_size) != HAL_OK)
        {
            //    debugLog("rx int uart2 error\r\n");
        }
    }
    void attachProtocolHandler(ProtocolBase *ptr)
    {
        protocolHandler = ptr;
    }
    void datardy()
    {

        m_msg_buffer.write((T *)m_rx_buffer, m_buf_size);
        HAL_UART_Receive_IT(m_hnd_port, m_rx_buffer, m_buf_size);
    }

    void send_byte(const uint8_t *data, size_t size)
    {
        HAL_UART_Transmit(m_hnd_port, data, size, m_timeout);
    }
    T *getString(T delimiter)
    {
        return m_msg_buffer.getString(delimiter);
    }
    const T read()
    {
        return m_msg_buffer.read();
    }
private:
    CircularBuffer<T, LOG_SIZE> m_msg_buffer;
    int m_timeout;
    UART_HandleTypeDef *m_hnd_port;
    size_t m_buf_size;
    uint8_t m_rx_buffer[msg_size];
    ProtocolBase *protocolHandler;
    osMutexId_t bufMutex;
};