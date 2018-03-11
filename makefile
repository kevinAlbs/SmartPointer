test: test.c smart.c thunk.S
	# compile on 32 bit architecture.
	gcc -g -o test -O0 -m32 test.c smart.c thunk.S

smart: main.c smart.c thunk.S
	gcc -g -o smart -O0 -m32 main.c smart.c thunk.S

clean:
	rm smart test