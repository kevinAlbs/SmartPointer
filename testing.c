#include <stdio.h>

int test_asm();
/*
Some useful gdb stuff
x/20x $esp # shows 20 words from top of stack
info breakpoints # list breakpoints
*/

int func(int arg1, int arg2);

int* funcAddr;

// Primary issue with this, is that when we return from func with a trampoline, we need to push another return address.
// saving eip is necessarily the responsibility of the caller.
int trampoline() {
	int x = 0x1234;
}

// Let's see if inline assembly can write the entire function.
int trampoline2() {
	// Issue with this, is that the ebp push is already added as part of the function definition.
	// volatile so it doesn't reorder
	// note a literal % (as in register) needs to be escaped as %%
	__asm__ __volatile__ (
		"pushl %0\n" // instructions
		: // output
		: "m" (funcAddr) // input
		);
}

// I need to define a function probably in a separate assembly file and link it here.


// What does the call stack look like on x86_64 for a function call?
// Can I redirect to a trampoline?
// Can I add arguments to that redirection?
int func(int arg1, int arg2) {
	int a = 1;
	int b = 2;
	// no-op.
	// try to set a register value.
	// note, this is AT-T syntax, 
	int* ptr = (&arg1);
	// args are pushed before saved eip (in last frame).
	// so eip is the last argument pushed (first arg, b/c variadic args) + width of 1st arg.
	int* saved_eip = ptr - 1;
	*saved_eip = &trampoline;
}

int main() {
	// int x=0xDEAD; // These locals get pushed onto both main and func stackframes.
	// int y=0xBEEF;
	// func(x, y);
	//funcAddr = (int*)&func;
	//trampoline2();
	int x = test_asm();
	printf("got %d\n", x);
}
