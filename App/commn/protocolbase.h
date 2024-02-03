#pragma once

class ProtocolBase
{
public:
    enum class ProtocolId : uint8_t
    {
        SC400,
        NONE
    };
    virtual bool CheckCrc(const char *data, const char crc) = 0;
    virtual const char *MakeRequestPacket(const int reqid) = 0;
    virtual bool MakeResponsePacket(const int reqid, char *buf) = 0;
    virtual bool UpdateModel(const char *data, db::SensorData &conf) = 0;
    virtual ~ProtocolBase() = default;
};
