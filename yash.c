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
int pid_ch1, pid_ch2, pid;

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
    int status;
    while(1){
        // Read in the input and tokenize the string
        printf("# ");
        fgets(buf, 2000, stdin);
        char* token = strtok(buf, " ");
        char** tokenz = NULL;
        int num_spaces = 0;
        while(token){
            // Sometimes the input is dumb and the token has a \n at the end...
            if(strstr(token, "\n"))
                token[strlen(token)-1] = '\0';
            tokenz = realloc(tokenz, sizeof(char*) * ++num_spaces);
            if(tokenz == NULL)
                exit(-1); //mem alloc failed.
            tokenz[num_spaces-1] = token;
            token = strtok(NULL, " ");
        }
        tokenz = realloc(tokenz, sizeof(char*) * ++num_spaces);
        tokenz[num_spaces-1] = NULL;
        int tokenz_len = num_spaces;

        pid_t cpid = fork();
        if(cpid == 0){
            // i is the current pos.
            // j is the last start pos.
            int i = 0, j = 0;
            char* special = "<>2>|";
            while((token = tokenz[i]) != NULL){
                if(strstr(special, token)){
                    int k = j;
                    int s = i - j + 1;
                    for(k; k < s; k++){
                        
                    }
                }
            }
            execvp(tokenz[0], tokenz);
            free(tokenz);
        }
        else{
            wait(NULL);
        }
        // DO STUFF HERE
    }
}
