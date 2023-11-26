#include <cstdint>
#include <array>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <string>

#include "hal.h"
#include "console.h"
#include "circualrbuf.h"
#include "model.h"
// #include "commands.h"

namespace
{

    constexpr size_t LOG_SIZE = 100;
    CircularBuffer<char, 15> keyInp;
    CircularBuffer<char, LOG_SIZE> log_buffer;

}

osMutexId_t logMutex, logFormatMutex;
osThreadId_t logThreadID;

// StringLookup CommandList;

void debugLog(const char *format, ...)
{
    char buffer[LOG_SIZE];
    std::va_list args;
    va_start(args, format);

    const auto status = osMutexAcquire(logFormatMutex, osWaitForever);
    if (status != osOK)
        return;

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

    osMutexAcquire(logMutex, osWaitForever);
    static auto owner = osMutexGetOwner(logMutex);
    log_buffer.write(s, std::strlen(s));
    osMutexRelease(logMutex);
    owner = {0};
}
static void processCommand(const std::string &str)
{
    if (str == "help")
    {
        logMessage("\n\rcommands:\n\r\thelp\n\r", false);
    }
    else if (str == "dump model")
    {
        dumpModel();
    }
    else
    {

        logMessage("\n\r command not found!\n\r", false);
    }
}
void consoleThread(void *argument)
{
    (void)argument;
    console_rx_init();

    while (1)
    {
        osMutexAcquire(logMutex, osWaitForever);

        // Log a character from the buffer, e.g., send it to a UART
        console_putchar(log_buffer.read());

        osMutexRelease(logMutex);
        if (keyInp.isCommandFound())
        {
            processCommand(keyInp.getCommand());
        }
    }
}

extern "C" void consoleKeyRxd(char c)
{
    keyInp.write(c);
}

void initializeLogger(osThreadAttr_t thread_attr)
{
    // Create a mutex to protect the log buffer
    osMutexAttr_t mutex_attr = {0};
    logMutex = osMutexNew(&mutex_attr);
    logFormatMutex = osMutexNew(&mutex_attr);

    // Create the console thread

    logThreadID = osThreadNew(consoleThread, NULL, &thread_attr);
}
