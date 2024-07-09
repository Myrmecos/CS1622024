#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

void signal_handler(int signum) {
    // Handle the signal here
    printf("Received signal %d\n", signum);
}

int main() {
    pid_t child_pid;
    pid_t fg_pgrp;

    // Fork a new child process
    child_pid = fork();

    if (child_pid == -1) {
        // Fork failed
        perror("fork");
        return 1;
    } else if (child_pid == 0) {
        // Child process
        printf("Child process, my PID is %d\n", getpid());

        // Change the child's process group to its own PID
        if (setpgid(0, 0) == -1) {
            perror("setpgid");
            return 1;
        }

        // Get the current foreground process group
        fg_pgrp = tcgetpgrp(STDIN_FILENO);
        if (fg_pgrp == -1) {
            perror("tcgetpgrp");
            return 1;
        }

        // Set the child process to the foreground process group
        printf("before changed to fg\n");
        if (tcsetpgrp(STDIN_FILENO, getpid()) == -1) {
            perror("tcsetpgrp");
            return 1;
        }
        printf("changed to fg\n");

        // Set up a signal handler for SIGINT (Ctrl+C)
        struct sigaction sa;
        sa.sa_handler = signal_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if (sigaction(SIGINT, &sa, NULL) == -1) {
            perror("sigaction");
            return 1;
        }

        // Wait for user input or other work
        while (1) {
            pause();
        }
    } else {
        // Parent process
        printf("Parent process, my PID is %d, and the child's PID is %d\n", getpid(), child_pid);

        // Wait for the child process to finish
        int status;
        if (waitpid(child_pid, &status, 0) == -1) {
            perror("waitpid");
            return 1;
        }
    }

    return 0;
}