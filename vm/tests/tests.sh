#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

PROGRAMS_ROOT_DIR="programs/"

function test_equals() {
    PROGRAM=${PROGRAMS_ROOT_DIR}${1}

    RESULT=$(make -s clean && make -s && ./assembler/assembler.py ${PROGRAM} && ./main out.bc)

    test $RESULT -eq $2 && echo -e $3 TEST ${GREEN}PASSED${NC} || echo -e $3 TEST ${RED}FAILED${NC}
}

test_equals "not.vm" 4294967294 "NOT"
test_equals "sub.vm" 5 "SUB"
test_equals "add.vm" 25 "ADD"
test_equals "and.vm" 8 "AND"
test_equals "or.vm" 29 "OR"
test_equals "mul.vm" 90 "MUL"
test_equals "jcn1.vm" 1 "JCN Jumps to the next line correctly"
test_equals "jcn2.vm" 1 "JCN skips the next line successfully"

