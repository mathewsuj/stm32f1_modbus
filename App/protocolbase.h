#pragma once

class ProtocolBase
{
public:
    virtual void check_crc(char *data) = 0;
    virtual ~ProtocolBase() = default;
};