#pragma once
#include <cstdint>
#ifdef __cplusplus
extern "C"
{
#endif

    struct Configurations
    {
        uint8_t status_mean_value = 0;
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
    void dumpModel();
    void model_init();

#ifdef __cplusplus
}
#endif
