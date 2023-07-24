#include <stdio.h>

#include "debug.h"
#include "utils.h"
#include "bytecode.h"

static const char* op_to_s_map[256];

void d_instr(struct instr* i, uint32_t offset) {
    printf("%04X ", offset); 
    const char* op_name = b_to_s(i->opcode);
    printf("%s ", op_name);

    switch(i->opcode) {
        case OP_LOADI:
            d_loadi(i);
            break;
        case OP_ADD:
            d_add(i);
            break;
        case OP_PRNT:
            d_prnt(i);
            break;
        case OP_AND:
            d_and(i);
            break;
        case OP_OR:
            d_or(i);
            break;
        case OP_NOT:
            d_not(i);
            break;
        case OP_JNE:
            d_jne(i);
            break;
        case OP_SUB:
            d_sub(i);
            break;
        case OP_MUL:
            d_mul(i);
            break;
        case OP_CMP:
            d_cmp(i);
            break;
        case OP_JEQ:
            d_jeq(i);
            break;
        case OP_LOAD:
            d_load(i);
            break;
        case OP_STORE:
            d_store(i);
            break;
        case OP_HALT:
            d_halt();
            break;
        case OP_NOOP:
            d_noop();
            break;
    }
    printf("\n");
}

void print_internal_state(uint32_t* registers, uint8_t count) {
    for(int i = 0; i < count/2; i++) {
        printf("reg%d: %04x", i, registers[i]); 
        printf("\treg%d: %04X\n", i+(count/2), registers[i+(count/2)]);
    }
}


void init_debug() {
    op_to_s_map[0] = "NOOP";
    op_to_s_map[1] = "LOADI";
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

const char*b_to_s(uint8_t opcode) {
    return op_to_s_map[opcode];
}

void d_noop() {}

void d_loadi(struct instr* i) {
    printf("DST: %02X, CNST: %04X", i->b2, *(uint16_t*) &(i->b3));
}

void d_add(struct instr* i) {
    printf("DST: %02X, SRC1: %02X, SRC2: %02X",
            i->b2,
            i->b3,
            i->b4);
}

void d_prnt(struct instr* i) {
    printf("SRC: %02X", i->b2);
}

void d_and(struct instr* i) {
    printf("DST: %02X, SRC1: %02X, SRC2: %02X",
            i->b2,
            i->b3,
            i->b4);
}

void d_or(struct instr* i) {
    printf("DST: %02X, SRC1: %02X, SRC2: %02X",
            i->b2,
            i->b3,
            i->b4);
}

void d_not(struct instr* i) {
    printf("DST: %02X",
            i->b2);
}

void d_sub(struct instr* i) {
    printf("DST: %02X, SRC1: %02X, SRC2: %02X",
            i->b2,
            i->b3,
            i->b4);
}

void d_mul(struct instr* i) {
    printf("DST: %02X, SRC1: %02X, SRC2: %02X",
            i->b2,
            i->b3,
            i->b4);
}

void d_jne(struct instr* i) {
    printf("DST: %04X",
            *(uint16_t*)&i->b2);
}

void d_cmp(struct instr* i) {
    printf("SRC1: %02X, SRC2: %02X",
            i->b2,
            i->b3);
}

void d_jeq(struct instr* i) {
    printf("DST: %04X",
            *(uint16_t*)&i->b2);
}

void d_load(struct instr* i) {
    printf("DST: %02X, ADDR: %04X", i->b2, *(uint16_t*)&i->b3);
}

void d_store(struct instr* i) {
    printf("SRC: %02X, ADDR: %04X", i->b2, *(uint16_t*)&i->b3);
}

void d_halt() {}

