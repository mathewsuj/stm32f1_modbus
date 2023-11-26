#pragma once
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void initializeLogger(osThreadAttr_t thread_attr);
    void debugLog(const char *format, ...);
    void logMessage(const char *message, int timestamp_enabled);
    void consoleKeyRxd(char c);

#ifdef __cplusplus
}
#endif