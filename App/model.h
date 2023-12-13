#pragma once
#include <cstdint>
#include <cstddef>

struct DataDef
{
    std::size_t pos;
    std::size_t size;
};
constexpr int db_start = 4;
struct Configurations
{
    uint8_t status_mean_value;
    uint16_t mean_value;
    uint8_t sign_deviation;
    int16_t deviation;
    uint16_t blue_diameter;
    uint16_t magenta_diameter;
    uint16_t ovality;
    uint8_t sign_position_axis_blue;
    int16_t position_axis_blue;
    uint8_t sign_position_axis_magenta;
    int16_t position_axis_magenta;
};
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
}

class SensorData
{
public:
    SensorData() : m_gauge_data{} {}
    void updateData(const Configurations &newData)
    {

        m_gauge_data = newData;
    }
    void dumpModel();
    template <typename protocol>
    void GetValues(int type, char *buf);

private:
    Configurations m_gauge_data;
};
