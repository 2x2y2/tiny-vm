// bit manipulation!! >.<
#define mask(start, len) ((~0U >> (32 - len)) << start)
#define extend(val, len) (val & mask((len-1), 1) ? (val | mask(len, 16)) : val)
#define extend32(val, len) (val & mask((len-1), 1) ? ((int)val | mask(len, 32)) : (int)val)
#define imm(val, bits) extend((val & mask(0, bits)), bits)
#define imm32(val, bits) extend32((val & mask(0, bits)), bits)
#define SWAP_BYTES(word) (word << 0x8 | word >> 0x8)

enum {
	R0 = 0,
	R1,
	R2,
	R3,
	R4,
	R5,
	_SP,
	_IP,
	_FLAG,
	REGISTER_COUNT
} registers;

enum {
	ZERO = 0x1 << 0,
	POS = 0x1 << 1,
	NEG = 0x1 << 2
} flags;

#define STOP 0xffff
enum {
	NOP = 0,
	JMP,
	JEQ,
	JNE,
	JLT,
	JGT,
	PSH,
	POP,
	MOV,
	LDR,
	STR,
	ADD,
	AND,
	NOT,
	CMP,
	PRNT
} instructions;

/*
instructions are 16-bits long:
- first 4 bits store the op-code,
- next 12 bits encode the arguments (some zeros can be whatever if irrelevant)
- STOP ("17th" instruction) is all 1's

NOP:
- doesn't matter

JMP, JEQ, JNE, JLT, JGT:
- '000000000' followed by 3 bits for source register
- '1' followed by 11 bits for immediate value in range [-1024;1023]

PSH:
- | '000000000' followed by 3 bits for source register
  | '1' followed by 11 bits for immediate value in range [-1024;1023]

POP:
- | '000000000' followed by 3 bits for destination register
  | '100000000000' to not store value in a register

MOV:
- 3 bits for destination register
- | '1' followed by 8 bits for immediate value in range [-128;127]
  | '000000' followed by 3 bits for source register

LDR:
- 3 bits for destination register
- '000000' followed by 3 bits for source register (as address)

STR:
- 3 bits for source register
- '000000' followed by 3 bits for destination register (as address)

ADD:
- 3 bits for destination register
- 3 bits for 1st register operand
- | '1' followed by 5 bits for immediate value operand in range [-16;15]
  | '000' followed by 3 bits for 2nd register operand

AND:
- 3 bits for destination register
- '000'
- 3 bits for 1st operand register
- 3 bits for 2nd operand register

NOT:
- 3 bits for destination register
- '000000' followed by 3 bits for source register

CMP:
- 3 bits for 1st register operand
- '000000'
- 3 bits for 2nd register operand

PRNT:
- '000000000' followed by 3 bits for source register
*/

