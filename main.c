#include "smart.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    int x = 5;
    char* data = free_on_exit(malloc(sizeof(char)));
    printf("run me with valgrind\n");
}