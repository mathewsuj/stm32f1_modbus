#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "cmsis_os.h"
#include "eeprom_def.h"
#include "hal.h"

// number of gauge sensors
constexpr int kSensorCount = 1;
// user uart ports
constexpr int kUartCount = 3;

struct uart_debug_port {};
struct uart_primary_sensor {};
struct uart_secondary_sensor {};
struct uart_PC {};

struct sc400 {};

namespace db
{
template <typename T>
class Data {
private:
  T value;
  const T min;
  const T max;
  bool persistant;
  bool changed;

public:
  Data(T defaultValue, const T minVal, T maxVal, bool persist)
      : value(defaultValue), min(minVal), max(maxVal), persistant(persist),
        changed(false)
  {}

  bool setValue(T inp)
  {
    if (inp >= min && inp <= max)
    {
      if (value != inp)
      {
        value = inp;
        changed = true;
      }
      return true;
    }
    return false;
  }

  Data<T> &operator=(const T &rhs)
  {
    setValue(rhs);
    return *this;
  }
  Data<T> &operator*=(const T &rhs)
  {
    value *= rhs;
    return *this;
  }
  T getValue() const
  {
    return value;
  }
  operator T() const
  {
    return value;
  }
  bool isPersistant()
  {
    return persistant;
  }
  bool isChanged()
  {
    return changed;
  }
  void flush()
  {
    changed = false;
  }
};

struct DataDef {
  std::size_t pos;
  std::size_t size;
};

struct SensorData {

  Data<uint8_t> status_mean_value{0, 0, 1, false};
  Data<uint16_t> mean_value{6016, 0, 6100, false};
  Data<int16_t> deviation{0, -2000, 2000, false};
  Data<uint16_t> blue_diameter{0, 0, 7000, false};
  Data<uint16_t> magenta_diameter{0, 0, 7000, false};
  Data<uint16_t> ovality{0, 0, 100, false};
  Data<int16_t> position_axis_blue{0, -1000, 1000, false};
  Data<int16_t> position_axis_magenta{0, -1000, 1000, false};
};
constexpr int db_start = 1;
namespace sikora
{
constexpr DataDef StatusMeanValue{db_start, 1};
constexpr DataDef MeanValue{db_start + 1, 5};
constexpr DataDef SignDeviation{db_start + 6, 1};
constexpr DataDef Deviation{db_start + 7, 5};
constexpr DataDef BlueDiameter{db_start + 12, 5};
constexpr DataDef MagentaDiameter{db_start + 17, 5};
constexpr DataDef Ovality{db_start + 22, 5};
constexpr DataDef SignPositionAxisBlue{db_start + 27, 1};
constexpr DataDef PositionAxisBlue{db_start + 28, 5};
constexpr DataDef SignPositionAxisMagenta{db_start + 33, 1};
constexpr DataDef PositionAxisMagenta{db_start + 34, 5};
constexpr DataDef Sc400DataEnd{db_start + 39, 0};
} // namespace sikora

namespace Communication
{
enum class ParityType : uint8_t { Even, Odd, None };
struct SerialPort {
  Data<uint16_t> BaudRate{19200, 9600, 48200, true};
  Data<ParityType> Parity{ParityType::Even, ParityType::Even, ParityType::Odd,
                          false};
  Data<uint8_t> StopBits{1, 1, 2, false};
  SerialPort() = default;
  void setValue(SerialPort p)
  {
    BaudRate.setValue(p.BaudRate);
  }
  void setValue(int baud)
  {
    BaudRate.setValue(baud);
  }
};

} // namespace Communication

struct TvgDb {
  std::array<SensorData, kSensorCount> sensor_data;
  std::array<Communication::SerialPort, kUartCount> uart_port;
};

class TvgDatabase {
private:
  TvgDatabase()
  {
    readDbFromFlash();
  }

public:
  using ParityType = Communication::ParityType;
  TvgDatabase(const TvgDatabase &) = delete;
  TvgDatabase(TvgDatabase &&) = delete;
  TvgDatabase &operator=(const TvgDatabase &) = delete;
  TvgDatabase &operator=(TvgDatabase &&) = delete;

  int getSensorParameter(int idx, int address)
  {
    switch (address)
    {
    case PRIMARY_SENSOR_STATUS_MEAN_VALUE:
      return tvgdb.sensor_data[idx].status_mean_value;
    }
  }

  void readDbFromFlash()
  {
    for (int idx = 0; idx < kSensorCount; idx++)
    {
      uint16_t offset = idx * SENSOR_PARAM_COUNT;
      getValueFromEEPROM(tvgdb.sensor_data[idx].status_mean_value,
                         PRIMARY_SENSOR_STATUS_MEAN_VALUE + offset);
      getValueFromEEPROM(tvgdb.sensor_data[idx].mean_value,
                         PRIMARY_SENSOR_MEAN_VALUE + offset);
      getValueFromEEPROM(tvgdb.sensor_data[idx].deviation,
                         PRIMARY_SENSOR_DEVIATION + offset);
      getValueFromEEPROM(tvgdb.sensor_data[idx].blue_diameter,
                         PRIMARY_SENSOR_BLUE_DIAMETER + offset);
      getValueFromEEPROM(tvgdb.sensor_data[idx].magenta_diameter,
                         PRIMARY_SENSOR_MAGENTA_DIAMETER + offset);
      getValueFromEEPROM(tvgdb.sensor_data[idx].ovality,
                         PRIMARY_SENSOR_OVALITY + offset);
      getValueFromEEPROM(tvgdb.sensor_data[idx].position_axis_blue,
                         PRIMARY_SENSOR_POSITION_AXIS_BLUE + offset);
      getValueFromEEPROM(tvgdb.sensor_data[idx].position_axis_magenta,
                         PRIMARY_SENSOR_POSITION_AXIS_MAGENTA + offset);
    }
    for (int idx = 0; idx < kUartCount; idx++)
    {
      getValueFromEEPROM(tvgdb.uart_port[idx].BaudRate,
                         PRIMARY_PORT_BAUDRATE + idx);
    }
  }
  template <typename T>
  void getValueFromEEPROM(Data<T> &x, uint8_t adr)
  {
    if (x.isPersistant())
      x.setValue(EEPromRead(adr));
  }
  template <typename T>
  void setValueInEEPROM(Data<T> &x, uint8_t adr, const T value)
  {
    x.setValue(value);
    if (x.isPersistant() && x.isChanged())
      EEPromWrite(adr, value);
  }
  bool updateSensorData(const int idx, const SensorData &newData)
  {

    uint16_t offset = idx * SENSOR_PARAM_COUNT;
    setValueInEEPROM<uint8_t>(tvgdb.sensor_data[idx].status_mean_value,
                              PRIMARY_SENSOR_STATUS_MEAN_VALUE + offset,
                              newData.status_mean_value);

    setValueInEEPROM<uint16_t>(tvgdb.sensor_data[idx].mean_value,
                               PRIMARY_SENSOR_MEAN_VALUE + offset,
                               newData.mean_value);

    setValueInEEPROM<int16_t>(tvgdb.sensor_data[idx].deviation,
                              PRIMARY_SENSOR_DEVIATION + offset,
                              newData.deviation);

    setValueInEEPROM<uint16_t>(tvgdb.sensor_data[idx].blue_diameter,
                               PRIMARY_SENSOR_BLUE_DIAMETER + offset,
                               newData.blue_diameter);

    setValueInEEPROM<uint16_t>(tvgdb.sensor_data[idx].magenta_diameter,
                               PRIMARY_SENSOR_MAGENTA_DIAMETER + offset,
                               newData.magenta_diameter);

    setValueInEEPROM<uint16_t>(tvgdb.sensor_data[idx].ovality,
                               PRIMARY_SENSOR_OVALITY + offset,
                               newData.ovality);

    setValueInEEPROM<int16_t>(tvgdb.sensor_data[idx].position_axis_blue,
                              PRIMARY_SENSOR_POSITION_AXIS_BLUE + offset,
                              newData.position_axis_blue);

    setValueInEEPROM<int16_t>(tvgdb.sensor_data[idx].position_axis_magenta,
                              PRIMARY_SENSOR_POSITION_AXIS_MAGENTA + offset,
                              newData.position_axis_magenta);
    return true;
  }
  bool setPortSettings(int port, int baud)
  {
    int portidx = port - 1;
    if ((portidx >= 0) && (portidx < kUartCount))
    {
      auto result = tvgdb.uart_port[portidx].BaudRate.setValue(baud);
      if (tvgdb.uart_port[portidx].BaudRate.isChanged())
      {
        // save in eeprom
        EEPromWrite(PRIMARY_PORT_BAUDRATE + portidx,
                    static_cast<uint16_t>(tvgdb.uart_port[portidx].BaudRate));
      }
      return result;
    } else
    {
      return false;
    }
  }
  Communication::SerialPort getPortSettings(int idx) const
  {
    if (idx >= 0 && idx < kUartCount)
      return tvgdb.uart_port[idx];
    else
    {
      static const Communication::SerialPort dummy;
      return dummy;
    }
  }
  template <typename Port>
  Communication::SerialPort getPortSettings() const
  {
    if constexpr (std::is_same_v<Port, uart_primary_sensor>)
    {
      return tvgdb.uart_port[0];
    } else if constexpr (std::is_same_v<Port, uart_secondary_sensor>)
    {
      return tvgdb.uart_port[1];
    } else if constexpr (std::is_same_v<Port, uart_PC>)
    {
      return tvgdb.uart_port[2];
    } else
    {
      // Default implementation or error handling
      static_assert(std::is_same<Port, void>::value, "Unsupported port type");
    }
  }

  template <typename Port>
  void setPortSettings(const db::Communication::SerialPort &port)
  {
    if constexpr (std::is_same_v<Port, uart_primary_sensor>)
    {
      tvgdb.uart_port[0].BaudRate.setValue(port.BaudRate);
    } else if constexpr (std::is_same_v<Port, uart_secondary_sensor>)
    {
      tvgdb.uart_port[1].BaudRate.setValue(port.BaudRate);
    } else if constexpr (std::is_same_v<Port, uart_PC>)
    {
      tvgdb.uart_port[2].BaudRate.setValue(port.BaudRate);
    } else
    {
      // Default implementation or error handling
      static_assert(std::is_same<Port, void>::value, "Unsupported port type");
    }
  }

  const SensorData &getSensorData(int idx) const
  {
    if (idx >= 0 && idx < kSensorCount)
      return tvgdb.sensor_data[idx];
    else
    {
      static const SensorData dummy;
      return dummy;
    }
  }

  static TvgDatabase &getInstance()
  {
    static TvgDatabase s;
    return s;
  }

  void dumpModel();
  void dumpPorts();
  void printPortSettings(const char *portName,
                         const Communication::SerialPort &port);
  void formatValueInBuffer(char *buf, const DataDef &dataDef, int value);
  void formatSignedValueInBuffer(char *buf, const DataDef &dataDef, int value);
  template <typename protocol>
  void GetValues(char *buf);

private:
  TvgDb tvgdb;
  static osMutexId_t mutex;
};
} // namespace db