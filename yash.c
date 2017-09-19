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

int pipefd[2];
int pid_ch1, pid_ch2, pid;

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
    char *pwd;			/* current working dir of shell when *
				 * this job was spawned              */
    struct process *procs;	/* list of processes                 */
    struct process *auxprocs;	/* auxiliary processes e.g multios   */
    LinkList filelist;		/* list of files to delete when done */
				/* elements are struct jobfile */
    int stty_in_env;		/* if STTY=... is present            */
    struct ttyinfo *ty;		/* the modes specified by STTY       */
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

void eval_cmds(int len, int maxlen, char* arr[len][maxlen]){
    int i;
    for(i = 0; i < len; i++){
        
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
    int status;
    while(1){
        prompt();
    }
}
