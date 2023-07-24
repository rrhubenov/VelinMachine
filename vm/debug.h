#ifndef vm_debug_h
#define vm_debug_h

#include "common.h"
#include "bytecode.h"


void init_debug(void);
void b_to_s(uint8_t opcode, char* buf);

// Disassemble instruction
void d_instr(struct instr*, uint32_t);
void print_internal_state(uint32_t* registers, uint8_t count);

#endif