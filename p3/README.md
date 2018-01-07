# Simple CPU

## Instruction Format
Opcode (23-19)
1. Function in CPU: ALU Control
2. Programming Function: Determines which operation to perform, and immediate mode sourcing

Dest Register (18-16)
1. Function in CPU: Reg FileRegister Select
2. Programming Function: Primary source register specification

Register A (15-13)
1. Function in CPU: MUX A Control
2. Programming Function: Secondary source register specification

Register B (12-10)
1. Function in CPU: MUX B Control
2. Programming Function: Destination register specification (This is 0 for immediate mode)

Data (9-0)
1. Function in CPU: Input Data (when necessary)
2. Programming Function: Signed input data (when necessary)

### Note:
The assembler processes MIPSs-like assembler syntax with all forms using the same instruction format
R0-format: opcode
R2-format: opcode  R(dest)  R(srcA)
R3-format: opcode  R(dest)  R(srcA)  R(srcB)
I1-format:  opcode  R(dest)  immediate value
I2-format:  opcode  R(dest)  R(srcA)  immediate value


## Operation Description

Operation    OpCode (5 bits)    Format

AND             00000                   R3
OR               00001                   R3
NOT             00010                   R2
ADD             00011                   R3
MOV            00100                   R2
SLL              00101                   R2
SRL             00110
SUB             00111
ADDI            01000
SUBI            01001
MOVI           01010
BNEZ           01011
NOP            01100
HALT           01101

# ALU
You need to design your ALU from scratch
