#pragma once
#include <array>

#include "circularbuf.h"
#include "cmsis_os.h"
#include "none.h"
#include "sc400.h"

template <typename portId, typename T>
class UartDevice {
  static constexpr size_t LOG_SIZE = 100;
  static constexpr size_t MSG_SIZE = 50;
  static constexpr int DEFAULT_TIMEOUT = 150;

public:
  UartDevice(UART_HandleTypeDef *hnd_port, size_t buf_size,
             int timeout = DEFAULT_TIMEOUT)
      : m_hnd_port(hnd_port), m_buf_size(buf_size), m_timeout(timeout)
  {
    protocolInstance[static_cast<std::size_t>(ProtocolId::SC400)] =
        (ProtocolBase *)&Sc400Protocol;
    protocolInstance[static_cast<std::size_t>(ProtocolId::NONE)] =
        (ProtocolBase *)&NoneProtocol;
    SetProtocol(ProtocolId::NONE);
  }
  const UART_HandleTypeDef *GetUartHandle()
  {
    return m_hnd_port;
  }
  inline bool Init(const db::Communication::SerialPort &conf)
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
  void SetProtocol(uint8_t id)
  {
    protocolSelector = id;
  }

  bool ChangeSettings(const db::Communication::SerialPort &conf)
  {
    HAL_UART_Abort_IT(m_hnd_port);
    HAL_UART_DeInit(m_hnd_port);

    SetProtocol(conf.Protocol);

    m_hnd_port->Init.BaudRate = conf.BaudRate;
    return (HAL_UART_Init(m_hnd_port) == HAL_OK);
  }

  void SendRequestPacket(const int id)
  {
    auto cmd = MakeRequestPacket(id);

    if (auto size = std::strlen(cmd); size > 0)
      SendByte(cmd, size);
  }

  void UpdateModel(const char *data, db::SensorData &conf)
  {
    protocolInstance[protocolSelector]->UpdateModel(data, conf);
  }

  void SendResponsePacket(const int id, char *buf)
  {
    auto statusOk = MakeResponsePacket(id, buf);

    if (statusOk)
    {
      auto size = std::strlen(buf);
      SendByte(buf, size);
    }
  }

  const char *MakeRequestPacket(const int id)
  {
    return protocolInstance[protocolSelector]->MakeRequestPacket(id);
  }

  bool MakeResponsePacket(const int id, char *buf)
  {
    return protocolInstance[protocolSelector]->MakeResponsePacket(id, buf);
  }

  bool CheckCrc(const char *data, const char crc)
  {
    return protocolInstance[protocolSelector]->CheckCrc(data, crc);
  }

  void DataRdy()
  {
    m_msg_buffer.write((T *)m_rx_buffer, m_buf_size);
    HAL_UART_Receive_IT(m_hnd_port, m_rx_buffer, m_buf_size);
  }

  void SendByte(const void *data, const size_t size)
  {
    HAL_UART_Transmit(m_hnd_port, (const uint8_t *)data, size, m_timeout);
  }

  const T *GetNextFrame()
  {
    const char *inp = GetString(protocolInstance[protocolSelector]->GetEOF());
    if (!inp)
      return inp;
    char crc = 0;
    if (protocolSelector == (int)ProtocolId::SC400)
      crc = Read();

    return protocolInstance[protocolSelector]->GetFrame(inp, crc);
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
  UART_HandleTypeDef *m_hnd_port;
  size_t m_buf_size;
  int m_timeout;
  uint8_t m_rx_buffer[MSG_SIZE];
  uint8_t protocolSelector;
  None NoneProtocol;
  Sc400 Sc400Protocol;

  std::array<ProtocolBase *, static_cast<std::size_t>(ProtocolId::PROTO_MAX)>
      protocolInstance;
};