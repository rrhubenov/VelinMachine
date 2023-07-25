#ifndef vm_debug_h
#define vm_debug_h

#include "common.h"
#include "bytecode.h"


void init_debug(void);
const char* b_to_s(uint8_t opcode);

// Disassemble instruction
void d_instr(struct instr*, uint32_t);
void print_internal_state(uint32_t* registers, uint8_t count);

void d_noop(void);
void d_loadi(struct instr*);
void d_add(struct instr*);
void d_prnt(struct instr*);
void d_and(struct instr*);
void d_or(struct instr*);
void d_not(struct instr*);
void d_jne(struct instr*);
void d_sub(struct instr*);
void d_mul(struct instr*);
void d_cmp(struct instr*);
void d_jeq(struct instr*);
void d_load(struct instr*);
void d_store(struct instr*);
void d_halt(void);
void d_lsl(struct instr*);
void d_lsr(struct instr*);

#endif
