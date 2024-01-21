#pragma once
#include <array>
#include <string>
#include <algorithm>
#include <charconv>

#include "model.h"

#include "protocolbase.h"
using namespace sikora;

class Sc400 : public ProtocolBase
{
public:
    static constexpr int STX = 2;
    static constexpr int ETX = 3;
    static constexpr int TX_BUFFER_SIZE = 50;

    Sc400() = default;

    bool CheckCrc(const char *data, const char crc) override;

    const char *MakeRequestPacket(const int reqid) override;

    bool MakeResponsePacket(const int reqid, char *payload) override;

    bool UpdateModel(const char *data, Configurations &conf);

private:
    char m_tx_buffer[TX_BUFFER_SIZE];
};