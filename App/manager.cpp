#include <array>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

#include "cmsis_os.h"

#include "console.h"
#include "manager.h"
#include "model.h"
#include "protocolbase.h"
#include "uart_device.h"

#define debug
#ifdef debug
#include "task.h"
#endif

extern UART_HandleTypeDef huart1, huart2, huart3;
// uart port configuration
UartDevice<uart_primary_sensor, char> port_primary_sensor(&huart2, 1);
// uart_device<uart_secondary_sensor> port_secondary_sensor;
UartDevice<uart_debug_port, char> port_debug(&huart3, 1);
UartDevice<uart_PC, char> port_PC(&huart1, 20);
//-------------------------------------------------------------------------------
osThreadId_t MgrThreadID;

template <typename T>
void processData();

template <>
void processData<uart_primary_sensor>()
{
  port_primary_sensor.SendRequestPacket(302);
  osDelay(500);
  if (const char *inp = port_primary_sensor.GetNextFrame(); inp)
  {

    debugLog("New packet from gauge received\n\r");
    db::SensorData newConfig;
    port_primary_sensor.UpdateModel(inp, newConfig);
    auto &sensor_data = db::TvgDatabase::getInstance();
    sensor_data.updateSensorData(0, newConfig);
  }
}

template <>
void processData<uart_PC>()
{
  if (const char *inp = port_PC.GetNextFrame(); inp)
  {

    auto id = atoi(inp + 1);
    debugLog("New packet from PC received\n\r");
    char buf[100];
    auto &sensor_data = db::TvgDatabase::getInstance();
    sensor_data.GetValues<sc400>(buf);
    port_PC.SendResponsePacket(id, buf);
  }
}

template <>
void processData<uart_secondary_sensor>()
{
  osDelay(500);
}

void managerThread(void *argument)
{
  (void)argument;
  auto &sensor_data = db::TvgDatabase::getInstance();

  port_primary_sensor.Init(sensor_data.getPortSettings<uart_primary_sensor>());
  port_debug.Init({});

  // port_secondary_sensor.Init(sensor_data.getPortSettings<uart_secondary_sensor>());

  port_PC.Init(sensor_data.getPortSettings<uart_PC>());
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
    // get data from gauge
    processData<uart_primary_sensor>();

    // response to query from PC
    processData<uart_PC>();

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
void uartSetPortConfiguration(const uint8_t port,
                              const db::Communication::SerialPort &setting)
{
  auto &sensor_data = db::TvgDatabase::getInstance();
  switch (port)
  {
  case 1:
    sensor_data.setPortSettings<uart_primary_sensor>(setting);
    break;
  case 2:
    sensor_data.setPortSettings<uart_secondary_sensor>(setting);
    break;
  case 3:
    sensor_data.setPortSettings<uart_PC>(setting);
    break;
  default:
    // Handle invalid port number (optional)
    break;
  }
}

void consolePutchar(const char data)
{
  if (data != '\0')
    port_debug.SendByte(&data, 1);
}

void consolePutstr(const char *data, const std::size_t size)
{
  if ((size > 0) && (*data != '\0'))
    port_debug.SendByte(data, size);
}

char *consoleGetcommand()
{
  return port_debug.GetString('\r');
}

void consoleDumpmodel()
{
  auto &sensor_data = db::TvgDatabase::getInstance();
  sensor_data.dumpModel();
}

void consoleDumpports()
{
  auto &sensor_data = db::TvgDatabase::getInstance();
  sensor_data.dumpPorts();
}

void initializeManager()
{
  osThreadAttr_t thread_attr_task;
  thread_attr_task.name = "managerTask";
  thread_attr_task.stack_size = 128 * 9;
  thread_attr_task.priority = (osPriority_t)osPriorityNormal;
  // Create the manager thread
  MgrThreadID = osThreadNew(managerThread, NULL, &thread_attr_task);
}

extern "C" void sensorRxdData(const UART_HandleTypeDef *huart)
{
  if (huart == port_primary_sensor.GetUartHandle())
    port_primary_sensor.DataRdy();
  else if (huart == port_debug.GetUartHandle())
    port_debug.DataRdy();
  else if (huart == port_PC.GetUartHandle())
    port_PC.DataRdy();
}