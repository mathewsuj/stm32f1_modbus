#pragma once
#include <cstdint>
#include <cstddef>

struct DataDef
{
    std::size_t pos;
    std::size_t size;
};
const int db_start = 4;
struct Configurations
{
    uint8_t status_mean_value;
    uint16_t mean_value;
    uint8_t sign_deviation;
    uint16_t deviation;
    uint16_t blue_diameter;
    uint16_t magenta_diameter;
    uint16_t ovality;
    uint8_t sign_position_axis_blue;
    uint16_t position_axis_blue;
    uint8_t sign_position_axis_magenta;
    uint16_t position_axis_magenta;
};
const DataDef StatusMeanValue{db_start, 1};
const DataDef MeanValue{db_start + 1, 5};
const DataDef SignDeviation{db_start + 6, 1};
const DataDef Deviation{db_start + 7, 5};
const DataDef BlueDiameter{db_start + 12, 5};
const DataDef MagentaDiameter{db_start + 17, 5};
const DataDef Ovality{db_start + 22, 5};
const DataDef SignPositioAxisBlue{db_start + 27, 1};
const DataDef PositioAxisBlue{db_start + 28, 5};
const DataDef SignPositioAxisMagenta{db_start + 33, 1};
const DataDef PositioAxisMagenta{db_start + 34, 5};

class SensorData
{
public:
    SensorData() : m_gauge_data{} {}
    void updateData(const Configurations &newData)
    {

        m_gauge_data = newData;
    }
    void dumpModel();

private:
    Configurations m_gauge_data;
};
