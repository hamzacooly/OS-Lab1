// Hamza Khatri
// hak533
// Fall 2017
// M/W 9-10:30
// yash.c

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int pipefd[2];
int status, pid_ch1, pid_ch2, pid;

static void sig_int(int signo) {
    printf("Sending signals to group:%d\n",pid_ch1); // group id is pid of first in pipeline
    kill(-pid_ch1,SIGINT);
}
static void sig_tstp(int signo) {
    printf("Sending SIGTSTP to group:%d\n",pid_ch1); // group id is pid of first in pipeline
    kill(-pid_ch1,SIGTSTP);
}

int main (void) {
    // Buffer for a max of 2k chars
    char buf[2000];
    while(1){
        // Read in the input and tokenize the string
        printf("yash: ");
        fgets(buf, 2000, stdin);
        char* token = strtok(buf, " ");
        char** tokenz = NULL;
        int num_spaces = 0;
        while(token){
            tokenz = realloc(tokenz, sizeof(char*) * ++num_spaces);
            if(tokenz == NULL)
                exit(-1); //mem alloc failed.
            tokenz[num_spaces-1] = token;
            token = strtok(NULL, " ");
        }
        tokenz = realloc(tokenz, sizeof(char*) * ++num_spaces);
        tokenz[num_spaces-1] = 0;

        pid_t cpid = fork();
        if(cpid == 0){
            printf("Child process! %s\n", tokenz[0]);
            execvp(tokenz[0], NULL);
            wait(&cpid);
        }
        else{
            printf("Parent\n");
        }
        // DO STUFF HERE
        free(tokenz);
    }
}
