#include "smart.h"

#include <stdlib.h> // free
#include <string.h> // memset

/* these limits are arbitrary. */
#define STACK_SIZE 256
#define MAX_PER_FRAME 32

typedef struct {
   int caller_ebp; /* ebp of the caller. This identifes the frame. */
   int caller_eip; /* the original return eip of the caller. */
   void *tracked_pointers[MAX_PER_FRAME];
   int tail; /* points to one past last entry. */
} tracked_stack_entry_t;

typedef struct {
   tracked_stack_entry_t stack[STACK_SIZE];
   int tail; /* points to one past last entry. */
} tracked_stack_t;

/* forward declare the assembly trampoline. */
void trampoline ();

tracked_stack_t tracked = {0};

int do_free () {
   tracked_stack_entry_t *entry = tracked.stack + (tracked.tail - 1);
   tracked.tail--; /* pop. */
   for (int i = 0; i < MAX_PER_FRAME; i++) {
      if (entry->tracked_pointers[i] == 0) break;
      free (entry->tracked_pointers[i]);
   }
   return entry->caller_eip;
}

void *free_on_exit (void *entry) {
   int ret_addr = 0;
   int do_free_addr = (int) &do_free;
   int *caller_ebp;

   /* get the value of ebp. */
   __asm__("movl (%%ebp), %0 \n"
           : "=r"(caller_ebp) /* output. */
           );

   /* check if there is a pre-existing stack entry for this caller
    * (identified by caller's ebp). */
   tracked_stack_entry_t *tracked_entry;

   if (tracked.tail > 0 &&
       tracked.stack[tracked.tail - 1].caller_ebp == (int) caller_ebp) {
      /* re-use. */
      tracked_entry = tracked.stack + tracked.tail - 1;
   } else {
      /* make a new entry. */
      tracked_entry = tracked.stack + tracked.tail++;
      memset (tracked_entry, 0, sizeof (*tracked_entry));
      tracked_entry->caller_ebp = (int) caller_ebp;
      /* hijack caller's return eip to return to do_free. */
      tracked_entry->caller_eip = *(caller_ebp + 1);
      *(caller_ebp + 1) = (int) trampoline;
   }

   /* push the passed pointer. */
   tracked_entry->tracked_pointers[tracked_entry->tail++] = entry;
   return entry;
}
