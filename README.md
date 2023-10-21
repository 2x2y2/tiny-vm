# 🖥️ tiny-vm

A tiny 16-bit virtual machine running its own tiny assembly written in C.

## How to use
`./tiny-comp <source> <output>`

`./tiny-vm <binary>`

## Architecture
Memory:
- 2^17 bytes
- code is loaded at 0x0000 by default
- stack is placed at 0x5555 by default

Instructions:
- the first 4 bits encode the op-code
- the following 12 bits encode the arguments (if any)
- all arithmetic operations are signed

Registers:
- 6 general purpose registers (R1, R2, ..., R5)
- SP and IP holding the stack pointer and the instruction pointer respectively
- FLAG register (read only by the conditional jumps, set only by the CMP operation, not modifyable by code)

## tiny-asm
op-code | args                          | desc
------- | -------------------------     | -----------
NOP     | nothing                       | does nothing
JMP     | r or unsigned imm11           | unconditional jump to arg
JEQ     | r or unsigned imm11           | jumps to arg if (FLAG == ZERO)
JNE     | r or unsigned imm11           | jumps to arg if (FLAG != ZERO)
JLT     | r or unsigned imm11           | jumps to arg if (FLAG == NEG)
JGT     | r or unsigned imm11           | jumps to arg if (FLAG == POS)
PSH     | r or signed imm11             | pushes arg to stack
POP     | r or nothing                  | pops the stack (into r if specified)
MOV     | r1, r2 or signed imm8         | r1 = (r2 or signed imm8)
LDR     | r1, r2                        | r1 = mem[r2]
STR     | r1, r2                        | mem[r1] = r2
ADD     | r1, r2, r3 or signed imm5     | r1 = r2 + (r3 or imm5)
AND     | r1, r2, r3                    | r1 = r2 AND r3
NOT     | r1, r2, r3                    | r1 = r2 NOT r3
CMP     | r1, r2                        | signed comparison of r1 and r2 (result stored in FLAG)
PRNT    | r                             | prints out r to stdout
STOP    | nothing                       | halts program execution

## tiny-comp
Compiles your code in tiny-assembly to binary format.

You can store 16-bit signed variables at the end of the code by prefixing them with a dot (and reference them in code by their address).

The compiler is, admittedly, poorly-written. It WILL compile garbage if provided garbage.

## Example program
The following program is a simple for-loop that prints all signed 16-bit integers.
```
MOV R4 10
MOV R5 11
LDR R0 R4
LDR R1 R5
PRNT R0
PRNT R1
ADD R1 R1 1
CMP R0 R1
JGT 5
STOP
.32767
.-32768
```
