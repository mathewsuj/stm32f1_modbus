#include "console.h"
#include "cmsis_os.h"
#include "manager.h"
// #include "task.h"

extern "C" void StartDefaultTask(void *argument)
{
  osThreadAttr_t thread_attr_console = {
      .name = "consoleTask",
      .stack_size = 128 * 4,
      .priority = (osPriority_t)osPriorityNormal,
  };
  osThreadAttr_t thread_attr_manager = {
      .name = "managerTask",
      .stack_size = 128 * 5,
      .priority = (osPriority_t)osPriorityNormal,
  };

  initializeLogger(thread_attr_console);

  initializeManager(thread_attr_manager);

  // UBaseType_t uxHighWaterMark;
  // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

  //  logMessage("gaugeremote ver 1.0\r\n", 0);

  /* Infinite loop */
  for (;;)
  {
    int status = 1;
    //   HAL_GPIO_TogglePin(USER_LED1_GPIO_Port, USER_LED1_Pin);
    debugLog("gaugeremote ver %d\r\n", status);
    osDelay(1000);
    // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
  }
}