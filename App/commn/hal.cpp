
#include "eeprom.h"
#include "manager.h"
#include "stm32f1xx_hal.h"

using namespace db;

uint16_t VirtAddVarTab[] = {PRIMARY_SENSOR_STATUS_MEAN_VALUE,
                            PRIMARY_SENSOR_MEAN_VALUE,
                            PRIMARY_SENSOR_DEVIATION,
                            PRIMARY_SENSOR_BLUE_DIAMETER,
                            PRIMARY_SENSOR_MAGENTA_DIAMETER,
                            PRIMARY_SENSOR_OVALITY,
                            PRIMARY_SENSOR_POSITION_AXIS_BLUE,
                            PRIMARY_SENSOR_POSITION_AXIS_MAGENTA,
                            SECONDARY_SENSOR_STATUS_MEAN_VALUE,
                            SECONDARY_SENSOR_MEAN_VALUE,
                            SECONDARY_SENSOR_DEVIATION,
                            SECONDARY_SENSOR_BLUE_DIAMETER,
                            SECONDARY_SENSOR_MAGENTA_DIAMETER,
                            SECONDARY_SENSOR_OVALITY,
                            SECONDARY_SENSOR_POSITION_AXIS_BLUE,
                            SECONDARY_SENSOR_POSITION_AXIS_MAGENTA,
                            PRIMARY_PORT_BAUDRATE,
                            SECONDARY_PORT_BAUDRATE,
                            PC_PORT_BAUDRATE};

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  sensorRxdData(huart);
}
void EEPromWrite(uint16_t adr, uint16_t value)
{
  EE_WriteVariable(adr, value);
}
uint16_t EEPromRead(uint16_t adr)
{
  uint16_t value;
  EE_ReadVariable(adr, &value);
  return value;
}
