#include <array>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

#include "cmsis_os.h"

#include "circularbuf.h"
#include "cli.h"
#include "console.h"
#include "manager.h"
#include "model.h"

#define debug
#ifdef debug
#include "task.h"
#endif

namespace
{
  constexpr size_t LOG_SIZE = 100;
  const char BACKSPACE = 127;
} // namespace

osMutexId_t logFormatMutex;
osThreadId_t thr_console_rx;

utility::Cli cli;

extern "C" void debugLog(const char *format, ...)
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

void logMessage(const char *message, const int timestamp_enabled)
{
  char s[LOG_SIZE] = {'\0'};
  if (timestamp_enabled)
  {
    uint32_t millis = osKernelGetTickCount();
    std::sprintf(s, "%ld: ", millis);
  }
  std::strcat(s, message);

  consolePutstr(&s[0], std::strlen(s));
}
void cleanup_string(char *str)
{
  int idx = 0;
  int newIdx = 0;
  while (str[idx])
  {
    if (str[idx] == BACKSPACE)
    {
      newIdx -= 2;
    } else
    {
      if (idx != newIdx)
        str[newIdx] = str[idx];
    }
    newIdx++;
    if (newIdx < 0)
      newIdx = 0;
    idx++;
  }
  str[newIdx] = '\0';
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
    if (char *inp = consoleGetcommand(); inp)
    {
      cleanup_string(inp);
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
void initializeLogger()
{

  // osMutexAttr_t mutex_attr = {0};
  logFormatMutex = osMutexNew(NULL);

  osThreadAttr_t thread_attr_task{};
  thread_attr_task.name = "consoleTask";
  thread_attr_task.stack_size = 128 * 9;
  thread_attr_task.priority = (osPriority_t)osPriorityNormal;

  // Create the console thread
  thr_console_rx = osThreadNew(consoleThreadRx, NULL, &thread_attr_task);
}
