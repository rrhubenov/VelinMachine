#!/usr/local/bin/python3

import sys

linenum = 0

class Instruction:
    def __init__(self, name: str, arg_count: int, opcode: int):
        self.name=name
        self.arg_count=arg_count
        self.opcode=opcode

        
instructions = []

"""
         Operation registration functions.

 To register a new operation for the assembler
 Just use one of these functions or create a new one
 if your operation uses some other arity
 function name semantics: register_OP_{arity}
 Ex: register_OP_1_1_1 registers an operation that takes 3
 arguments each with length 1 byte
 Ex: register_OP_2_0 registers an operation that takes 1 
 argument with length 2 bytes

 The total width (bytes) of the arguments must not be more than 3 bytes
 Currently there isn't a function that registers an operation
 which has an arg of width half-byte, but one can be easily created.

"""

def register_OP_1_1_1(instr):
    def process(args) -> bytes:
        opcode = instr.opcode

        assert len(args) == 3
        assert opcode < 256

        args = [int(arg) for arg in args]

        assert args[0] < 256, f"{instr.name}: All values must be between 0 and 255 inclusive"
        assert args[1] < 256, f"{instr.name}: All values must be between 0 and 255 inclusive"
        assert args[2] < 256, f"{instr.name}: All values must be between 0 and 255 inclusive"

        opcode_byte = bytes([opcode])
        dst = bytes([args[0]])
        src1 = bytes([args[1]])
        src2 = bytes([args[2]])

        return opcode_byte + dst + src1 + src2
    process.__name__ = f'process_{instr.name}'
    instr.process = process
    instructions.append(instr)

def register_OP_1_0_0(instr):
    def process(args) -> bytes:
        opcode = instr.opcode

        assert len(args) == 1
        assert opcode < 256

        args = [int(arg) for arg in args]

        assert args[0] < 256, f"{instr.name}: All values must be between 0 and 255 inclusive"

        opcode_byte = bytes([opcode])
        op = bytes([args[0]])
        null = bytes([0, 0])

        return opcode_byte + op + null
    process.__name__ = f'process_{instr.name}'
    instr.process = process
    instructions.append(instr)

def register_OP_0_0_0(instr):
    def process(args) -> bytes:
        opcode = instr.opcode

        assert len(args) == 0
        assert opcode < 256

        opcode_byte = bytes([opcode])
        null = bytes([0,0,0])

        return opcode_byte + null
    process.__name__ = f'process_{instr.name}'
    instr.process = process
    instructions.append(instr)

def register_OP_1_1_0(instr):
    def process(args) -> bytes:
        opcode = instr.opcode

        assert len(args) == 2
        assert opcode < 256

        args = [int(arg) for arg in args]

        opcode_byte = bytes([opcode])
        src1 = bytes([args[0]])
        src2 = bytes([args[1]])
        null = bytes([0])

        return opcode_byte + src1 + src2 + null
    process.__name__ = f'process_{instr.name}'
    instr.process = process
    instructions.append(instr)

def register_OP_1_2(instr):
    def process(args) -> bytes:
        opcode = instr.opcode

        assert len(args) == 2
        assert opcode < 256

        args = [int(arg) for arg in args]

        opcode_byte = bytes([opcode])
        first_op = bytes([args[0]])
        second_op = args[1].to_bytes(2, sys.byteorder)

        return opcode_byte + first_op + second_op
    process.__name__ = f'process_{instr.name}'
    instr.process = process
    instructions.append(instr)

def register_OP_2_0(instr):
    def process(args) -> bytes:
        opcode = instr.opcode

        assert len(args) == 1
        assert opcode < 256

        args = [int(arg) for arg in args]

        opcode_byte = bytes([opcode])
        op = args[0].to_bytes(2, sys.byteorder)
        null = bytes([0])

        return opcode_byte + op + null
    process.__name__ = f'process_{instr.name}'
    instr.process = process
    instructions.append(instr)

#### OPS

LOADI = Instruction("LOADI", 2, 1)
register_OP_1_2(LOADI)

ADD  = Instruction("ADD", 3, 2)
register_OP_1_1_1(ADD)

PRNT = Instruction("PRNT", 1, 3)
register_OP_1_0_0(PRNT)

AND = Instruction("AND", 3, 4)
register_OP_1_1_1(AND)

OR = Instruction("OR", 3, 5)
register_OP_1_1_1(OR)

NOT = Instruction("NOT", 1, 6)
register_OP_1_0_0(NOT)

JNE = Instruction("JNE", 1, 7)
register_OP_2_0(JNE)

JEQ = Instruction("JEQ", 1, 11)
register_OP_2_0(JEQ)

SUB = Instruction("SUB", 3, 8)
register_OP_1_1_1(SUB)

MUL = Instruction("MUL", 3, 9)
register_OP_1_1_1(MUL)

# TODO:  There needs to be a test that the bit arg is <= 31
# Can pass a "test" function that accepts the args
CMP = Instruction("CMP", 2, 10)
register_OP_1_1_0(CMP)

HLT = Instruction("HLT", 0, 255)
register_OP_0_0_0(HLT)

NOOP = Instruction("NOOP", 0, 0)
register_OP_0_0_0(NOOP)

LOAD = Instruction("LOAD", 2, 12)
register_OP_1_2(LOAD)

STORE = Instruction("STORE", 2, 13)
register_OP_1_2(STORE)

#####


def gen_byte_code(op, args) -> bytes:

    # Two bytes
    bytecode = bytes(2)

    instruction = filter(lambda instr: instr.name == op, instructions)

    # Existing op check
    try:
        instruction = next(instruction)
    except StopIteration:
        assert False, f'Unknown op: {op}'

    # Correct arg count check
    assert len(args) == instruction.arg_count, f'Line: {linenum}, LOADI: wrong number of args. Given {len(args)}, expected 2'

    return instruction.process(args)

    

with open(sys.argv[1], 'rt') as f:
    lines = f.readlines()

    result = bytes()

    for line in lines:
        line = line.strip()
        if line == "":
            continue
        linesplit = line.split()

        # Remove OP
        op = linesplit.pop(0)

        instruction_bytes = gen_byte_code(op, linesplit)
        
        result = result + instruction_bytes

        linenum = linenum + 1

    output_file = open('out.bc', 'wb')
    output_file.write(result)
