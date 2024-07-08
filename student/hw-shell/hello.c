#include<stdio.h>

int main(int argc, char* argv[]) {
    printf("hello there!\n");
    for (int i = 0; i < argc; i++) {
        printf("The %dth argument: %s\n", i, argv[i]);
    }
}