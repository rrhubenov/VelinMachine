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
A: CMP S1 S2       | Compare and set bits in PSW (flags register) 

## Shifts
LSL DST, S1, S2 | Logical shift left    TODO!
LSR DST, S1, S2 | Logical shift rigth   TODO!

## Boolean
4: AND DST, S1, S2 | Boolean AND                                
5: OR DST, S1, S2  | Boolean OR                                 
6: NOT DST         | Boolean NOT # Inverts the bits of DST      

## Comparison


## Branching
// Използва първия бит на PSW регистъра
7: JNE DST         | Jump to destination if zero flag IS NOT set
B: JEQ DST         | Jump to destination if zero flag IS set    


## Misc
3: PRNT SRC     | Print value in register SRC        
FF: HALT       | Halt the machine                   

# Registers

IN PROGRESS

CNST: register for loading constants
0 - PSW (Program Status Word) 32 bit register that is used for flags
1 - 5 GP Registers





