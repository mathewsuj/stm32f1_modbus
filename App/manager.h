#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    void initializeManager(osThreadAttr_t thread_attr);
    void sensorRxdData(int id, uint8_t *buf, int size);

#ifdef __cplusplus
}
#endif