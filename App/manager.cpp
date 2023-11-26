#include "cmsis_os.h"
#include "manager.h"
#include "model.h"

osThreadId_t MgrThreadID;

void managerThread(void *argument)
{
    (void)argument;

    while (1)
    {
        osDelay(10);
    }
}

void initializeManager(osThreadAttr_t thread_attr)
{
    model_init();
    // Create the manager thread
    MgrThreadID = osThreadNew(managerThread, NULL, &thread_attr);
}
