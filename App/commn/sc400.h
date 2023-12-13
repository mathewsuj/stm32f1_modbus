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
    const int STX = 2;
    const int ETX = 3;
    static const int TX_BUFFER_SIZE = 50;

    Sc400() = default;

    bool CheckCrc(const char *data, const char crc) override;

    const char *MakeRequestPacket(int reqid) override;

    bool MakeResponsePacket(int reqid, char *payload) override;

    bool UpdateModel(const char *data, Configurations &conf);

private:
    char m_tx_buffer[TX_BUFFER_SIZE];
};