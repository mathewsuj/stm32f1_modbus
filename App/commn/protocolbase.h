#pragma once

class ProtocolBase
{
public:
    enum class ProtocolId
    {
        SC400,
        NONE
    };
    virtual bool CheckCrc(const char *data, const char crc) = 0;
    virtual const char *MakeRequestPacket(int reqid) = 0;
    virtual bool MakeResponsePacket(int reqid, char *buf) = 0;
    virtual bool UpdateModel(const char *data, Configurations &conf) = 0;
    virtual ~ProtocolBase() = default;
};
