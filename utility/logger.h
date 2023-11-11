#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    void initializeLogger();
    void debug_log(const char *format, ...);
    void logMessage(const char *message);

#ifdef __cplusplus
}
#endif