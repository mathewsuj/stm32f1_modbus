#include "protocolbase.h"

class None : public ProtocolBase
{
public:
    bool CheckCrc(const char *data, const char crc) override
    {
        (void)data;
        (void)crc;
        return true;
    }
    const char *MakeRequestPacket(int reqid) override
    {
        (void)reqid;
        return nullptr;
    }
    bool MakeResponsePacket(int reqid, char *buf) override
    {
        (void)reqid;
        (void)buf;
        return false;
    }
    bool UpdateModel(const char *data, Configurations &conf)
    {
        (void)data;
        (void)conf;
        return true;
    }
};