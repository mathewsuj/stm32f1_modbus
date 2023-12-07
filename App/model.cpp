#include <cstdint>
#include "model.h"
#include "console.h"

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