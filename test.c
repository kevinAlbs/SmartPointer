#include <stdlib.h>

#include "smart.h"

static void test_single() {
    char *x = (char*)free_on_exit(malloc(sizeof(char)));
}

static void test_multiple() {
    char *x[] = {0, 0, 0};
    x[0] = (char*)free_on_exit(malloc(sizeof(char)));
    x[1] = (char*)free_on_exit(malloc(sizeof(char)));
    x[2] = (char*)free_on_exit(malloc(sizeof(char)));
}

static void test_nested() {
    char *x = (char*)free_on_exit(malloc(sizeof(char)));
    test_single();
    test_multiple();
}

/* Run this with valgrind. You may need to install 32 bit debug
 * symbols for the C library. On Ubuntu this is libc6-dbg:i386.
 * See https://stackoverflow.com/q/10172302/774658 */
int main() {
	test_single();
    test_multiple();
    test_nested();
    /* test that free_on_exit works from main. */
    char *x = (char*) free_on_exit(malloc(sizeof(char)));
}