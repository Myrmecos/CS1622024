#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char* argv[]) {
    printf("hello there!\n");
    for (int i = 0; i < argc; i++) {
        printf("The %dth argument: %s\n", i, argv[i]);
    }
    char* buffer = malloc(sizeof(char) * 64);
    int cnt = 3;
    while (scanf("%s", buffer) != EOF) {
        printf("%s\n", buffer);
        cnt --;
        if (cnt == 0) {break;}
    }
}