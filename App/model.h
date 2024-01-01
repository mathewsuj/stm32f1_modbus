#pragma once
#include <cstdint>
#include <cstddef>
#include <type_traits>

struct uart_debug_port
{
};
struct uart_primary_sensor
{
};
struct uart_secondary_sensor
{
};
struct uart_PC
{
};

struct sc400
{
};

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
namespace Communication
{
    enum class ParityType
    {
        Even,
        Odd,
        None
    };
    struct SerialPort
    {
        uint16_t BaudRate;
        ParityType Parity;
        uint8_t StopBits;
    };
    struct Settings
    {
        SerialPort PrimaryPort;
        SerialPort SecondaryPort;
        SerialPort PCPort;
    };
}

class SensorData
{
public:
    using ParityType = Communication::ParityType;

    SensorData() : m_gauge_data{}, m_settings{{19200, ParityType::None, 1}, {19200, ParityType::None, 1}, {19200, ParityType::None, 1}} {}
    void updateData(const Configurations &newData)
    {

        m_gauge_data = newData;
    }
    void dumpModel();
    void dumpPorts();
    void printPortSettings(const char *portName, const Communication::SerialPort &port);
    void formatValueInBuffer(char *buf, const DataDef &dataDef, int value);
    void formatSignedValueInBuffer(char *buf, const DataDef &dataDef, int value);
    template <typename protocol>
    void GetValues(char *buf);

    template <typename Port>
    Communication::SerialPort getPortSettings() const
    {
        if constexpr (std::is_same_v<Port, uart_primary_sensor>)
        {
            return m_settings.PrimaryPort;
        }
        else if constexpr (std::is_same_v<Port, uart_secondary_sensor>)
        {
            return m_settings.SecondaryPort;
        }
        else if constexpr (std::is_same_v<Port, uart_PC>)
        {
            return m_settings.PCPort;
        }
        else
        {
            // Default implementation or error handling
            static_assert(std::is_same<Port, void>::value, "Unsupported port type");
        }
    }
    template <typename Port>
    void setPortSettings(const Communication::SerialPort &port)
    {
        if constexpr (std::is_same_v<Port, uart_primary_sensor>)
        {
            m_settings.PrimaryPort = port;
        }
        else if constexpr (std::is_same_v<Port, uart_secondary_sensor>)
        {
            m_settings.SecondaryPort = port;
        }
        else if constexpr (std::is_same_v<Port, uart_PC>)
        {
            m_settings.PCPort = port;
        }
        else
        {
            // Default implementation or error handling
            static_assert(std::is_same<Port, void>::value, "Unsupported port type");
        }
    }

private:
    Configurations m_gauge_data;
    Communication::Settings m_settings;
};
