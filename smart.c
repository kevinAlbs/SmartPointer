#include "smart.h"

#include <stdio.h>

typedef struct {
	
} garbage_stack_t;

void free_on_exit(void* entry) {
	// first let's get the return address of the caller.
	int retAddr;
	__asm__ __volatile__ (
	"movl %%esp, %0 \n"
	: "=r" (retAddr)
	:
	: "%eax"
	);
	printf("got retAddr=%x\n", retAddr);
}