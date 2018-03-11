#include "smart.h"

#include <stdio.h>
#include <stdlib.h>

void f1() {
    printf("enter f1\n");
    int f1local = 0xF1;
    int *x = malloc(sizeof(int));
    free_on_exit((void*)x);
    printf("exit f1\n");
}

void f0() {
    printf("enter f0\n");
    int f0local = 0xF0;
    f1();
    printf("exit f0\n");
}
// main returns to __libc_start_main
// so it is not a special case.
int main() {
	printf("main\n");
    f0();
}