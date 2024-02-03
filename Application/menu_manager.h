#pragma once
#include "eeprom_def.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void initializeMenuManager();
uint16_t getDbValue(uint16_t idx);

#ifdef __cplusplus
}
#endif