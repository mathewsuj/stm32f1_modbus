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
        conf.status_mean_value = *(data + StatusMeanValue.pos) - '0'; // skip STX and Command Id bytes

        auto parseInteger = [](const char *begin, std::size_t size, auto &value) -> bool
        {
            const auto end = begin + size;
            auto [p, ec] = std::from_chars(begin, end, value, 10);
            return ec == std::errc() && p == end;
        };

        if (!parseInteger(data + MeanValue.pos, MeanValue.size, conf.mean_value))
            return false;

        conf.sign_deviation = *(data + StatusMeanValue.pos); // skip STX and Command Id bytes

        if (!parseInteger(data + Deviation.pos, Deviation.size, conf.deviation))
            return false;

        if (!parseInteger(data + BlueDiameter.pos, BlueDiameter.size, conf.blue_diameter))
            return false;

        if (!parseInteger(data + MagentaDiameter.pos, MagentaDiameter.size, conf.magenta_diameter))
            return false;

        if (!parseInteger(data + Ovality.pos, Ovality.size, conf.ovality))
            return false;

        if (!parseInteger(data + PositioAxisBlue.pos, PositioAxisBlue.size, conf.position_axis_blue))
            return false;

        if (!parseInteger(data + PositioAxisMagenta.pos, PositioAxisMagenta.size, conf.position_axis_magenta))
            return false;

        return true;
    }

private:
    std::array<uint8_t, 50> tx_buffer;
};