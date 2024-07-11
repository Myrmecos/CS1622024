#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main() {
    int num_of_proc = 10;
    int * fd_arr = malloc(sizeof(int) * (num_of_proc - 1) * 2);
    int * pipe_arr = malloc(sizeof(int) * (num_of_proc - 1));
    char* mystr = malloc(sizeof(char) * 64);

    for (int i = 0; i < num_of_proc - 1; i++) {
        pipe_arr[i] = pipe(&(fd_arr[i*2]));
    }
    pid_t this_id;
    for (int i = 0; i < num_of_proc; i++) {
        this_id = fork();
        if (this_id == 0) { //child process
            //marginal case: first process
            if (i != 0) {
                dup2(fd_arr[i*2-2], STDIN_FILENO);
            }
            //marginal case: last process
            if (i != num_of_proc - 1) {
                dup2(fd_arr[i*2+1], STDOUT_FILENO);
            }
            
            //close all other file descriptors
            for (int j = 0; j < (num_of_proc - 1) * 2; j++) {
                    close(fd_arr[j]);
            }
            
            if (i == 0) {
                printf("hello from thread 0\n");
            }
            else if (i == num_of_proc - 1) {
                printf("hi there this is the last process\n");
                fgets(mystr, 64, stdin);
                printf("%s", mystr);
            }
            else {
                fgets(mystr, 64, stdin);
                printf("%s", mystr);
            }
            
            //close(STDIN_FILENO);
            //close(STDOUT_FILENO);
            
           //printf("hello from thread!\n");
           break;
        } else {wait(NULL);}
    }

}