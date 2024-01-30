#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "tiny-conf.h"

char* __strtok_r(char* s, const char* delim, char** save_ptr) {
	char* end;
	if (s == NULL)
		s = *save_ptr;
	if (*s == '\0') {
		*save_ptr = s;
		return NULL;
	}
	s += strspn(s, delim);
	if (*s == '\0')
	{
		*save_ptr = s;
		return NULL;
	}
	end = s + strcspn(s, delim);
	if (*end == '\0')
	{
		*save_ptr = end;
		return s;
	}
	*end = '\0';
	*save_ptr = end + 1;
	return s;
}

uint16_t str_to_opcode(char* str) {
	if (strcmp(str, "NOP") == 0) return NOP;
	if (strcmp(str, "JMP") == 0) return JMP;
	if (strcmp(str, "JEQ") == 0) return JEQ;
	if (strcmp(str, "JNE") == 0) return JNE;
	if (strcmp(str, "JLT") == 0) return JLT;
	if (strcmp(str, "JGT") == 0) return JGT;
	if (strcmp(str, "PSH") == 0) return PSH;
	if (strcmp(str, "POP") == 0) return POP;
	if (strcmp(str, "MOV") == 0) return MOV;
	if (strcmp(str, "LDR") == 0) return LDR;
	if (strcmp(str, "STR") == 0) return STR;
	if (strcmp(str, "ADD") == 0) return ADD;
	if (strcmp(str, "AND") == 0) return AND;
	if (strcmp(str, "NOT") == 0) return NOT;
	if (strcmp(str, "CMP") == 0) return CMP;
	if (strcmp(str, "PRNT") == 0) return PRNT;
	if (strcmp(str, "STOP") == 0) return STOP;
	printf("[fail] opcode %s not recognised\n", str);
	exit(1);
	return 0;
}

// this really could be cleaner but uhmmm uhhh well uhhh
void assemble(const char* src, const char* out) {
	FILE* source = fopen(src, "r");
	if (!source) {
		printf("[fail] couldn't open %s\n", src);
		exit(1);
	}

	FILE* output = fopen(out, "wb");
	if (!output) {
		printf("[fail] couldn't open %s\n", out);
	}

	uint8_t data_section_started = 0;
	char line_buff[256];
	uint32_t line = 0;
	while (fgets(line_buff, sizeof(line_buff), source)) {
		line++;
		uint16_t assembled = 0;
		uint32_t line_len = strlen(line_buff);
		if (line_buff[line_len-1] == '\n')
			line_buff[line_len-1] = '\0';

		if (line_buff[0] == '.') {
			data_section_started = 1;
			int to_store = (atoi(line_buff+1));
			if (to_store > 32767 || to_store < -32768)
				printf("[l%d] tried to store value outside of range [-32768;32767]\n", line);
			fwrite(&to_store, 1, sizeof(uint16_t), output);
			continue;
		}

		if (data_section_started) {
			printf("[l%d] tried to write instructions after storing data\n", line);
			exit(1);
		}
		char* rest = NULL;
		char* tok = __strtok_r(line_buff, " ", &rest);
		uint16_t opcode = str_to_opcode(tok);
		tok = __strtok_r(NULL, " ", &rest);
		switch (opcode) {
			case JMP: {
				assembled |= JMP << 12;
				if (strlen(tok) == 2 && tok[0] == 'R') {
					assembled |= (tok[1] - 48) & mask(0, 3);
				}
				else if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= mask(11, 1);
					uint32_t addr = atoi(tok);
					if (addr & mask(15, 17)) {
						printf("[l%d] JMP immediate value must be in range [0;2047]\n", line);
						exit(1);
					}
					assembled |= addr & mask(0, 11);
				} } break;
			case JEQ: {
				assembled |= JEQ << 12;
				if (strlen(tok) == 2 && tok[0] == 'R') {
					assembled |= (tok[1] - 48) & mask(0, 3);
				}
				else if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= mask(11, 1);
					uint32_t addr = atoi(tok);
					if (addr & mask(15, 17)) {
						printf("[l%d] JEQ immediate value must be in range [0;2047]\n", line);
						exit(1);
					}
					assembled |= addr & mask(0, 11);
				} } break;
			case JNE: {
				assembled |= JNE << 12;
				if (strlen(tok) == 2 && tok[0] == 'R') {
					assembled |= (tok[1] - 48) & mask(0, 3);
				}
				else if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= mask(11, 1);
					uint32_t addr = atoi(tok);
					if (addr & mask(15, 17)) {
						printf("[l%d] JNE immediate value must be in range [0;2047]\n", line);
						exit(1);
					}
					assembled |= addr & mask(0, 11);
				} } break;
			case JLT: {
				assembled |= JLT << 12;
				if (strlen(tok) == 2 && tok[0] == 'R') {
					assembled |= (tok[1] - 48) & mask(0, 3);
				}
				else if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= mask(11, 1);
					uint32_t addr = atoi(tok);
					if (addr & mask(15, 17)) {
						printf("[l%d] JLT immediate value must be in range [0;2047]\n", line);
						exit(1);
					}
					assembled |= addr & mask(0, 11);
				} } break;
			case JGT: {
				assembled |= JGT << 12;
				if (strlen(tok) == 2 && tok[0] == 'R') {
					assembled |= (tok[1] - 48) & mask(0, 3);
				}
				else if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= mask(11, 1);
					uint32_t addr = atoi(tok);
					if (addr & mask(15, 17)) {
						printf("[l%d] JGT immediate value must be in range [0;2047]\n", line);
						exit(1);
					}
					assembled |= addr & mask(0, 11);
				} } break;
			case PSH: {
				assembled |= PSH << 12;
				if (strlen(tok) == 2 && tok[0] == 'R') {
					assembled |= (tok[1] - 48) & mask(0, 3);
				}
				else if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= mask(11, 1);
					int val = atoi(tok);
					if (val > 1023 || val < -1024) {
						printf("[l%d] PSH immediate value must be in range [-1024;1023]\n", line);
						exit(1);
					}
					assembled |= val & mask(0, 11);;
				} } break;
			case POP: {
				assembled |= POP << 12;
				if (strlen(tok) == 2 && tok[0] == 'R') {
					assembled |= (tok[1] - 48) & mask(0, 3);
				}
				else if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= mask(11, 1);
				} } break;
			case MOV: {
				assembled |= MOV << 12;
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP << 9;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP << 9;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3)) << 9;
				}
				tok = __strtok_r(NULL, " ", &rest);
				if (strlen(tok) == 2 && tok[0] == 'R') {
					assembled |= (tok[1] - 48) & mask(0, 3);
				}
				else if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= mask(8, 1);
					int val = atoi(tok);
					if (val > 127 || val < -128) {
						printf("[l%d] MOV immediate value must be in range [-128;127]\n", line);
						exit(1);
					}
					assembled |= val & 0xff;
				} } break;
			case STR: {
				assembled |= STR << 12;
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP << 9;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP << 9;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3)) << 9;
				}
				tok = __strtok_r(NULL, " ", &rest);
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3));
				} } break;
			case LDR: {
				assembled |= LDR << 12;
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP << 9;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP << 9;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3)) << 9;
				}
				tok = __strtok_r(NULL, " ", &rest);
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3));
				} } break;
			case ADD: {
				assembled |= ADD << 12;
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP << 9;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP << 9;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3)) << 9;
				}
				tok = __strtok_r(NULL, " ", &rest);
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP << 6;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP << 6;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3)) << 6;
				}
				tok = __strtok_r(NULL, " ", &rest);
				if (strlen(tok) == 2 && tok[0] == 'R') {
					assembled |= (tok[1] - 48) & mask(0, 3);
				}
				else if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= mask(5, 1);
					int val = atoi(tok);
					if (val > 15 || val < -16) {
						printf("[l%d] ADD immediate value must be in range [-15;16]\n", line);
						exit(1);
					}
					assembled |= ((uint16_t)val & mask(0, 5));
				} } break;
			case AND: {
				assembled |= AND << 12;
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP << 9;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP << 9;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3)) << 9;
				}
				tok = __strtok_r(NULL, " ", &rest);
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP << 3;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP << 3;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3)) << 3;
				}
				tok = __strtok_r(NULL, " ", &rest);
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3));
				} } break;
			case NOT: {
				assembled |= NOT << 12;
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP << 9;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP << 9;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3)) << 9;
				}
				tok = __strtok_r(NULL, " ", &rest);
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3));
				} } break;
			case CMP: {
				assembled |= CMP << 12;
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP << 9;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP << 9;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3)) << 9;
				}
				tok = __strtok_r(NULL, " ", &rest);
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3));
				} } break;
			case PRNT:
				assembled |= PRNT << 12;
				if (strlen(tok) == 2 && tok[0] == 'S') {
					assembled |= _SP;
				}
				else if (strlen(tok) == 2 && tok[0] == 'I') {
					assembled |= _IP;
				}
				else {
					assembled |= ((tok[1] - 48) & mask(0, 3));
				} break;
			case STOP:
				assembled |= mask(0, 16);
				break;
		}
		fwrite(&assembled, 1, sizeof(uint16_t), output);
	}
}

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("usage: ./tiny-comp <source> <output>\n");
		exit(1);
	}

	assemble(argv[1], argv[2]);
	return 0;
}

