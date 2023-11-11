#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    void initializeLogger();
    void debugLog(const char *format, ...);
    void logMessage(const char *message);
    void consoleKeyRxd(char c);

#ifdef __cplusplus
}
#endif