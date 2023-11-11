#include <cstdint>
#include <array>
#include <string>
#include <cstdarg>
#include "cmsis_os.h"
#include "main.h"
#include "console.h"
#include "circualrbuf.h"

static constexpr uint8_t LOG_SEGMENT_SIZE = 10;
char log_segment_buffer[LOG_SEGMENT_SIZE];
static size_t log_segment_size{0};

osMutexId_t logMutex, logFormatMutex;
osThreadId_t logThreadID;

static CircularBuffer<char, 10> keyInp;
static CircularBuffer<char, 100> log_buffer;

void debugLog(const char *format, ...)
{
    char buffer[256];
    std::va_list args;
    va_start(args, format);

    osMutexAcquire(logFormatMutex, osWaitForever);

    vsnprintf(buffer, 256, format, args);
    logMessage(buffer);

    osMutexRelease(logFormatMutex);
    va_end(args);
}

void logMessage(const char *message)
{
    auto millis = osKernelGetTickCount();
    std::string s = std::to_string(millis) + ": " + message;

    for (const auto &c : s)
    {
        log_segment_buffer[log_segment_size++] = c;

        if (log_segment_size == LOG_SEGMENT_SIZE || c == '\0')
        {
            log_buffer.write(log_segment_buffer, log_segment_size);

            log_segment_size = 0;
            osMutexRelease(logMutex);
        }
    }
}
static void processCommand(const std::string &str)
{
    if (str == "help")
    {
        logMessage("\n\rcommands:\n\r\thelp\n\r");
    }
    else
    {
        logMessage("\n\rcommand not found!\n\r");
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