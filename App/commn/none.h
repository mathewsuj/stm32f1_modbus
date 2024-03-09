#include "protocolbase.h"

class None : public ProtocolBase {
public:
  None() = default;
  bool CheckCrc(const char *data, const char crc) override
  {
    (void)data;
    (void)crc;
    return true;
  }
  const char *MakeRequestPacket(const int reqid) override
  {
    (void)reqid;
    return nullptr;
  }
  bool MakeResponsePacket(const int reqid, char *buf) override
  {
    (void)reqid;
    (void)buf;
    return false;
  }
  bool UpdateModel(const char *data, db::SensorData &conf,
                   bool mono = true) override
  {
    (void)data;
    (void)conf;
    (void)mono;
    return true;
  }
  const char *GetFrame(const char *data, const char crc)
  {
    (void)data;
    (void)crc;
    return nullptr;
  }
  char GetEOF()
  {
    return 0;
  }
  char GetSOF()
  {
    return 0;
  }
};