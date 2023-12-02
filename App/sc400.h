#pragma once
#include <array>
#include <string>

#include "protocolbase.h"

class Sc400 : public ProtocolBase
{
public:
    const int ETX = 2;
    const int STX = 3;

    Sc400() = default;

    void check_crc(char *data) override
    {
    }

    const std::string makeRequestPacket(int reqid)
    {
        if (reqid > 999)
            return std::string{}; // invalid reqid
        uint8_t bcc{0};
        buffer[0] = ETX;
        std::string str = std::to_string(reqid);
        while (str.length() < 3)
            str = "0" + str;

        for (size_t i = 0; i < 3; ++i)
        {
            auto c = static_cast<uint8_t>(str[i]);
            buffer[i + 1] = c;
            bcc ^= c;
        }
        buffer[4] = STX;
        bcc ^= STX;

        if (bcc < 0x20)
            bcc += 0x20;
        buffer[5] = bcc;
        std::string msg(buffer.begin(), buffer.begin() + 6);
        return msg;
    }

private:
    std::array<uint8_t, 50> buffer;
};