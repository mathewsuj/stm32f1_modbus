#pragma once

class ProtocolBase
{
public:
    virtual bool check_crc(const char *data, const char crc) = 0;
    virtual ~ProtocolBase() = default;
};