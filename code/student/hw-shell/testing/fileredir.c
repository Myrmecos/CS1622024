#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        freopen("hi.txt", "w", stdout);
        FILE* myfile = fopen("hello.txt", "r");
        char buffer[256];

        while (fgets(buffer, 256, myfile) != 0) {
            fputs(buffer, stdout);
        }
        fclose(stdout);
        exit(0);
    }
    else if (pid > 0) {
        printf("hello\n");
        wait(NULL);
    }
}