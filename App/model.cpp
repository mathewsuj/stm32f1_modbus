#include <cstdint>
#include "model.h"
#include "console.h"

uint8_t uart_rxbus[100];
Configurations gauge_data;

void model_init()
{
    gauge_data = {};
}
void dumpModel()
{
    //  logMessage("gauge parameters\r\n", false);
    debugLog("gauge data mean value: %d\r\n", gauge_data.status_mean_value);
}