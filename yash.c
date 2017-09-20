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
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

typedef struct job *Job;

struct process {
    pid_t pid;
    int status; // Return code from waitpid
    int stopped;
    int completed;
};

struct job {
    int number; // job id
    pid_t pgid;
    struct process procs[2]; // list of procs
    char text[2000]; // line submitted
};

Job jobs[100];
int job_num = 0;

int cpid, cpid2, pid, status;

static void sig_int(int signo) {
    printf("Sending signals to group:%d\n",cpid); // group id is pid of first in pipeline
    kill(-cpid,SIGINT);
}
static void sig_tstp(int signo) {
    printf("Sending SIGTSTP to group:%d\n",cpid); // group id is pid of first in pipeline
    kill(-cpid,SIGTSTP);
}

void builtins(char* cmd){
    if(strcmp(cmd, "fg") == 0){
        int i = 0;
        while(jobs[i] != NULL){
            
            i++;
        }
    }
    else if(strcmp(cmd, "bg") == 0){
        int i = 0;
        while(jobs[i] != NULL){
            if(jobs[i]->pgid == 1){
                kill(-jobs[i]->pgid, SIGCONT);
                break;
            }
            i++;
        }
    }
    else if(strcmp(cmd, "jobs") == 0){

    }
}

void eval_cmds(int len, int maxlen, char* arr[len][maxlen]){
    int i;
    int inputfd, outputfd, errfd;
    int pipefd[2];
    int bg = 0;
    // if need to send job to bg
    if(strcmp(arr[len-2][0], "&") == 0)
        bg = 1;

    int pipe_index = -1;
    for(i = 0; i < len; i++){
        if(arr[i][0] == NULL)
            break;
        if(strcmp(arr[i][0],"|") == 0)
            pipe_index = i;
    }
    if(pipe_index < 0){ // No pipe in the job
        cpid = fork();
        if(cpid == 0){
            setsid();
            for(i = 0; i < len; i++){
                if(arr[i][0] == NULL)
                    break;
                if(strcmp(arr[i][0],"<") == 0){
                    inputfd = open(arr[i+1][0], O_RDONLY);
                    if(errno == ENOENT){
                        printf("Error opening file.\n");
                        exit(1);
                    }
                    else{
                        dup2(inputfd, STDIN_FILENO);
                    }
                }
                if(strcmp(arr[i][0],">") == 0){
                    outputfd = open(arr[i+1][0], O_CREAT | O_WRONLY, S_IRGRP | S_IRUSR | S_IWGRP | S_IWUSR);
                    dup2(outputfd, STDOUT_FILENO);
                }
                if(strcmp(arr[i][0],"2>") == 0){
                    errfd = open(arr[i+1][0], O_CREAT | O_WRONLY, S_IRGRP | S_IRUSR | S_IWGRP | S_IWUSR);
                    dup2(errfd, STDERR_FILENO);
                }
            }
            execvp(arr[0][0], arr[0]);
        }
        else{
            int count = 0;
            while(count < 1){
                pid = waitpid(-1, &status, WUNTRACED | WCONTINUED);
                if (pid == -1)
                {
                    return;
                }
                if (WIFEXITED(status))
                {
                  count++;
                }
                else if (WIFSIGNALED(status))
                {
                    printf("child %d killed by signal %d\n", pid, WTERMSIG(status));
                    kill(pid, SIGCONT);
                    count++;
                }
                else if (WIFSTOPPED(status))
                {
                    // printf("%d stopped by signal %d\n", pid, WSTOPSIG(status));
                    // printf("Sending CONT to %d\n", pid);
                    // sleep(4); //sleep for 4 seconds before sending CONT
                    // kill(pid, SIGCONT);
                    return;
                }
                else if (WIFCONTINUED(status))
                {
                    printf("Continuing %d\n", pid);
                }
            }
        }
    }
    else{ // 1 pipe in the job
        if (pipe(pipefd) < 0)
	        fprintf(stderr,"pipe error");
        cpid = fork();
        if(cpid == 0){
            setsid(); // child 1 creates a new session and a new group and becomes leader with PID cpid
            dup2(pipefd[1], STDOUT_FILENO);
            for(i = 0; i < len; i++){
                if(arr[i][0] == NULL)
                    break;
                if(strcmp(arr[i][0],"<") == 0){
                    inputfd = open(arr[i+1][0], O_RDONLY);
                    if(errno == ENOENT){
                        printf("Error opening file.\n");
                        exit(1);
                    }
                    else{
                        dup2(inputfd, STDIN_FILENO);
                    }
                }
                if(strcmp(arr[i][0],">") == 0){
                    outputfd = open(arr[i+1][0], O_CREAT | O_WRONLY, S_IRGRP | S_IRUSR | S_IWGRP | S_IWUSR);
                    dup2(outputfd, STDOUT_FILENO);
                }
                if(strcmp(arr[i][0],"2>") == 0){
                    errfd = open(arr[i+1][0], O_CREAT | O_WRONLY, S_IRGRP | S_IRUSR | S_IWGRP | S_IWUSR);
                    dup2(errfd, STDERR_FILENO);
                }
            }
            close(pipefd[0]);
            execvp(arr[0][0], arr[0]);
            exit(1);
        }
        else{
            cpid2 = fork();
            if(cpid2 == 0){
                sleep(1);
                setpgid(0, cpid);
                dup2(pipefd[0], STDIN_FILENO);
                for(i = pipe_index+1; i < len; i++){
                    if(arr[i][0] == NULL)
                        break;
                    if(strcmp(arr[i][0],"<") == 0){
                        inputfd = open(arr[i+1][0], O_RDONLY);
                        if(errno == ENOENT){
                            printf("Error opening file.\n");
                            exit(1);
                        }
                        else{
                            dup2(inputfd, STDIN_FILENO);
                        }
                    }
                    if(strcmp(arr[i][0],">") == 0){
                        outputfd = open(arr[i+1][0], O_CREAT | O_WRONLY, S_IRGRP | S_IRUSR | S_IWGRP | S_IWUSR);
                        dup2(outputfd, STDOUT_FILENO);
                    }
                    if(strcmp(arr[i][0],"2>") == 0){
                        errfd = open(arr[i+1][0], O_CREAT | O_WRONLY, S_IRGRP | S_IRUSR | S_IWGRP | S_IWUSR);
                        dup2(errfd, STDERR_FILENO);
                    }
                }
                close(pipefd[1]);
                execvp(arr[pipe_index+1][0], arr[pipe_index+1]);
                exit(1);
            }
            else{
                //close the pipe in the parent
                close(pipefd[0]);
                close(pipefd[1]);
                if(bg){
                }
                int count = 0;
                while (count < 2){
                    pid = waitpid(-1, &status, WUNTRACED | WCONTINUED);
                    if (pid == -1)
                    {
                      perror("waitpid");
                      exit(EXIT_FAILURE);
                    }
            
                    if (WIFEXITED(status))
                    {
                      count++;
                    }
                    else if (WIFSIGNALED(status))
                    {
                      printf("child %d killed by signal %d\n", pid, WTERMSIG(status));
                      count++;
                    }
                    else if (WIFSTOPPED(status))
                    {
                      printf("%d stopped by signal %d\n", pid, WSTOPSIG(status));
                      printf("Sending CONT to %d\n", pid);
                      sleep(4); //sleep for 4 seconds before sending CONT
                      kill(pid, SIGCONT);
                    }
                    else if (WIFCONTINUED(status))
                    {
                      printf("Continuing %d\n", pid);
                    }
                }
            }
        }
    }
}

void parse_stuff(int len, char* arr[]){
    int i, j=1, k=0;
    // Counts how much mem needed to allocate
    for(i = 0; i < len; i++){
        if(arr[i] == NULL)
            break;
        if(strcmp(arr[i], "<") == 0){
            j++;
        }
        else if(strcmp(arr[i], ">") == 0){
            j++;
        }
        else if(strcmp(arr[i], "2>") == 0){
            j++;
        }
        else if(strcmp(arr[i], "|") == 0){
            j++;
        }
        else if(strcmp(arr[i], "&") == 0){
            j++;
        }
    }
    char* cmds[j + j][len + 1];
    int cmds_len = j + j;
    j = 0;
    for(i = 0; i < len; i++){
        if(arr[i] == NULL){
            cmds[j][k] = NULL;
            break;
        }
        if(strcmp(arr[i], "<") == 0){
            cmds[j][k] = NULL;
            j++;
            cmds[j][0] = arr[i];
            cmds[j][1] = NULL;
            j++;
            k = 0;
        }
        else if(strcmp(arr[i], ">") == 0){
            cmds[j][k] = NULL;
            j++;
            cmds[j][0] = arr[i];
            cmds[j][1] = NULL;
            j++;
            k = 0;
        }
        else if(strcmp(arr[i], "2>") == 0){
            cmds[j][k] = NULL;
            j++;
            cmds[j][0] = arr[i];
            cmds[j][1] = NULL;
            j++;
            k = 0;
        }
        else if(strcmp(arr[i], "|") == 0){
            cmds[j][k] = NULL;
            j++;
            cmds[j][0] = arr[i];
            cmds[j][1] = NULL;
            j++;
            k = 0;
        }
        else if(strcmp(arr[i], "&") == 0){
            cmds[j][k] = NULL;
            j++;
            cmds[j][0] = arr[i];
            cmds[j][1] = NULL;
            j++;
            k = 0;
        }
        else{
            cmds[j][k] = arr[i];
            k++;
        }
    }
    free(arr);
    eval_cmds(cmds_len, len+1, cmds);
}

void prompt(void){
    char buf[2000];
    printf("# ");
    if(fgets(buf, 2000, stdin) == NULL)
        exit(0);
    if(strcmp(buf, "fg") == 0 
        || strcmp(buf, "bg") == 0
        || strcmp(buf, "jobs") == 0){
        builtins(buf);
    }
    else{
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
            token = strtok(NULL, "  ");
        }
        tokenz = realloc(tokenz, sizeof(char*) * ++num_spaces);
        tokenz[num_spaces-1] = NULL;
        int tokenz_len = num_spaces; // Size of tokenz array
        parse_stuff(tokenz_len, tokenz);
    }
}

int main (void) {
    if (signal(SIGINT, sig_int) == SIG_ERR)
        printf("signal(SIGINT) error");
    if (signal(SIGTSTP, sig_tstp) == SIG_ERR)
        printf("signal(SIGTSTP) error");
    while(1){
        prompt();
    }
}
