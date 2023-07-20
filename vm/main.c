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

#define PROGRAM_SIZE_LIMIT 1024

//// OPCODES

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


////

char debug_symbols[256][10] = { "NONE", "LOADI", "ADD", "PRNT", "AND", "OR", "NOT", "JNE", "SUB", "MUL", "CMP"};

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

uint32_t get_bit(uint32_t target, uint8_t bit);
uint32_t get_bit(uint32_t target, uint8_t bit) {
    uint32_t mask = 1;
    return target & (mask << bit);
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
    } else {
        errx(increment(errCode), "Unknown op: %X", op);
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
            print_debug_info(instruction);
        }
        PC++;
    }

}



