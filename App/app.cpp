#include "console.h"
#include "cmsis_os.h"
#include "manager.h"
#define debug
#ifdef debug
#include "task.h"
#endif

extern "C" void StartDefaultTask(void *argument)
{

  osThreadAttr_t thread_attr_consoleRx = {
      .name = "consoleRxTask",
      .stack_size = 128 * 7,
      .priority = (osPriority_t)osPriorityNormal,
  };

  osThreadAttr_t thread_attr_manager = {
      .name = "managerTask",
      .stack_size = 128 * 5,
      .priority = (osPriority_t)osPriorityNormal,
  };

  initializeLogger(thread_attr_consoleRx);

  initializeManager(thread_attr_manager);

#ifdef debug
  UBaseType_t uxHighWaterMark;
  uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
#endif

  //  logMessage("gaugeremote ver 1.0\r\n", 0);

  /* Infinite loop */
  for (;;)
  {
    int status = 1;
    //   HAL_GPIO_TogglePin(USER_LED1_GPIO_Port, USER_LED1_Pin);
    for (int i = 0; i < 20; i++)
      debugLog("gaugeremote ver %d %d\r\n", status, i);
    osDelay(1000);
#ifdef debug
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
#endif
  }
}