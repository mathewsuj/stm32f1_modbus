#include <cstdint>
#include <cstdio>
#include <cmath>
#include "model.h"
#include "console.h"
#include "manager.h"

void SensorData::dumpModel()
{
    debugLog("Gauge Data Dump:\r\n");
    debugLog("gauge data mean value: %d\r\n", m_gauge_data.status_mean_value);
    debugLog("Status Mean Value: %d\r\n", m_gauge_data.status_mean_value);
    debugLog("Mean Value: %d\r\n", m_gauge_data.mean_value);
    debugLog("Deviation: %d\r\n", m_gauge_data.deviation);
    debugLog("Blue Diameter:  %d\r\n", m_gauge_data.blue_diameter);
    debugLog("Magenta Diameter:  %d\r\n", m_gauge_data.magenta_diameter);
    debugLog("Ovality:  %d\r\n", m_gauge_data.ovality);
    debugLog("Position Axis Blue:  %d\r\n", m_gauge_data.position_axis_blue);
    debugLog("Position Axis Magenta:  %d\r\n", m_gauge_data.position_axis_magenta);
}
void SensorData::printPortSettings(const char *portName, const Communication::SerialPort &port)
{
    debugLog("%s: %d, %d, %d\r\n", portName, port.BaudRate, static_cast<int>(port.Parity), port.StopBits);
}
void SensorData::dumpPorts()
{
    debugLog("Port Settings:\r\n");
    printPortSettings("Primary Sensor Port", m_settings.PrimaryPort);
    printPortSettings("Secondary Sensor Port", m_settings.SecondaryPort);
    printPortSettings("PC Port", m_settings.PCPort);
}
void SensorData::formatValueInBuffer(char *buf, const DataDef &dataDef, int value)
{
    char format[5] = "%05d";
    if (dataDef.size != 5)
        format[2] = '0' + dataDef.size;
    std::snprintf(buf + dataDef.pos, dataDef.size + 1, format, value);
}

template <>
void SensorData::GetValues<sc400>(char *buf)
{
    using namespace sikora;

    *(buf + StatusMeanValue.pos) = m_gauge_data.status_mean_value + '0';
    formatValueInBuffer(buf, MeanValue, m_gauge_data.mean_value);

    *(buf + SignDeviation.pos) = (m_gauge_data.deviation > 0) ? '+' : '-';
    formatValueInBuffer(buf, Deviation, m_gauge_data.deviation);

    formatValueInBuffer(buf, BlueDiameter, m_gauge_data.blue_diameter);
    formatValueInBuffer(buf, MagentaDiameter, m_gauge_data.magenta_diameter);
    formatValueInBuffer(buf, Ovality, m_gauge_data.ovality);

    *(buf + SignPositionAxisBlue.pos) = (m_gauge_data.position_axis_blue > 0) ? '+' : '-';
    formatValueInBuffer(buf, PositionAxisBlue, m_gauge_data.position_axis_blue);

    *(buf + SignPositionAxisMagenta.pos) = (m_gauge_data.position_axis_magenta > 0) ? '+' : '-';
    formatValueInBuffer(buf, PositionAxisMagenta, m_gauge_data.position_axis_magenta);
}
