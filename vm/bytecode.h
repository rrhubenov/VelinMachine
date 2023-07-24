#ifndef vm_bytecode_h
#define vm_bytecode_h

#include "common.h"

#define OP_LOADI 0x1
#define OP_ADD   0x2
#define OP_PRNT  0x3
#define OP_AND   0x4
#define OP_OR    0x5
#define OP_NOT   0x6
#define OP_JNE   0x7
#define OP_SUB   0x8
#define OP_MUL   0x9
#define OP_CMP   0xA
#define OP_JEQ   0xB
#define OP_LOAD  0xC
#define OP_STORE 0xD
#define OP_HALT  0xFF
#define OP_NOOP  0x0

//char debug_symbols[256][10] = { "NONE", "LOADI", "ADD", "PRNT", "AND", "OR", "NOT", "JNE", "SUB", "MUL", "CMP"};

struct instr {
    // byte1
    uint8_t opcode;
    // byte2
    uint8_t b2;
    // byte3
    uint8_t b3;
    // byte4
    uint8_t b4;
};

struct __attribute__((__packed__)) instr1r1v {
    uint8_t _;
    uint8_t reg;
    uint16_t val;
};

struct instr3r {
    uint8_t _;
    uint8_t reg1;
    uint8_t reg2;
    uint8_t reg3;
};

struct instr1r {
    uint8_t _;
    uint8_t reg1;
    uint8_t __;
    uint8_t ___;
};

struct instr2r {
    uint8_t _;
    uint8_t reg1;
    uint8_t reg2;
    uint8_t __;
};


#endif
