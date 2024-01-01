#include "console.h"
#include "cmsis_os.h"
#include "manager.h"

#define debug
#ifdef debug
#include "task.h"
#endif

extern "C" void StartDefaultTask(void *argument)
{
  (void)argument;

  initializeLogger();

  initializeManager();

  osDelay(100);
  logMessage("TVG CON 0.10v\r\n", 0);

#ifdef debug
  UBaseType_t uxHighWaterMark, minWaterMark;
  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  minWaterMark = uxHighWaterMark;
  const char *taskName = osThreadGetName(osThreadGetId());
  debugLog("HighWaterMark - %s: %ld\r\n", taskName, minWaterMark);
#endif

  /* Infinite loop */
  for (;;)
  {
    osDelay(1000);
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