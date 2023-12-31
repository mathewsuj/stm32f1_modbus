#include <cstdint>
#include <array>
#include <string_view>
#include "cmsis_os.h"
#include "main.h"
#include "logger.h"

const uint8_t LOG_BUFFER_SIZE = 128;
const uint8_t LOG_SEGMENT_SIZE = 10;
std::array<uint8_t, LOG_BUFFER_SIZE> log_buffer;
std::array<uint8_t, LOG_SEGMENT_SIZE> log_segment_buffer;

static size_t log_segment_size = 0;

static size_t logBufferHead = 0;
static size_t logBufferTail = 0;

osMutexId_t logMutex;
osThreadId_t logThreadID;

void logMessage(const char *message)
{
    const std::string_view messageView(message);
    for (const char c : messageView)
    {
        log_segment_buffer[log_segment_size++] = c;

        if (log_segment_size == LOG_SEGMENT_SIZE || c == '\n')
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
    }
}

void loggerThread(void *argument)
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
    }
}
void initializeLogger()
{
    // Create a mutex to protect the log buffer
    osMutexAttr_t mutex_attr = {0};
    logMutex = osMutexNew(&mutex_attr);

    // Create the logger thread
    osThreadAttr_t thread_attr = {0};
    thread_attr.name = "Logger";
    logThreadID = osThreadNew(loggerThread, NULL, &thread_attr);
}