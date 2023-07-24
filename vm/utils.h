#ifndef vm_utils_h
#define vm_utils_h

#include "common.h"

void check_err(int, const char*);
void set_bit(uint32_t*, uint8_t);

// Bit and byte extraction/manipulation
void unset_bit(uint32_t* target, uint8_t bit);
uint32_t get_bit(uint32_t target, uint8_t bit);
uint8_t get_1byte(uint32_t from, uint8_t pos);
uint16_t get_2byte(uint32_t from, uint8_t pos);

#endif
