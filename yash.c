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

typedef struct process *Process;
typedef struct job *Job;

struct process {
    struct process *next;
    pid_t pid;                  /* process id                       */
    char text[2000];	/* text to print when 'jobs' is run */
    int status;			/* return code from waitpid */
};

struct job {
    pid_t gleader;		/* process group leader of this job  */
    pid_t other;		/* subjob id (SUPERJOB)
				 * or subshell pid (SUBJOB) */
    int stat;                   /* see STATs below                   */
    struct process *procs;	/* list of processes                 */
};

// static void sig_int(int signo) {
//     printf("Sending signals to group:%d\n",pid_ch1); // group id is pid of first in pipeline
//     kill(-pid_ch1,SIGINT);
// }
// static void sig_tstp(int signo) {
//     printf("Sending SIGTSTP to group:%d\n",pid_ch1); // group id is pid of first in pipeline
//     kill(-pid_ch1,SIGTSTP);
// }

void builtins(char* cmd){
    if(strcmp(cmd, "fg") == 0){

    }
    else if(strcmp(cmd, "bg") == 0){

    }
    else if(strcmp(cmd, "jobs") == 0){

    }
}

// void redirects(int type, char* before[], char* file){
//     if(type == 0){ // "<"
//         int inputfd = open(file, O_RDONLY);
//         if(errno == ENOENT){
//             printf("Error opening file.\n");
//             exit(1);
//         }
//         else{
//             dup2(inputfd, STDIN_FILENO);
//         }
//     }
//     else if(type == 1){ // ">"

//     }
//     else if(type == 2){ // "2>"

//     }
// }

void eval_cmds(int len, int maxlen, char* arr[len][maxlen]){
    int status;
    int i;
    int inputfd, outputfd, errfd;
    int pipefd[2];
    int pid_ch1, pid_ch2, pid;
    int bg = 0;
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
        pid_t cpid = fork();
        if(cpid == 0){
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
            wait(&status);
        }
    }
    else{ // 1 pipe in the job
        if (pipe(pipefd) != 0){
            printf("Messed up pipe\n");
            exit(1);
        }
        pid_t cpid = fork();
        if(cpid == 0){
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
            dup2(pipefd[1], 1);
            close(pipefd[0]);
            
        }
        else{
            pid_t cpid2 = fork();
            if(cpid2 == 0){

            }
            else{

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
    fgets(buf, 2000, stdin);
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
    while(1){
        prompt();
    }
}
