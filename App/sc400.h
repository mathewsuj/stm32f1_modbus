#pragma once
#include <array>
#include <string>
#include <charconv>
#include "model.h"

#include "protocolbase.h"

class Sc400 : public ProtocolBase
{
public:
    const int STX = 2;
    const int ETX = 3;

    Sc400() = default;

    bool check_crc(const char *data, const char crc) override
    {
        uint8_t bcc = 0;
        if (*data != STX)
            return false;

        for (data++; *data; data++)
        {
            bcc ^= *data; // XOR the rest of the bytes with CRC
        }
        bcc ^= ETX;
        if (bcc < 0x20)
            bcc += 0x20;

        return crc == bcc;
    }

    const std::string makeRequestPacket(int reqid)
    {
        if (reqid > 999)
            return std::string{}; // invalid reqid
        uint8_t bcc{0};
        tx_buffer[0] = ETX;
        std::string str = std::to_string(reqid);
        while (str.length() < 3)
            str = "0" + str;

        for (size_t i = 0; i < 3; ++i)
        {
            auto c = static_cast<uint8_t>(str[i]);
            tx_buffer[i + 1] = c;
            bcc ^= c;
        }
        tx_buffer[4] = STX;
        bcc ^= STX;

        if (bcc < 0x20)
            bcc += 0x20;
        tx_buffer[5] = bcc;
        std::string msg(tx_buffer.begin(), tx_buffer.begin() + 6);
        return msg;
    }
    bool updateModel(const char *data, Configurations &conf)
    {
        int index = 1;
        conf.status_mean_value = *data - '0';

        auto parseInteger = [](const char *begin, const char *end, uint16_t &value) -> bool
        {
            auto [p, ec] = std::from_chars(begin, end, value);
            return ec == std::errc() && p == end;
        };
        if (!parseInteger(data + 1, data + 5, conf.mean_value))
            return false;

        if (!parseInteger(data + 7, data + 11, conf.deviation))
            return false;
        if (*(data + 6) == '-')
            conf.deviation *= -1;
        if (!parseInteger(data + 12, data + 16, conf.blue_diameter))
            return false;

        if (!parseInteger(data + 17, data + 21, conf.magenta_diameter))
            return false;

        if (!parseInteger(data + 22, data + 26, conf.ovality))
            return false;

        if (!parseInteger(data + 28, data + 32, conf.position_axis_blue))
            return false;
        if (*(data + 27) == '-')
            conf.position_axis_blue *= -1;

        if (!parseInteger(data + 34, data + 38, conf.position_axis_magenta))
            return false;
        if (*(data + 33) == '-')
            conf.position_axis_magenta *= -1;
        return true;
    }

private:
    std::array<uint8_t, 50> tx_buffer;
};