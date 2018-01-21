#include "smart.h"

#include <stdio.h>
#include <stdlib.h>

// main returns to __libc_start_main
// so it is not a special case.
int main() {
	printf("main\n");
	int *x = malloc(sizeof(int));
	free_on_exit((void*)x);
}