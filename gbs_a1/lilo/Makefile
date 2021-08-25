-include ../common_abgabe.mk
CC = gcc
CFLAGS = -std=c11 -pedantic -D_XOPEN_SOURCE=700 -Wall -Werror

lilo: lilo.c ${MAKEFILE_LIST}
	${CC} ${CFLAGS} -o lilo lilo.c

clean:
	rm lilo

test:
	python3 tests/unittest.py -t tests/

.PHONY: clean test
