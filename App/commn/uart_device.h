#pragma once
#include "cmsis_os.h"
#include "circularbuf.h"
#include "sc400.h"
#include "none.h"

template <ProtocolBase::ProtocolId Id>
struct ProtocolSelector;

template <>
struct ProtocolSelector<ProtocolBase::ProtocolId::SC400>
{
    using type = Sc400;
};
template <>
struct ProtocolSelector<ProtocolBase::ProtocolId::NONE>
{
    using type = None;
};

template <ProtocolBase::ProtocolId Id>
using SelectedProtocol = typename ProtocolSelector<Id>::type;

template <typename portId>
struct portData
{
    UART_HandleTypeDef *hnd;
    portData(UART_HandleTypeDef *handler) : hnd(handler) {}
};

template <ProtocolBase::ProtocolId protocol_id, typename T>
class UartDevice
{
    static constexpr size_t LOG_SIZE = 100;
    static constexpr size_t MSG_SIZE = 50;
    static constexpr int DEFAULT_TIMEOUT = 150;

    SelectedProtocol<protocol_id> protocolInstance;

public:
    UartDevice(UART_HandleTypeDef *hnd_port, size_t buf_size, int timeout = DEFAULT_TIMEOUT) : m_hnd_port(hnd_port), m_buf_size(buf_size), m_timeout(timeout) {}
    inline bool Init(const Communication::SerialPort &conf)
    {
        if (ChangeSettings(conf))
        {
            if (HAL_UART_Receive_IT(m_hnd_port, m_rx_buffer, m_buf_size) != HAL_OK)
            {
                return false;
            }
        }
        return true;
    }

    bool ChangeSettings(const Communication::SerialPort &conf)
    {
        HAL_UART_Abort_IT(m_hnd_port);
        HAL_UART_DeInit(m_hnd_port);

        m_hnd_port->Init.BaudRate = conf.BaudRate;
        return (HAL_UART_Init(m_hnd_port) == HAL_OK);
    }

    void SendRequestPacket(int id)
    {
        auto cmd = MakeRequestPacket(302);

        if (auto size = std::strlen(cmd); size > 0)
            SendByte(cmd, size);
    }

    void UpdateModel(const char *data, Configurations &conf)
    {
        protocolInstance.UpdateModel(data, conf);
    }

    void SendResponsePacket(int id, char *buf)
    {
        auto statusOk = MakeResponsePacket(id, buf);

        if (statusOk)
        {
            auto size = std::strlen(buf);
            SendByte(buf, size);
        }
    }

    const char *MakeRequestPacket(int id)
    {
        return protocolInstance.MakeRequestPacket(id);
    }

    bool MakeResponsePacket(int id, char *buf)
    {
        return protocolInstance.MakeResponsePacket(id, buf);
    }

    bool CheckCrc(const char *data, const char crc)
    {
        return protocolInstance.CheckCrc(data, crc);
    }

    void DataRdy()
    {
        m_msg_buffer.write((T *)m_rx_buffer, m_buf_size);
        HAL_UART_Receive_IT(m_hnd_port, m_rx_buffer, m_buf_size);
    }

    void SendByte(const void *data, size_t size)
    {
        HAL_UART_Transmit(m_hnd_port, (const uint8_t *)data, size, m_timeout);
    }

    T *GetString(T delimiter)
    {
        return m_msg_buffer.getString(delimiter);
    }

    const T Read()
    {
        return m_msg_buffer.read();
    }

private:
    CircularBuffer<T, LOG_SIZE> m_msg_buffer;
    int m_timeout;
    UART_HandleTypeDef *m_hnd_port;
    size_t m_buf_size;
    uint8_t m_rx_buffer[MSG_SIZE];
};