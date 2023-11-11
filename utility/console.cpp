#include <cstdint>
#include <array>
#include <string>
#include <cstdarg>
#include "cmsis_os.h"
#include "main.h"
#include "console.h"
#include "circualrbuf.h"

const uint8_t LOG_BUFFER_SIZE = 128;
const uint8_t LOG_SEGMENT_SIZE = 10;
std::array<uint8_t, LOG_BUFFER_SIZE> log_buffer;
std::array<uint8_t, LOG_SEGMENT_SIZE> log_segment_buffer;

static size_t log_segment_size = 0;

static size_t logBufferHead = 0;
static size_t logBufferTail = 0;

osMutexId_t logMutex, logFormatMutex;
osThreadId_t logThreadID;

static CircularBuffer<10> keyInp;
static CircularBuffer<100> logBuf;

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
    std::string s = std::to_string(millis) + ": ";
    ;
    s += message;

    const char *c = s.c_str();
    do
    {
        log_segment_buffer[log_segment_size++] = *c;

        if (log_segment_size == LOG_SEGMENT_SIZE || *c == '\0')
        {
            osMutexAcquire(logMutex, osWaitForever);
            for (size_t i = 0; i < log_segment_size; i++)
            {
                log_buffer[logBufferHead] = log_segment_buffer[i];
                logBufferHead = (logBufferHead + 1) % LOG_BUFFER_SIZE;
            }
            log_segment_size = 0;
            osMutexRelease(logMutex);
        }

    } while (*c++ != '\0');
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
        if (logBufferHead != logBufferTail)
        {
            // Log a character from the buffer, e.g., send it to a UART or another output
            char logChar = log_buffer[logBufferTail];
            console_msg(logChar); // send char to console

            // Move the tail to the next position in the circular buffer
            logBufferTail = (logBufferTail + 1) % LOG_BUFFER_SIZE;
        }
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