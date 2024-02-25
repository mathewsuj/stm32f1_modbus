#include <cstring>

#include "sc400.h"

const char *Sc400::GetFrame(const char *data, const char crc)
{
  while (*data)
  {
    if (*data != STX)
      data++;
    else
    {
      if (!CheckCrc(data, crc))
        break;
      if (*(data + 1) == NAK)
        break;
      return data;
    }
  }
  return nullptr;
}
bool Sc400::CheckCrc(const char *data, const char crc)
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
bool Sc400::MakeResponsePacket(const int reqid, char *payload)
{
  static_assert(StatusMeanValue.pos == db::db_start);
  if (reqid > 999)
    return false; // invalid reqid
  if (reqid != 302)
    return false;

  uint8_t bcc{0};
  payload[0] = STX;
  std::string str = std::to_string(reqid);
  while (str.length() < 3)
    str = "0" + str;

  for (size_t i = 0; i < 3; ++i)
  {
    auto c = str[i];
    payload[i + 1] = c;
    bcc ^= c;
  }
  // populate data
  for (size_t i = StatusMeanValue.pos; i < Sc400DataEnd.pos; ++i)
  {
    auto c = *(payload + i);
    // m_tx_buffer[i] = c;
    bcc ^= c;
  }

  payload[Sc400DataEnd.pos] = ETX;
  bcc ^= ETX;

  if (bcc < 0x20)
    bcc += 0x20;
  payload[Sc400DataEnd.pos + 1] = bcc;
  payload[Sc400DataEnd.pos + 2] = '\0';

  return true;
}
const char *Sc400::MakeRequestPacket(const int reqid)
{
  if (reqid > 999)
    return nullptr; // invalid reqid
  uint8_t bcc{0};
  m_tx_buffer[0] = STX;
  std::string str = std::to_string(reqid);
  while (str.length() < 3)
    str = "0" + str;

  for (size_t i = 0; i < 3; ++i)
  {
    auto c = static_cast<uint8_t>(str[i]);
    m_tx_buffer[i + 1] = c;
    bcc ^= c;
  }
  m_tx_buffer[4] = ETX;
  bcc ^= ETX;

  if (bcc < 0x20)
    bcc += 0x20;
  m_tx_buffer[5] = bcc;
  m_tx_buffer[6] = '\0';
  return m_tx_buffer;
}
bool Sc400::UpdateModel(const char *data, db::SensorData &conf)
{
  conf.status_mean_value =
      (*(data + StatusMeanValue.pos) - '0'); // skip STX and Command Id bytes

  auto parseInteger = [](const char *begin, std::size_t size,
                         auto &value) -> bool {
    const auto end = begin + size;
    auto [p, ec] = std::from_chars(begin, end, value, 10);
    return ec == std::errc() && p == end;
  };

  uint16_t value;
  if (!parseInteger(data + MeanValue.pos, MeanValue.size, value))
    return false;
  conf.mean_value = value;

  char sign = *(data + SignDeviation.pos);

  if (!parseInteger(data + Deviation.pos, Deviation.size, value))
    return false;
  conf.deviation = value;

  if (sign == '_')
    conf.deviation *= -1;

  if (!parseInteger(data + BlueDiameter.pos, BlueDiameter.size, value))
    return false;
  conf.blue_diameter = value;

  if (!parseInteger(data + MagentaDiameter.pos, MagentaDiameter.size, value))
    return false;
  conf.magenta_diameter = value;

  if (!parseInteger(data + Ovality.pos, Ovality.size, value))
    return false;
  conf.ovality = value;

  sign = *(data + SignPositionAxisBlue.pos);

  if (!parseInteger(data + PositionAxisBlue.pos, PositionAxisBlue.size, value))
    return false;
  conf.position_axis_blue = value;

  if (sign == '-')
    conf.position_axis_blue *= -1;

  sign = *(data + SignPositionAxisMagenta.pos);

  if (!parseInteger(data + PositionAxisMagenta.pos, PositionAxisMagenta.size,
                    value))
    return false;
  conf.position_axis_magenta = value;

  if (sign == '_')
    conf.position_axis_magenta *= -1;

  return true;
}