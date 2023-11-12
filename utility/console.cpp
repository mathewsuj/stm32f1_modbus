#include <cstdint>
#include <array>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <string>
#include "cmsis_os.h"
#include "main.h"
#include "console.h"
#include "circualrbuf.h"

static constexpr size_t LOG_SIZE = 100;

osMutexId_t logMutex, logFormatMutex;
osThreadId_t logThreadID;

static CircularBuffer<char, 10> keyInp;
static CircularBuffer<char, LOG_SIZE> log_buffer;

void debugLog(const char *format, ...)
{
    char buffer[LOG_SIZE];
    std::va_list args;
    va_start(args, format);

    osMutexAcquire(logFormatMutex, osWaitForever);

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
    log_buffer.write(s, std::strlen(s));
    osMutexRelease(logMutex);
}
static void processCommand(const std::string &str)
{
    if (str == "help")
    {
        logMessage("\n\rcommands:\n\r\thelp\n\r", false);
    }
    else
    {
        logMessage("\n\rcommand not found!\n\r", false);
    }
}
void consoleThread(void *argument)
{
    (void)argument;

    while (1)
    {
        osMutexAcquire(logMutex, osWaitForever);

        // Log a character from the buffer, e.g., send it to a UART
        console_msg(log_buffer.read());

        osMutexRelease(logMutex);
        if (keyInp.isCommandFound())
        {
            processCommand(keyInp.getCommand());
        }
    }
}

void consoleKeyRxd(char c)
{
    keyInp.write(c);
}

void initializeLogger()
{
    // Create a mutex to protect the log buffer
    osMutexAttr_t mutex_attr = {0};
    logMutex = osMutexNew(&mutex_attr);
    logFormatMutex = osMutexNew(&mutex_attr);

    // Create the console thread
    osThreadAttr_t thread_attr = {0};
    thread_attr.name = "console";
    logThreadID = osThreadNew(consoleThread, NULL, &thread_attr);
}
