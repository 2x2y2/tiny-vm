#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "tiny-conf.h"

#define CODE_BASE_ADDRESS  0x0000
#define STACK_BASE_ADDRESS 0x5555

#define MEM_SIZE (0x1 << 16)
uint16_t mem[MEM_SIZE] = { 0 };
uint16_t r[REGISTER_COUNT] = { 0 };
char done = 0;

#define FLAG r[_FLAG]
#define IP r[_IP]
#define SP r[_SP]
#define fetch (mem[r[_IP]])

void print_code(size_t program_size_bytes) {
	printf("mem[%#06x]=\n", CODE_BASE_ADDRESS);
	for (size_t i = 0; i < program_size_bytes/2; i++) { // 2 bytes per instruction
		uint16_t word = mem[CODE_BASE_ADDRESS+i];
		printf("%#06x\n", word);
	}
	printf("----------------\n");
}

void print_stack(void) {
	printf("stack=");
	for (size_t i = STACK_BASE_ADDRESS; i < (size_t)(SP+1); i++) {
		printf("%hu, ", mem[i]);
	}
	printf("\n");
}

void print_registers(void) {
	printf("registers=");
	for (size_t i = 0; i < REGISTER_COUNT; i++) {
		printf("%hu, ", r[i]);
	}
	printf("\n");
}

void failwith(const char* msg) {
	printf("[fail] %s\n", msg);
	exit(1);
}

void eval(uint16_t instruction) {
	uint16_t opcode = instruction >> 12;
	instruction &= mask(0, 12);
	switch (opcode) {
		case NOP:
			break;
		case JMP:
			if (instruction & mask(11, 1)) {
				IP = (instruction & mask(0, 11)) - 1;
			}
			else {
				IP = r[instruction & mask(0, 3)] - 1;
			} break;
		case JEQ:
			if (FLAG == ZERO) {
				if (instruction & mask(11, 1)) {
					IP = (instruction & mask(0, 11)) - 1;
				}
				else {
					IP = r[instruction & mask(0, 3)] - 1;
				}
			} break;
		case JNE:
			if (FLAG != ZERO) {
				if (instruction & mask(11, 1)) {
					IP = (instruction & mask(0, 11)) - 1;
				}
				else {
					IP = r[instruction & mask(0, 3)] - 1;
				}
			} break;
		case JLT:
			if (FLAG == NEG) {
				if (instruction & mask(11, 1)) {
					IP = (instruction & mask(0, 11)) - 1;
				}
				else {
					IP = r[instruction & mask(0, 3)] - 1;
				}
			} break;
		case JGT:
			if (FLAG == POS) {
				if (instruction & mask(11, 1)) {
					IP = (instruction & mask(0, 11)) - 1;
				}
				else {
					IP = r[instruction & mask(0, 3)] - 1;
				}
			} break;
		case PSH:
			if (SP == 0xffff) {
				failwith("tried to push but stack is full");
			}
			if (instruction & mask(11, 1)) {
				mem[++SP] = imm(instruction, 11);
			}
			else {
				mem[++SP] = r[instruction & mask(0, 3)];
			} break;
		case POP:
			if (SP < 0) {
				failwith("tried to pop but stack is empty");
			}
			if (instruction & mask(11, 1)) {
				SP--;
			}
			else {
				r[instruction & mask(0, 3)] = mem[SP--];
			} break;
		case MOV:
			if (instruction & mask(8, 1)) {
				uint16_t dst = (instruction >> 9) & mask(0, 3);
				r[dst] = imm(instruction, 8);
			}
			else {
				uint16_t dst = (instruction >> 9) & mask(0, 3);
				uint16_t src = instruction & mask(0, 3);
				r[dst] = r[src];
			} break;
		case LDR: {
			uint16_t dst = (instruction >> 9) & mask(0, 3);
			uint16_t src = instruction & mask(0, 3);
			r[dst] = mem[r[src]];
			} break;
		case STR: {
			uint16_t dst = instruction & mask(0, 3);
			uint16_t src = (instruction >> 9) & mask(0, 3);
			mem[r[dst]] = r[src];
			} break;
		case ADD:
			if (instruction & mask(5, 1)) {
				uint16_t dst = (instruction >> 9) & mask(0, 3);
				uint16_t src = (instruction >> 6) & mask(0, 3);
				r[dst] = r[src] + imm(instruction, 5);
			}
			else {
				uint16_t dst = (instruction >> 9) & mask(0, 3);
				uint16_t op1 = (instruction >> 6) & mask(0, 3);
				uint16_t op2 = instruction & mask(0, 3);
				r[dst] = r[op1] + r[op2];
			} break;
		case AND: {
			uint16_t dst = (instruction >> 9) & mask(0, 3);
			uint16_t op1 = (instruction >> 3) & mask(0, 3);
			uint16_t op2 = instruction & mask(0, 3);
			r[dst] = r[op1] & r[op2];
			} break;
		case NOT: {
			uint16_t dst = (instruction >> 9) & mask(0, 3);
			uint16_t src = instruction & mask(0, 3);
			r[dst] = ~r[src];
			} break;
		case CMP: {
			uint16_t op1 = r[(instruction >> 9) & mask(0, 3)] ^ mask(15, 1);
			uint16_t op2 = r[instruction & mask(0, 3)] ^ mask(15, 1);
			if (op1 == op2)
				FLAG = ZERO;
			else if (op1 > op2)
				FLAG = POS;
			else
				FLAG = NEG;
			} break;
		case PRNT:
			if (!(instruction & mask(3, 9))) {
				uint16_t reg = instruction & mask(0, 3);
				printf("r[%hu]=%d\n", reg, imm32(r[reg], 16));
			}
			else {
				done = 1;
				printf("PROGRAM TERMINATED\n");
			} break;
		default:
			printf("ILLEGAL INSTRUCTION: %hu\n", (instruction | (opcode << 12)));
			exit(1);
			break;
	}
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("usage: ./tiny-vm <binary>\n");
		exit(1);
	}

	FILE* bin = fopen(argv[1], "rb");
	if (!bin) {
		printf("[fail] couldn't open %s\n", argv[1]);
		exit(1);
	}

	fseek(bin, 0, SEEK_END);
	size_t bin_size_bytes = ftell(bin);
	fseek(bin, 0, SEEK_SET);

	fread(&mem[CODE_BASE_ADDRESS], sizeof(uint16_t), bin_size_bytes/2, bin);
	fclose(bin);
#ifdef DEBUG
	print_code(bin_size_bytes);
#endif //DEBUG
	IP = CODE_BASE_ADDRESS;
	SP = STACK_BASE_ADDRESS - 1;
	FLAG = ZERO;
	while (!done) {
		eval(fetch);
#ifdef DEBUG
		print_stack();
		print_registers();
#endif //DEBUG
		IP++;
	}

	return 0;
}

