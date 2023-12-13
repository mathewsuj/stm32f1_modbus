#include "protocolbase.h"

class None : public ProtocolBase
{
public:
    bool CheckCrc(const char *data, const char crc) override
    {
        return true;
    }
    const char *MakeRequestPacket(int reqid) override
    {
        return nullptr;
    }
    bool MakeResponsePacket(int reqid, char *buf) override
    {
        return false;
    }
    bool UpdateModel(const char *data, Configurations &conf)
    {
        return true;
    }
};