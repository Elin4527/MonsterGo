#ifndef WIRE_UTIL_H
#define WIRE_UTIL_H

#include <Wire.h>

void wireInit();
void wireWriteByte(int address, uint8_t value);
void wireWriteRegister(int address, uint8_t reg, uint8_t value);
void wireRequestArray(int address, uint32_t* buffer, uint8_t amount);

#endif
