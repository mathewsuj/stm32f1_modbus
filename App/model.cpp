#include "model.h"
#include "console.h"
#include "manager.h"
#include <cmath>
#include <cstdint>
#include <cstdio>

using namespace db;

// osMutexId_t TvgDatabase::mutex = osMutexNew(NULL);

uint16_t EEpromLocs[NO_EEPROM_LOC];

void TvgDatabase::dumpModel()
{
  int idx = 0;
  debugLog("Gauge Data Dump:\r\n");
  debugLog("Status Mean Value: %d\r\n",
           tvgdb.sensor_data[idx].status_mean_value.getValue());
  debugLog("Mean Value: %d\r\n", tvgdb.sensor_data[idx].mean_value.getValue());
  debugLog("Deviation: %d\r\n", tvgdb.sensor_data[idx].deviation.getValue());
  debugLog("Blue Diameter:  %d\r\n",
           tvgdb.sensor_data[idx].blue_diameter.getValue());
  debugLog("Magenta Diameter:  %d\r\n",
           tvgdb.sensor_data[idx].magenta_diameter.getValue());
  debugLog("Ovality:  %d\r\n", tvgdb.sensor_data[idx].ovality.getValue());
  debugLog("Position Axis Blue:  %d\r\n",
           tvgdb.sensor_data[idx].position_axis_blue.getValue());
  debugLog("Position Axis Magenta:  %d\r\n",
           tvgdb.sensor_data[idx].position_axis_magenta.getValue());
}
void TvgDatabase::printPortSettings(const char *portName,
                                    const Communication::SerialPort &port)
{
  int baudrate = port.BaudRate;
  int parity =
      static_cast<int>(static_cast<Communication::ParityType>(port.Parity));
  int stopbits = port.StopBits;
  debugLog("%s: %d, %d, %d\r\n", portName, baudrate, parity, stopbits);
}
void TvgDatabase::dumpPorts()
{
  debugLog("Port Settings:\r\n");
  printPortSettings("Primary Sensor Port", tvgdb.uart_port[0]);
  printPortSettings("Secondary Sensor Port", tvgdb.uart_port[1]);
  printPortSettings("PC Port", tvgdb.uart_port[2]);
}
void TvgDatabase::formatValueInBuffer(char *buf, const DataDef &dataDef,
                                      int value)
{
  char format[5] = "%05d";
  if (dataDef.size != 5)
    format[2] = '0' + dataDef.size;
  std::snprintf(buf + dataDef.pos, dataDef.size + 1, format, value);
}

template <>
void TvgDatabase::GetValues<sc400>(char *buf)
{
  using namespace sikora;

  int idx = 0;

  *(buf + StatusMeanValue.pos) = tvgdb.sensor_data[idx].status_mean_value + '0';
  formatValueInBuffer(buf, MeanValue, tvgdb.sensor_data[idx].mean_value);

  *(buf + SignDeviation.pos) =
      (tvgdb.sensor_data[idx].deviation > 0) ? '+' : '-';
  formatValueInBuffer(buf, Deviation, tvgdb.sensor_data[idx].deviation);

  formatValueInBuffer(buf, BlueDiameter, tvgdb.sensor_data[idx].blue_diameter);
  formatValueInBuffer(buf, MagentaDiameter,
                      tvgdb.sensor_data[idx].magenta_diameter);
  formatValueInBuffer(buf, Ovality, tvgdb.sensor_data[idx].ovality);

  *(buf + SignPositionAxisBlue.pos) =
      (tvgdb.sensor_data[idx].position_axis_blue > 0) ? '+' : '-';
  formatValueInBuffer(buf, PositionAxisBlue,
                      tvgdb.sensor_data[idx].position_axis_blue);

  *(buf + SignPositionAxisMagenta.pos) =
      (tvgdb.sensor_data[idx].position_axis_magenta > 0) ? '+' : '-';
  formatValueInBuffer(buf, PositionAxisMagenta,
                      tvgdb.sensor_data[idx].position_axis_magenta);
}
