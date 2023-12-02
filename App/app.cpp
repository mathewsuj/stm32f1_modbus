#include "console.h"
#include "cmsis_os.h"
#include "manager.h"

extern "C" void StartDefaultTask(void *argument)
{
  osThreadAttr_t thread_attr = {
      .name = "consoleTask",
      .stack_size = 128 * 6,
      .priority = (osPriority_t)osPriorityNormal,
  };
  initializeLogger(thread_attr);

  thread_attr.name = "manager";
  initializeManager(thread_attr);

  //  logMessage("gaugeremote ver 1.0\r\n", 0);

  /* Infinite loop */
  for (;;)
  {
    int status = 1;
    //   HAL_GPIO_TogglePin(USER_LED1_GPIO_Port, USER_LED1_Pin);
    debugLog("gaugeremote ver %d\r\n", status);
    osDelay(1000);
  }
}