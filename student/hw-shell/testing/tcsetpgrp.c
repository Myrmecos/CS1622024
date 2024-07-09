#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        pid_t current_pid = getpid();
        pid_t current_pgid = getpgrp();

        printf("Current PID: %d\n", current_pid);
        printf("Current PGID: %d\n", current_pgid);

        // Move the current process group to the foreground
        if (tcsetpgrp(STDIN_FILENO, current_pgid) == -1) {
            perror("tcsetpgrp");
            return 1;
        }

        printf("Process group moved to foreground.\n");

        // Wait for the user to press Ctrl+C to exit
        while (1) {
            pause();
        }
    }
    if (pid > 0) {
        wait(NULL);
    }

    return 0;
}