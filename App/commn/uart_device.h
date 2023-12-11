#pragma once
#include "cmsis_os.h"
#include "circularbuf.h"
#include "sc400.h"

template <ProtocolBase::ProtocolId Id>
struct ProtocolSelector;

template <>
struct ProtocolSelector<ProtocolBase::SC400>
{
    using type = Sc400;
};
template <>
struct ProtocolSelector<ProtocolBase::NONE>
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
class uart_device
{
    static constexpr size_t LOG_SIZE = 100;
    static constexpr size_t msg_size = 50;
    static constexpr int default_timeout = 150;

    SelectedProtocol<protocol_id> protocolInstance;

public:
    uart_device(UART_HandleTypeDef *hnd_port, size_t buf_size, int timeout = default_timeout) : m_hnd_port(hnd_port), m_buf_size(buf_size), m_timeout(timeout) {}
    inline bool Init()
    {
        if (HAL_UART_Receive_IT(m_hnd_port, m_rx_buffer, m_buf_size) != HAL_OK)
        {
            return false;
        }
        return true;
    }
    void SendRequestPacket(int id)
    {
        auto cmd = MakeRequestPacket(302);
        const uint8_t *cmd_array = reinterpret_cast<const uint8_t *>(&cmd[0]);
        if (auto size = cmd.length(); size > 0)
            SendByte(cmd_array, size);
    }
    void UpdateModel(const char *data, Configurations &conf)
    {
        protocolInstance.UpdateModel(data, conf);
    }
    void SendResponsePacket(int id, char *buf)
    {
        auto cmd = MakeResponsePacket(id, buf);
        const uint8_t *cmd_array = reinterpret_cast<const uint8_t *>(&cmd[0]);
        if (auto size = cmd.length(); size > 0)
            SendByte(cmd_array, size);
    }
    const std::string MakeRequestPacket(int id)
    {
        return protocolInstance.MakeRequestPacket(id);
    }
    const std::string MakeResponsePacket(int id, char *buf)
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

    void SendByte(const uint8_t *data, size_t size)
    {
        HAL_UART_Transmit(m_hnd_port, data, size, m_timeout);
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
    uint8_t m_rx_buffer[msg_size];
};