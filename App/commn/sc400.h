#pragma once
#include <algorithm>
#include <array>
#include <charconv>
#include <string>

#include "model.h"

#include "protocolbase.h"
using namespace db::sikora;

class Sc400 : public ProtocolBase {
public:
  static constexpr int STX = 0x2;
  static constexpr int ETX = 0x3;
  static constexpr int ACK = 0x6;
  static constexpr int NAK = 0x15;
  static constexpr int TX_BUFFER_SIZE = 50;

  Sc400() = default;

  bool CheckCrc(const char *data, const char crc) override;
  const char *MakeRequestPacket(const int reqid) override;
  bool MakeResponsePacket(const int reqid, char *payload) override;
  bool UpdateModel(const char *data, db::SensorData &conf);
  const char *GetFrame(const char *data, const char crc);
  char GetEOF()
  {
    return ETX;
  }
  char GetSOF()
  {
    return STX;
  }

private:
  char m_tx_buffer[TX_BUFFER_SIZE];
};