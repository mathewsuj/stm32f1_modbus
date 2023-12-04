#pragma once
#include <cstdint>
#include <cstddef>
#ifdef __cplusplus
extern "C"
{
#endif

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
    struct DataDef
    {
        std::size_t pos;
        std::size_t size;
    };

    constexpr DataDef StatusMeanValue = {0, 1};
    constexpr DataDef MeanValue = {1, 4};
    constexpr DataDef Deviation = {7, 4};
    constexpr DataDef BlueDiameter = {12, 4};
    constexpr DataDef MagentaDiameter = {17, 4};
    constexpr DataDef Ovality = {22, 4};
    constexpr DataDef PositionAxisBlue = {28, 4};
    constexpr DataDef PositionAxisMagenta = {34, 4};

    class SensorData
    {
    public:
        SensorData() : m_gauge_data{} {}
        void updateData(const Configurations &newData)
        {

            m_gauge_data = newData;
        }

    private:
        Configurations m_gauge_data;
    };

#ifdef __cplusplus
}
#endif
