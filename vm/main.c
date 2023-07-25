#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "bytecode.h"
#include "debug.h"
#include "utils.h"

#define PROGRAM_SIZE_LIMIT 1024
#define REGISTER_COUNT 16

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
uint32_t JNE_MASK = 1;
uint8_t CMP_BIT = 0;

int errCode = 1;

// Program counter
uint16_t PC = 0;

uint32_t registers[REGISTER_COUNT];
uint32_t RAM[1LL << 16]; // 2 ^ 16

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
        const struct instr1r1v* real_i = (const struct instr1r1v*) (i);
        registers[real_i->reg] = real_i->val;
    } else if(op == OP_ADD) {
        const struct instr3r* real_i = (const struct instr3r*) (i);
        registers[real_i->reg1] = registers[real_i->reg2] + registers[real_i->reg3];
    } else if(op == OP_NOOP) {
           
    } else if(op == OP_PRNT) {
        dprintf(1, "%u\n", registers[i->b2]);  
    } else if(op == OP_AND) {
        const struct instr3r* real_i = (const struct instr3r*) (i);
        registers[real_i->reg1] = registers[real_i->reg2] & registers[real_i->reg3];
    } else if(op == OP_OR) {
        const struct instr3r* real_i = (const struct instr3r*) (i);
        registers[real_i->reg1] = registers[real_i->reg2] | registers[real_i->reg3];
    } else if(op == OP_NOT) {
        const struct instr3r* real_i = (const struct instr3r*) (i);
        registers[real_i->reg1] = ~registers[real_i->reg1];
    } else if(op == OP_HALT) {
        exit(0);
    } else if(op == OP_JNE) {
        const struct instr1r* real_i = (const struct instr1r*) (i);
        if(!get_bit(registers[REG_PSW], CMP_BIT)) {
            // PC must be set to (DEST - 1) since it gets incremented after every instruction
            PC = real_i->reg1 - 1;
        } 
    } else if(op == OP_JEQ) {
        const struct instr1r* real_i = (const struct instr1r*) (i);
        if(get_bit(registers[REG_PSW], CMP_BIT)) {
            // PC must be set to (DEST - 1) since it gets incremented after every instruction
            PC = real_i->reg1 - 1;
        } 
    } else if(op == OP_SUB) {
        const struct instr3r* real_i = (const struct instr3r*) (i);
        registers[real_i->reg1] = registers[real_i->reg2] - registers[real_i->reg3]; 
    } else if(op == OP_MUL) {
        const struct instr3r* real_i = (const struct instr3r*) (i);
        registers[real_i->reg1] = registers[real_i->reg2] * registers[real_i->reg3]; 
    } else if(op == OP_CMP) {
        const struct instr2r* real_i = (const struct instr2r*) (i);
        if(registers[real_i->reg1] == registers[real_i->reg2]) {
            set_bit(&registers[REG_PSW], CMP_BIT);
        } else {
            unset_bit(&registers[REG_PSW], CMP_BIT);
        }
    } else if(op == OP_LOAD) {
        const struct instr1r1v* real_i = (const struct instr1r1v*) (i);
        uint16_t addr = real_i->val + PROGRAM_SIZE_LIMIT;
        registers[real_i->reg] = RAM[addr];
    } else if(op == OP_STORE) {
        const struct instr1r1v* real_i = (const struct instr1r1v*) (i);    
        uint32_t val = registers[real_i->reg];
        uint16_t addr = real_i->val + PROGRAM_SIZE_LIMIT;
        RAM[addr] = val; 
    } else if(op == OP_LSL) {
        const struct instr3r* real_i = (const struct instr3r*) (i);
        registers[real_i->reg1] = registers[real_i->reg1] << 1;
    } else if(op == OP_LSR) {
        const struct instr3r* real_i = (const struct instr3r*) (i);
        registers[real_i->reg1] = registers[real_i->reg1] >> 1;
    } else {
        errx(1, "Unknown op: %X", op);
    }
}

uint8_t debugFlag = 0;

int main(int argc, char** argv) {

    if(argc < 2) {
        errx(1, "Expected binary file. Use provided assembler");
    }

    // Debug flag is given
    if(argc == 3) {
        if(!strcmp(argv[2], "-d")) {
            debugFlag = 1; 
        } else {
            errx(1, "3 arguments were given but the second was not -d"); 
        }
    }

    // Set all registers to 0
    init_regs();
    init_debug();

    // Open binary file
    int fd;
    check_err((fd = open(argv[1], O_RDONLY)), "Opening binary file");

    // Ensure file can be loaded into RAM and ROM
    struct stat file_info;
    check_err(fstat(fd, &file_info), "Could not determine file size");

    if(file_info.st_size % sizeof(uint32_t) != 0) {
        errx(errCode++, "File size is not divisible by 4 (instr size) ");
    }

    if(file_info.st_size > PROGRAM_SIZE_LIMIT) {
        errx(errCode++, "Program is over 1024 bytes. The VM currently supports programs up to 1024 bytes long");
    }

    // Halt instruction
    struct instr* instruction;

    if(read(fd, &RAM, file_info.st_size) == -1) {
        errx(errCode++, "Error reading input file");
    }
  
    close(fd);

    while(1) {
        instruction = (struct instr*) &RAM[PC];
        execute(instruction); 
        if(debugFlag) {
            d_instr(instruction, PC);
            print_internal_state(registers, REGISTER_COUNT);
        }
        PC++;
    }

}

