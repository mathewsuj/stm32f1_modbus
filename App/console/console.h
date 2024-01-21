#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    void initializeLogger();
    void debugLog(const char *format, ...);
    void logMessage(const char *message, const int timestamp_enabled);

#ifdef __cplusplus
}
#endif