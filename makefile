testing: testing.c
	# compile with -m32 since 64 bit calling conventions pass args through registers
	# and is just generally harder to follow.
	gcc -g -o testing -O0 -m32 testing.c test.S

smart: main.c smart.c thunk.S
	gcc -g -o main -O0 -m32 main.c smart.c thunk.S