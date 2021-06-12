CFLAGS=-Wall -O3 -march=znver2

.PHONY: all clean

all: comm

test: comm
	./runtests.sh

clean:
	rm -f comm


