#include <cstdint>
#include <array>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <string>

#include "console.h"
#include "circularbuf.h"
#include "model.h"
#include "manager.h"
#include "cli.h"

#define debug
#ifdef debug
#include "task.h"
#endif

namespace
{
    constexpr size_t LOG_SIZE = 100;
}

osMutexId_t logFormatMutex;
osThreadId_t thr_console_rx;

utility::Cli cli;

void debugLog(const char *format, ...)
{
    char buffer[LOG_SIZE];
    std::va_list args;

    const auto status = osMutexAcquire(logFormatMutex, osWaitForever);
    if (status != osOK)
        return;

    va_start(args, format);
    std::vsnprintf(buffer, LOG_SIZE, format, args);

    logMessage(buffer, true);
    osMutexRelease(logFormatMutex);
    va_end(args);
}

void logMessage(const char *message, int timestamp_enabled)
{
    char s[LOG_SIZE] = {'\0'};
    if (timestamp_enabled)
    {
        auto millis = osKernelGetTickCount();
        std::sprintf(s, "%d: ", millis);
    }
    std::strcat(s, message);

    console_putstr(&s[0], std::strlen(s));
}

void consoleThreadRx(void *argument)
{
    (void)argument;
    osDelay(100);

#ifdef debug
    UBaseType_t uxHighWaterMark, minWaterMark;
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    minWaterMark = uxHighWaterMark;
    const char *taskName = osThreadGetName(osThreadGetId());
    debugLog("HighWaterMark - %s: %ld\r\n", taskName, minWaterMark);
#endif

    while (1)
    {
        if (const char *inp = console_getcommand(); inp)
        {
            if (*inp)
                cli.processCommand(inp);
        }
        osDelay(100);
#ifdef debug
        uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        if (minWaterMark > uxHighWaterMark)
        {
            minWaterMark = uxHighWaterMark;
            debugLog("HighWaterMark - %s: %ld\r\n", taskName, minWaterMark);
        }
#endif
    }
}
void initializeLogger(osThreadAttr_t thread_attr_rx)
{

    osMutexAttr_t mutex_attr = {0};
    logFormatMutex = osMutexNew(&mutex_attr);

    // Create the console thread
    thr_console_rx = osThreadNew(consoleThreadRx, NULL, &thread_attr_rx);
}
