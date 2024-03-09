#include "model.h"
#include "protocolbase.h"

class Tvg5000 : public ProtocolBase {
public:
  static constexpr int STX = 0xA7;
  static constexpr int ETX = 0xA8;
  bool CheckCrc(const char *data, const char crc) override;

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
                   bool mono = true) override;

  const char *GetFrame(const char *data, const char crc = 0) override;

  char GetEOF() override
  {
    return ETX;
  }
  char GetSOF() override
  {
    return STX;
  }
  char BcdToDecimal(const char hex);
  int BcdToDecimal(const char data[], int size);
};