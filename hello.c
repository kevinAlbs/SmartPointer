#include <stdio.h>
int main(int argc, char** argv, char** envp) {
    while(*envp) {
        printf("%s\n", envp[0]);
        envp++;    
    }
}
