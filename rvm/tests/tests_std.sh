
cargo build

function test_equals() {
    PROGRAM=${PROGRAMS_ROOT_DIR}${1}
    RESULT=$(./assembler/assembler.py ${PROGRAM} && target/debug/rvm out.bc)

    test $RESULT -eq $2 && echo -e $3 TEST ${GREEN}PASSED${NC} || echo -e $3 TEST ${RED}FAILED${NC} "Expected: $2, Got: $RESULT"
}

