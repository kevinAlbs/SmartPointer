#include "smart.h"

#include <stdio.h>
#include <stdlib.h>

void f() {
    char *data = free_on_exit (malloc (sizeof (char)));
}

int
main ()
{
   printf ("run me with valgrind\n");
   f();
}