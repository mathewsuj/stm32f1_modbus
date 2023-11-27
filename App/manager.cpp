#include <cstdint>
#include <array>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <string>

#include "cmsis_os.h"
#include "manager.h"
#include "model.h"
#include "hal.h"
#include "circualrbuf.h"
namespace
{

    constexpr size_t LOG_SIZE = 100;
    // clang-format off
    struct primary_sensor {};
    struct secondary_sensor {};
    // clang-format on
    template <typename type>
    using rx_buffer = CircularBuffer<uint8_t, LOG_SIZE>;

    rx_buffer<primary_sensor> primary_sensor_buf;
    rx_buffer<secondary_sensor> secondary_sensor_buf;

}

osThreadId_t MgrThreadID;

void managerThread(void *argument)
{
    (void)argument;
    sensor_init(primary_port, 1);

    while (1)
    {
        osDelay(10);
    }
}

void initializeManager(osThreadAttr_t thread_attr)
{
    model_init();
    // Create the manager thread
    MgrThreadID = osThreadNew(managerThread, NULL, &thread_attr);
}
void sensorRxdData(int id, uint8_t *buf, int size)
{
    primary_sensor_buf.write(buf, size);
}