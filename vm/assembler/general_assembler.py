#!/usr/local/bin/python3

import sys

linenum = 0

class Instruction:
    def __init__(self, name: str, arg_count: int, opcode: int):
        self.name=name
        self.arg_count=arg_count
        self.opcode=opcode

        
instructions = []

def parse_args(args):
    pass

with open(sys.argv[1], 'rt') as f:
    lines = f.readlines()

    result = bytes()

    for line in lines:
        line = line.strip()
        if line == "":
            continue
        linesplit = line.split()

        instr = Instruction(linesplit[1], len(linesplit[2:]), linesplit[0])

    output_file = open('out.bc', 'wb')
    output_file.write(result)
