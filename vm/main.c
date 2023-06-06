#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define increment(i) (i++)

//// OPCODES

#define OP_LOADI 1
#define OP_ADD   2
#define OP_PRNT  3
#define OP_AND   4
#define OP_OR    5
#define OP_NOT   6
#define OP_JCN   7
#define OP_SUB   8
#define OP_MUL   9
#define OP_CMP   10
#define OP_HALT  255
#define OP_NOOP  0


//// Register IDs in array
///
/// Use these values to access the registers in the registers array

#define REG_PSW 0 // Program Status Word register
#define REG_GP1 1 // General Purpose 1
#define REG_GP2 2 // General Purpose 2
#define REG_GP3 3 // General Purpose 3
#define REG_GP4 4 // General Purpose 4
#define REG_GP5 5 // General Purpose 5

//// Masks
uint32_t JCN_MASK = 1;


////

char debug_symbols[256][10] = { "NONE", "LOADI", "ADD", "PRNT", "AND", "OR", "NOT", "JCN", "SUB", "MUL", "CMP"};

int errCode = 1;

// Program counter
uint16_t PC = 0;

uint32_t registers[16];
uint32_t RAM[1LL << 16]; // 2 ^ 16

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

struct __attribute__((__packed__)) instrrv {
    uint8_t _;
    uint8_t reg;
    uint16_t val;
};

struct instrrrr {
    uint8_t _;
    uint8_t reg1;
    uint8_t reg2;
    uint8_t reg3;
};

void check_err(int, const char*);
void check_err(int status_code, const char* message) {
    if(status_code == -1) {
        err(increment(errCode), "%s", message);
    }
}

void set_bit(uint32_t*, uint8_t);
void set_bit(uint32_t* target, uint8_t bit) {
    uint32_t mask = 1;
    *target = (*target) | (mask << bit);
}

void unset_bit(uint32_t* target, uint8_t bit);
void unset_bit(uint32_t* target, uint8_t bit) {
    uint32_t mask = 1;
    *target = (*target) & ~(mask << bit);
}

void init_regs(void);
void init_regs(void) {
    for(int i = 0; i < 16; i++) {
        registers[i] = 0;
    }
}

void execute(const struct instr*);
void execute(const struct instr* i) {
    uint8_t op = i->opcode;
    if(op == OP_LOADI) {
        const struct instrrv* real_i = (const struct instrrv*) (i);
        registers[real_i->reg] = real_i->val;
    } else if(op == OP_ADD) {
        const struct instrrrr* real_i = (const struct instrrrr*) (i);
        registers[real_i->reg1] = registers[real_i->reg2] + registers[real_i->reg3];
    } else if(op == OP_NOOP) {
           
    } else if(op == OP_PRNT) {
        printf("%u\n", registers[i->b2]);  
    } else if(op == OP_AND) {
        const struct instrrrr* real_i = (const struct instrrrr*) (i);
        registers[real_i->reg1] = registers[real_i->reg2] & registers[real_i->reg3];
    } else if(op == OP_OR) {
        const struct instrrrr* real_i = (const struct instrrrr*) (i);
        registers[real_i->reg1] = registers[real_i->reg2] | registers[real_i->reg3];
    } else if(op == OP_NOT) {
        const struct instrrrr* real_i = (const struct instrrrr*) (i);
        registers[real_i->reg1] = ~registers[real_i->reg1];
    } else if(op == OP_HALT) {
        exit(0);
    } else if(op == OP_JCN) {
        const struct instrrrr* real_i = (const struct instrrrr*) (i);
        if(registers[REG_PSW] & JCN_MASK) {
            // PC must be set to (DEST - 1) since it gets incremented after every instruction
            PC = real_i->reg1 - 1;
        }
    } else if(op == OP_SUB) {
        const struct instrrrr* real_i = (const struct instrrrr*) (i);
        registers[real_i->reg1] = registers[real_i->reg2] - registers[real_i->reg3]; 
    } else if(op == OP_MUL) {
        const struct instrrrr* real_i = (const struct instrrrr*) (i);
        registers[real_i->reg1] = registers[real_i->reg2] * registers[real_i->reg3]; 
    } else if(op == OP_CMP) {
        const struct instrrrr* real_i = (const struct instrrrr*) (i);
        if(registers[real_i->reg2] == registers[real_i->reg3]) {
            set_bit(&registers[REG_PSW], real_i->reg1);
        } else {
            unset_bit(&registers[REG_PSW], real_i->reg1);
        }
    } else {
        errx(increment(errCode), "Unknown op: %d", op);
    }
}

void print_debug_info(const struct instr*);
void print_debug_info(const struct instr* instruction) {
    printf("OP: %x, byte1: %x, byte2: %x, byte3: %x\n", instruction->opcode, instruction->b2, instruction->b3, instruction->b4);
    for(int i = 0; i < 16; i++) {
        printf("reg%d: %x\n", i, registers[i]); 
    }
}

uint8_t debugFlag = 0;

int main(int argc, char** argv) {

    if(argc < 2) {
        errx(increment(errCode), "Expected binary file. Use provided assembler");
    }

    // Debug flag is given
    if(argc == 3) {
        if(!strcmp(argv[2], "-d")) {
            debugFlag = 1; 
        } else {
            errx(increment(errCode), "3 arguments were given but the second was not -d"); 
        }
    }

    // Set all registers to 0
    init_regs();

    // Open binary file
    int fd;
    check_err((fd = open(argv[1], O_RDONLY)), "Opening binary file");

    // Ensure file can be loaded into RAM and ROM
    struct stat file_info;
    check_err(fstat(fd, &file_info), "Could not determine file size");

    if(file_info.st_size % sizeof(uint32_t) != 0) {
        errx(errCode++, "File size is not divisible by 4 (instr size) ");
    }

    // Halt instruction
    struct instr instruction;

    if(read(fd, &RAM, file_info.st_size) == -1) {
        errx(errCode++, "Error reading input file");
    }
  
    close(fd);

    while(1) {
        instruction = *(struct instr*) &RAM[PC];
        if(debugFlag) {
            print_debug_info(&instruction);
        }
        execute(&instruction); 
        PC++;
    }

}



