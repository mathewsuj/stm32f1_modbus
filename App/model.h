#pragma once
#include "cmsis_os.h"
#include "eeprom_def.h"
#include "hal.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <tuple>
#include <type_traits>

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
  bool changed;

public:
  Data(T defaultValue, const T minVal, T maxVal)
      : value(defaultValue), min(minVal), max(maxVal), changed(false)
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
  bool isChanged()
  {
    return changed;
  }
  void flush()
  {
    changed = false;
  }
};

struct SensorData {

  Data<uint8_t> status_mean_value{0, 0, 1};
  Data<uint16_t> mean_value{6016, 0, 6100};
  Data<int16_t> deviation{0, 4000, 1};
  Data<uint16_t> blue_diameter{0, 0, 1};
  Data<uint16_t> magenta_diameter{0, 0, 1};
  Data<uint16_t> ovality{0, 0, 1};
  Data<int16_t> position_axis_blue{0, 0, 1};
  Data<int16_t> position_axis_magenta{0, 0, 1};
};

constexpr int kDbStart = 4;
struct DataDef {
  std::size_t pos;
  std::size_t size;
};
namespace sikora
{
constexpr DataDef StatusMeanValue{kDbStart, 1};
constexpr DataDef MeanValue{kDbStart + 1, 5};
constexpr DataDef SignDeviation{kDbStart + 6, 1};
constexpr DataDef Deviation{kDbStart + 7, 5};
constexpr DataDef BlueDiameter{kDbStart + 12, 5};
constexpr DataDef MagentaDiameter{kDbStart + 17, 5};
constexpr DataDef Ovality{kDbStart + 22, 5};
constexpr DataDef SignPositionAxisBlue{kDbStart + 27, 1};
constexpr DataDef PositionAxisBlue{kDbStart + 28, 5};
constexpr DataDef SignPositionAxisMagenta{kDbStart + 33, 1};
constexpr DataDef PositionAxisMagenta{kDbStart + 34, 5};
constexpr DataDef Sc400DataEnd{kDbStart + 39, 0};
} // namespace sikora
namespace Communication
{
enum class ParityType : uint8_t { Even, Odd, None };
struct SerialPort {
  Data<uint16_t> BaudRate{19200, 9600, 48200};
  Data<ParityType> Parity{ParityType::Even, ParityType::Even, ParityType::Odd};
  Data<uint8_t> StopBits{1, 1, 2};
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
  TvgDatabase(const SensorData &) = delete;
  TvgDatabase &operator=(const SensorData &) = delete;

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
    x.setValue(EEPromRead(adr));
  }
  template <typename T>
  void setValueInEEPROM(Data<T> &x, uint8_t adr, const T value)
  {
    x.setValue(value);
    if (x.isChanged())
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
    osMutexAcquire(mutex, osWaitForever);
    static TvgDatabase s;
    osMutexRelease(mutex);
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