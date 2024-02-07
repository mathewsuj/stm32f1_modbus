#include "model.h"
#ifdef __cplusplus
extern "C" {
#endif
constexpr uint16_t kInvalid = 0xffff;

uint16_t getDbValue(uint16_t adr)
{
  int idx;
  db::TvgDatabase &sensor_data = db::TvgDatabase::getInstance();
  if (adr < db::PRIMARY_PORT_BAUDRATE)
  {
    idx = adr / SENSOR_PARAM_COUNT;

    const auto &sensor_values = sensor_data.getSensorData(idx);

    adr -= idx * SENSOR_PARAM_COUNT;
    switch (adr)
    {
    case db::PRIMARY_SENSOR_STATUS_MEAN_VALUE:
      return sensor_values.status_mean_value;
    case db::PRIMARY_SENSOR_MEAN_VALUE:
      return sensor_values.mean_value;
    case db::PRIMARY_SENSOR_DEVIATION:
      return sensor_values.deviation;
    case db::PRIMARY_SENSOR_BLUE_DIAMETER:
      return sensor_values.blue_diameter;
    case db::PRIMARY_SENSOR_MAGENTA_DIAMETER:
      return sensor_values.magenta_diameter;
    case db::PRIMARY_SENSOR_OVALITY:
      return sensor_values.ovality;
    case db::PRIMARY_SENSOR_POSITION_AXIS_BLUE:
      return sensor_values.position_axis_blue;
    case db::PRIMARY_SENSOR_POSITION_AXIS_MAGENTA:
      return sensor_values.position_axis_magenta;
    default:
      return kInvalid;
    }

  } else
  {
    idx = adr - db::PRIMARY_PORT_BAUDRATE;
    const auto &uart = sensor_data.getPortSettings(idx);
    return uart.BaudRate;
  }

  return kInvalid;
}

uint16_t setDbValue(uint16_t adr, const uint16_t value)
{
  int idx;
  db::TvgDatabase &sensor_data = db::TvgDatabase::getInstance();
  if (adr < db::PRIMARY_PORT_BAUDRATE)
  {
    idx = adr / SENSOR_PARAM_COUNT;

    auto sensor_values = sensor_data.getSensorData(idx);

    adr -= idx * SENSOR_PARAM_COUNT;
    switch (adr)
    {
    case db::PRIMARY_SENSOR_STATUS_MEAN_VALUE:
      sensor_values.status_mean_value.setValue(value);
      break;
    case db::PRIMARY_SENSOR_MEAN_VALUE:
      sensor_values.mean_value.setValue(value);
      break;
    case db::PRIMARY_SENSOR_DEVIATION:
      sensor_values.deviation.setValue(value);
      break;
    case db::PRIMARY_SENSOR_BLUE_DIAMETER:
      sensor_values.blue_diameter.setValue(value);
      break;
    case db::PRIMARY_SENSOR_MAGENTA_DIAMETER:
      sensor_values.magenta_diameter.setValue(value);
      break;
    case db::PRIMARY_SENSOR_OVALITY:
      sensor_values.ovality.setValue(value);
      break;
    case db::PRIMARY_SENSOR_POSITION_AXIS_BLUE:
      sensor_values.position_axis_blue.setValue(value);
      break;
    case db::PRIMARY_SENSOR_POSITION_AXIS_MAGENTA:
      sensor_values.position_axis_magenta.setValue(value);
      break;
    default:
      return kInvalid;
    }
    return sensor_data.updateSensorData(idx, sensor_values);

  } else
  {
    idx = adr - db::PRIMARY_PORT_BAUDRATE;
    return sensor_data.setPortSettings(idx, value);
  }

  return kInvalid;
}
#ifdef __cplusplus
}
#endif