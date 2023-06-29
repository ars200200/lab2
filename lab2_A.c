#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <signal.h>

#define SECONDS_TO_WAIT 5

pid_t child_pid;

void sig_handler(int sig) {
    printf("Signal from B received!\n");
    
    sleep(5); 
    kill(child_pid, SIGTERM);
    
    printf("Out of time for B!\n");
    exit(1);
}

void B_Ended(int sig){
    printf("B finished his work!\n");
    exit(0);
}

int main() {
    signal(SIGUSR1, sig_handler); // Register signal handler for SIGUSR1
    signal(SIGUSR2, B_Ended); // Register signal handler for SIGUSR2
    char* argv[] = {"./lab2_B", NULL};
    if ((child_pid = fork()) == 0) {
        execv(argv[0], argv); // Execute program B
        exit(1);
    } else {
        wait(NULL); // Wait for child process to terminate
    }
    return 0;
}

//ls | grep lab2
