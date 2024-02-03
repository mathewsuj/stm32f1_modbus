#include "menu_manager.h"
#include "c_intf.h"
#include "cmsis_os.h"
#include "console.h"

#define debug
#ifdef debug
#include "task.h"
#endif

osThreadId_t thr_menumgr;

void MenuMgrThread(void *argument)
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
    uint16_t val = getDbValue(PRIMARY_SENSOR_MEAN_VALUE);
    debugLog("sensor: mean value: %d\r\n", val);
    setDbValue(PRIMARY_SENSOR_MEAN_VALUE, val + 1);
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
void initializeMenuManager()
{

  const osThreadAttr_t thread_attr_task = {
      .name = "menuMgrTask",
      .stack_size = 128 * 9,
      .priority = (osPriority_t)osPriorityNormal,
  };

  // Create the  thread
  thr_menumgr = osThreadNew(MenuMgrThread, NULL, &thread_attr_task);
}