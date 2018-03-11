#include "smart.h"

#include <stdio.h>
#include <stdlib.h>

// These limits are arbitrary. This is static sizes for simplicity.
#define STACK_SIZE 256
#define MAX_PER_FRAME 32
// This allocates STACK_SIZE * MAX_PER_FRAME * <something> bytes

typedef struct {
	// Actually the caller_ebp is probably not necessary if we assume that the stack is right.
	int caller_ebp; // ebp of the caller. This identifes the frame during do_free.
	int caller_eip; // so we can fix the stack.
	void* tracked_pointers[MAX_PER_FRAME];
	int tail; // points to one past last entry
} garbage_stack_entry_t;

typedef struct {
	garbage_stack_entry_t stack[STACK_SIZE];
	int tail; // points to one past last entry
} garbage_stack_t;


void thunk();

garbage_stack_t garbage = {0};

int do_free() {
	printf("Now time to free\n");
	printf("Let's determine if this is the right stack frame.\n");
	// I think it's safe to assume that the top of the stack has the right address to return to.
	garbage_stack_entry_t* entry = garbage.stack + (garbage.tail - 1);
	garbage.tail--; // pop
	for (int i = 0; i < MAX_PER_FRAME; i++) {
		if (entry->tracked_pointers[i] == 0) break;
		free (entry->tracked_pointers[i]);
	}
	return entry->caller_eip;
}

void* free_on_exit(void* entry) {
	// first let's get the return address of the caller, and also hijack it to ours.
	int ret_addr = 0;
	int do_free_addr = (int)&do_free;

	/*
	Stack:
	f0
	f1
	free_on_exit
	*/

	// this allows us to distinguish the stack frame during "do_free"
	int* f1_ebp, *f0_ebp; // point to locations on the stack
	int f0_eip; // value of *(f1_ebp + 1)

	// Store f0 ebp/eip, and hijack f1's return eip to go to do_free.

	__asm__ __volatile__ (
	"movl (%%ebp), %0 \n"  // first get location of f0's ebp
	: "=r" (f1_ebp) // output
	: // input
	: // clobbered
	);

	f0_ebp = (int*) *(f1_ebp);
	f0_eip = *(f1_ebp + 1); // four bytes up the stack. It is pushed by f0.

	// This should (hopefully) point to correct location.
	printf("got f1_ebp=%x, f0_ebp=%x, f0_eip=%x\n", f1_ebp, f0_ebp, f0_eip);

	// Hijack f0's eip to return to do_free.
	// Not this may have already been done on a previous call to free_on_exit.
	*(f1_ebp + 1) = (int) thunk;

	printf("hijacked eip\n");

	// Check if there is already a stack entry for this frame. Identified by
	// f0_ebp because we lose the eip, and when we get to do_free we've popped f1's ebp.
	garbage_stack_entry_t* garbage_entry;
	if (garbage.stack[garbage.tail - 1].caller_ebp == f0_ebp) {
		// re-use.
		printf("already have entry\n");
		garbage_entry = garbage.stack + garbage.tail - 1;
	} else {
		printf("making new entry\n");
		// make a new entry.
		garbage_entry = garbage.stack + garbage.tail++;
		memset (garbage_entry, 0, sizeof(*garbage_entry));
		garbage_entry->caller_ebp = f0_ebp;
		garbage_entry->caller_eip = f0_eip;
	}

	// TODO: check bounds.
	garbage_entry->tracked_pointers[garbage_entry->tail++] = entry;
	return entry;
}