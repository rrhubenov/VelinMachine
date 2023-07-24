#include <stdio.h>

#include "debug.h"


static char* op_to_s_map[256];

void d_instr(struct instr* i, uint32_t offset) {
    printf("%X ", offset);

    char op_name[10];
    b_to_s(i->opcode, op_name);

    printf("%s %X %X %X\n", op_name, i->b2, i->b3, i->b4);
}

void print_internal_state(uint32_t* registers, uint8_t count) {
    for(int i = 0; i < count; i++) {
        printf("reg%d: %x\n", i, registers[i]); 
    }
}

void init_debug() {
    op_to_s_map[0] = "NOOP";
    op_to_s_map[1] = "LOAD";
    op_to_s_map[2] = "ADD";
    op_to_s_map[3] = "PRNT";
    op_to_s_map[4] = "AND";
    op_to_s_map[5] = "OR";
    op_to_s_map[6] = "NOT";
    op_to_s_map[7] = "JNE";
    op_to_s_map[8] = "SUB";
    op_to_s_map[9] = "MUL";
    op_to_s_map[0xA] = "CMP";
    op_to_s_map[0xB] = "JEQ";
    op_to_s_map[0xC] = "LOAD";
    op_to_s_map[0xD] = "STORE";
    op_to_s_map[0xFF] = "HALT";
}

void b_to_s(uint8_t opcode, char* buf) {
    buf = op_to_s_map[opcode];
}

