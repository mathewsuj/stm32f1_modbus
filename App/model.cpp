#include <cstdint>
#include <cstdio>
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
void SensorData::GetValues(int type, char *buf)
{
    using namespace sikora;
    int index = 1;
    *(buf + StatusMeanValue.pos) = m_gauge_data.status_mean_value + '0';
    std::snprintf(buf + MeanValue.pos, 6, "%05d", m_gauge_data.mean_value);

    *(buf + SignDeviation.pos) = (m_gauge_data.deviation > 0) ? '+' : '-';
    std::snprintf(buf + Deviation.pos, 6, "%05d", m_gauge_data.deviation);

    std::snprintf(buf + BlueDiameter.pos, 6, "%05d", m_gauge_data.blue_diameter);
    std::snprintf(buf + MagentaDiameter.pos, 6, "%05d", m_gauge_data.magenta_diameter);
    std::snprintf(buf + Ovality.pos, 6, "%05d", m_gauge_data.ovality);

    *(buf + SignPositionAxisBlue.pos) = (m_gauge_data.position_axis_blue > 0) ? '+' : '-';
    std::snprintf(buf + PositionAxisBlue.pos, 6, "%05d", m_gauge_data.position_axis_blue);

    *(buf + SignPositioAxisMagenta.pos) = (m_gauge_data.position_axis_magenta > 0) ? '+' : '-';
    std::snprintf(buf + PositionAxisMagenta.pos, 6, "%05d", m_gauge_data.position_axis_magenta);
}