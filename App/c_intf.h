#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t getDbValue(uint16_t adr);
uint16_t setDbValue(uint16_t adr, const uint16_t value);

#ifdef __cplusplus
}
#endif