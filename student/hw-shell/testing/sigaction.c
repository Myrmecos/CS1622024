#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
void signal_callback_handler(int signum) {
printf("Caught signal!\n");
exit(1);
}
int main() {
    pid_t pid = fork();
    if (pid == 0) {
        struct sigaction sa;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sa.sa_handler = signal_callback_handler;
        sigaction(SIGINT, &sa, NULL);
        while (1) {}
    }
    else if (pid > 0) {
        wait(NULL);
        printf("end\n")
    }
}