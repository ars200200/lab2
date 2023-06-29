#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define MAX_COMMANDS_NUMBER 16
#define MAX_COMMANDS_ARGUMENTS 256

#define MAX_INPUT_BUFFER_SIZE MAX_COMMANDS_NUMBER * MAX_COMMANDS_ARGUMENTS

//ls | grep lab2

int main(int argc, char* argv[]){
    char* cmds[MAX_COMMANDS_NUMBER][MAX_COMMANDS_ARGUMENTS];
    
    char input_buff[MAX_INPUT_BUFFER_SIZE];
    fgets(input_buff, sizeof(input_buff), stdin); 
    kill(getppid(), SIGUSR1);
    
    //parse input_buff into commands and its args    
    char* input_token = strtok(input_buff, "|");
    size_t cmd_cntr = 0;
    while(input_token != NULL){
        cmds[cmd_cntr++][0] = input_token;
        input_token = strtok(NULL, "|");
    }
    cmds[cmd_cntr][0] = input_token;
   
    //spliting commands and args
    for(size_t i=0; i<cmd_cntr; ++i){
        char* arg_token = strtok(cmds[i][0], " \n");
        size_t args_cntr = 0;
        while(arg_token != NULL){
            cmds[i][args_cntr++] = arg_token;
            arg_token = strtok(NULL, " \n");
        }
    }

    int fd[cmd_cntr-1][2];
    for(size_t i=0; i<cmd_cntr-1; ++i)
        pipe(fd[i]); 
    
    for(size_t i=0; i<cmd_cntr; ++i){
        pid_t child_pid = fork();
        if(child_pid < 0) printf("Fork error");
        else if(child_pid == 0){
            if(i==0) dup2(fd[i][1], STDOUT_FILENO);
            else if(i+1==cmd_cntr) dup2(fd[i-1][0], STDIN_FILENO);
            else{
                dup2(fd[i-1][0], STDIN_FILENO);
                dup2(fd[i][1], STDOUT_FILENO);
            }
            for(size_t j=0; j<cmd_cntr; ++j){
                close(fd[j][0]);
                close(fd[j][1]);
            }
            execvp(cmds[i][0], cmds[i]);
        }
    } 
    for(size_t j=0; j<cmd_cntr; ++j){
        close(fd[j][0]);
        close(fd[j][1]);
    }    
    wait(NULL);
    kill(getppid(), SIGUSR2);
    return 0;
}
