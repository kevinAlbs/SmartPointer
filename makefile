# compile on 32 bit architecture without compiler optimizations
all: test smart

test: test.c smart.c trampoline.S
	gcc -o test -O0 -m32 test.c smart.c trampoline.S 

smart: main.c smart.c trampoline.S
	gcc -o smart -O0 -m32 main.c smart.c trampoline.S 

format: main.c smart.c test.c smart.h
	clang-format -style=file -i main.c smart.c test.c smart.h

clean:
	rm smart test
