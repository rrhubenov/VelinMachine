#!/bin/bash

PROGRAMS_ROOT_DIR="../programs/"
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

source tests/tests_std.sh

test_equals "not.vm" 4294967294 "NOT"
test_equals "sub.vm" 5 "SUB"
test_equals "add.vm" 25 "ADD"
test_equals "and.vm" 8 "AND"
test_equals "or.vm" 29 "OR"
test_equals "mul.vm" 90 "MUL"
test_equals "store_load.vm" 10 "Loading after storing in address works as expected"
test_equals "jne.vm" 1 "JNE Jump successfully"
test_equals "jeq.vm" 2 "JEQ Jump successfully"
test_equals "lsl.vm" 2 "Logical Shift Left: 1 turns to 2"
test_equals "lsr.vm" 1 "Logical Shift Left: 2 turns to 1"
