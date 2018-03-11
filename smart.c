#include "smart.h"

#include <string.h> // memset
#include <stdlib.h> // free

/* these limits are arbitrary. Use static sizes for simplicity. */
#define STACK_SIZE 256
#define MAX_PER_FRAME 32

typedef struct {
	int caller_ebp; /* ebp of the caller. This identifes the frame during do_free. */
	int caller_eip; /* the original return eip of the caller. */
	void* tracked_pointers[MAX_PER_FRAME];
	int tail; /* points to one past last entry. */
} garbage_stack_entry_t;

typedef struct {
	garbage_stack_entry_t stack[STACK_SIZE];
	int tail; /* points to one past last entry. */
} garbage_stack_t;


/* forward declare the assembly trampoline. */
void trampoline();

garbage_stack_t garbage = {0};

int do_free() {
	garbage_stack_entry_t* entry = garbage.stack + (garbage.tail - 1);
	garbage.tail--; /* pop. */
	for (int i = 0; i < MAX_PER_FRAME; i++) {
		if (entry->tracked_pointers[i] == 0) break;
		free (entry->tracked_pointers[i]);
	}
	return entry->caller_eip;
}

void* free_on_exit(void* entry) {
	int ret_addr = 0;
	int do_free_addr = (int)&do_free;
	int* caller_ebp;

	/* get the value of ebp. */
	__asm__ __volatile__ (
	"movl (%%ebp), %0 \n"
	: "=r" (caller_ebp) /* output. */
	);

	/* check if there is a pre-existing stack entry for this caller (identified by caller's ebp). */
	garbage_stack_entry_t* garbage_entry;

	if (garbage.stack[garbage.tail - 1].caller_ebp == (int)caller_ebp) {
		/* re-use. */
		garbage_entry = garbage.stack + garbage.tail - 1;
	} else {
		/* make a new entry. */
		garbage_entry = garbage.stack + garbage.tail++;
		memset (garbage_entry, 0, sizeof(*garbage_entry));
		garbage_entry->caller_ebp = (int)caller_ebp;
		/* hijack caller's return eip to return to do_free. */
		garbage_entry->caller_eip = *(caller_ebp + 1);
		*(caller_ebp + 1) = (int) trampoline;
	}

	garbage_entry->tracked_pointers[garbage_entry->tail++] = entry;
	return entry;
}
