#pragma once

class ProtocolBase {
public:
  ProtocolBase() = default;
  virtual bool CheckCrc(const char *data, const char crc = 0) = 0;
  virtual const char *MakeRequestPacket(const int reqid) = 0;
  virtual bool MakeResponsePacket(const int reqid, char *buf) = 0;
  virtual bool UpdateModel(const char *data, db::SensorData &conf,
                           bool mono = true) = 0;
  virtual const char *GetFrame(const char *data, const char crc = 0) = 0;
  virtual char GetEOF() = 0;
  virtual char GetSOF() = 0;
  virtual ~ProtocolBase() = default;
};
