#include <cstring>

#include "tvg5000.h"

const char *Tvg5000::GetFrame(const char *data, const char crc)
{
  while (*data)
  {
    if (*data != STX)
      data++;
    else
    {
      while (*data == STX)
        data++;
      data--;
      if (!CheckCrc(data, crc))
        break;

      return data;
    }
  }
  return nullptr;
}
bool Tvg5000::CheckCrc(const char *data, const char crc)
{
  (void)crc;
  uint8_t bcc = 0;
  if (*data != STX)
    return false;
  data++;
  for (int i = 0; i < 6; data++, i++)
  {
    bcc += BcdToDecimal(*data); // XOR the rest of the bytes with CRC
  }
  bcc = bcc % 100;

  return BcdToDecimal(*data) == bcc;
}
char Tvg5000::BcdToDecimal(const char hex)
{
  return ((hex & 0xF0) >> 4) * 10 + (hex & 0x0F);
}
int Tvg5000::BcdToDecimal(const char data[], int size)
{
  int number = 0;
  for (int i = 0; i < size; ++i)
  {
    number *= 100;
    number += BcdToDecimal(data[i]);
  }
  return number;
}

bool Tvg5000::UpdateModel(const char *data, db::SensorData &conf, bool mono)
{
  // conf.status_mean_value =
  //     (*(data + StatusMeanValue.pos) - '0'); // skip STX and Command Id bytes
  char sign = *(data + db::tvg5000::Data1.pos);
  char number[db::tvg5000::Data1.size];
  std::memcpy(number, data + db::tvg5000::Data1.pos, db::tvg5000::Data1.size);
  number[0] &= 0xf;
  int parameter = BcdToDecimal(number, db::tvg5000::Data1.size);

  if (sign == 0x0)
  {
    parameter *= -1;
  }
  if (mono)
  {
    conf.wire_position = parameter;
  } else
  {
    conf.x_axis_diameter = parameter;
  }

  std::memcpy(number, data + db::tvg5000::Data2.pos, db::tvg5000::Data2.size);

  parameter = BcdToDecimal(number, db::tvg5000::Data2.size);

  if (mono)
  {
    conf.mean_diameter = parameter;
  } else
  {
    conf.y_axis_diameter = parameter;
  }

  return true;
}