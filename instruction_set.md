# Instructions

Every instruction is exactly 4 bytes.
First byte is always opcode.

Next bytes might be either only registers or register and a constant.

## Noop
0: NOOP         | No operation   

## Loads
C: LOAD  DST ADDR | Load word  (32 bits)  
1: LOADI DST CNST | Load immidiate. Write CNST to DST register.  

## Stores
D: STORE SRC ADDR | Store word (32 bits)   

## Arithmetic
2: ADD DST, S1, S2 | Add            
8: SUB DST, S1, S2 | Subtract       
9: MUL DST, S1, S2 | Multiply       

## Shifts
E: LSL DST     | Logical shift left    
F: LSR DST     | Logical shift rigth  

## Boolean
4: AND DST, S1, S2 | Boolean AND                                
5: OR DST, S1, S2  | Boolean OR                                 
6: NOT DST         | Boolean NOT # Inverts the bits of DST      

## Comparison
A: CMP S1 S2       | Compare and set bits in PSW (flags register) 
?: CMPI S1 CNST    | Compare value in S1 with constanst and set bits in PSW

## Branching
// Използва първия бит на PSW регистъра
7: JNE DST         | Jump to destination if zero flag IS NOT set | (2,0) -> 2 bytes for DST
B: JEQ DST         | Jump to destination if zero flag IS set     | (2,0) -> 2 bytes for DST


## Misc
3: PRNT SRC     | Print value in register SRC        
FF: HALT       | Halt the machine                   

# Registers

IN PROGRESS

CNST: register for loading constants
0 - PSW (Program Status Word) 32 bit register that is used for flags
1 - 5 GP Registers

# To be added
Need a "mov" instruction to move (or copy) a value from one register
to another.

Need an easy way to add, subtract, multiply to a register with
immediate values. The current versions use only registers.



