SRC_1 = tiny-vm.c
SRC_2 = tiny-assembler.c
CC_FLAGS = -Wall -Wextra -std=c11
CC = gcc

all:
	${CC} ${SRC_1} ${CC_FLAGS} -o tiny-vm
	${CC} ${SRC_2} ${CC_FLAGS} -o tiny-assembler
