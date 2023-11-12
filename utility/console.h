#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    void initializeLogger();
    void debugLog(const char *format, ...);
    void logMessage(const char *message, int timestamp_enabled);
    void consoleKeyRxd(char c);

#ifdef __cplusplus
}
#endif