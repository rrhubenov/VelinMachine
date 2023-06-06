#!/bin/bash
./assembler/assembler.py program.vm && make && ./build/main out.bc -d
