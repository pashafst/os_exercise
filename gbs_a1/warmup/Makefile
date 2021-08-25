-include ../common_abgabe.mk
CC = gcc
CFLAGS = -std=c11 -pedantic -D_XOPEN_SOURCE=700 -Wall -Werror

hello: hello.c ${MAKEFILE_LIST}
	${CC} ${CFLAGS} -o $@ $<

clean:
	rm hello

test:
	python3 tests/unittest.py -t tests/

.PHONY: clean test
