#pragma once

class ProtocolBase
{
public:
    typedef enum
    {
        SC400,
        NONE
    } ProtocolId;
    virtual bool CheckCrc(const char *data, const char crc) = 0;
    virtual const std::string MakeRequestPacket(int reqid) = 0;
    virtual const std::string MakeResponsePacket(int reqid, char *buf) = 0;
    virtual ~ProtocolBase() = default;
};
class None : public ProtocolBase
{
    bool CheckCrc(const char *data, const char crc)
    {
        return true;
    }
    const std::string MakeRequestPacket(int reqid)
    {
        return std::string{};
    }
    const std::string MakeResponsePacket(int reqid, char *buf)
    {
        return std::string{};
    }
};