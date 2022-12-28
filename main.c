#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>


#define clear() printf("\033[H\033[J")

void init() {
    printf("\n\n\t****Welcome To My Linux Shell****\n\n");
    sleep(2);
    clear();
}

void signalHandler(int sig_num)
{

    signal(SIGINT, signalHandler);
    printf("\n");
    fflush(stdout);
}

void history(char *buf) {

    FILE *fptr;

    fptr = fopen("history.txt", "a");

    if (fptr == NULL) {
        printf("Error!");
        exit(1);
    }

    fprintf(fptr, "%s\n", buf);
    fclose(fptr);
}


void clear_file() {

    FILE *fptr;

    fptr = fopen("history.txt", "w");

    if (fptr == NULL) {
        printf("Error!");
        exit(1);
    }

    fclose(fptr);
}


int takeInput(char *str) {
    char *input;

    input = readline("\n~$ ");
    if (strlen(input) != 0) {
        history(input);
        add_history(input);
        strcpy(str, input);
        return 0;
    } else {
        return 1;
    }
}

// print Current Directory.
void printDir() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char *username = getenv("USER");
    printf("\n@%s:~%s", username, cwd);
}

// function for system commands
void execArgs(char **parsed) {

    pid_t pid = fork();

    if (pid == -1) {
        fprintf(stderr, "\nfailed fork!");
        return;
    } else if (pid == 0) {

        if (execvp(parsed[0], parsed) < 0) {
            fprintf(stderr, "\nfailed execute command..");
        }
        exit(0);
    } else {
        wait(NULL);
        return;
    }
}

// Function for piped system commands
void execArgsPiped(char **parsed, char **parsedpipe) {
    // 0-read, 1-write
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        fprintf(stderr, "\nfailed pipe");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        fprintf(stderr, "\nfailed fork");
        return;
    }

    if (p1 == 0) {
        // p1 (child1) --> write
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed[0], parsed) < 0) {
            fprintf(stderr, "\nnfailed execute command 1..");
            exit(0);
        }
    } else {
        p2 = fork();

        if (p2 < 0) {
            fprintf(stderr, "\nnfailed fork");
            return;
        }

        // p2 (child2) --> read
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                fprintf(stderr, "\nnfailed execute command 2..");
                exit(0);
            }
        } else {
            wait(NULL);
            wait(NULL);
        }
    }
}


void openHelp() {
    puts("\nList of Commands supported:"
         "\n>cd"
         "\n>exit"
         "\n>firstword"
         "\n>maxword"
         "\n>delwspace"
         "\n>notcomment"
         "\n>nolines"
         "\n>ften"
         "\n>other linux commands"
    );

    return;
}


int commandHandler(char **parsed) {

    int i, switchCommand;
    char *command_list[9];

    command_list[0] = "exit";
    command_list[1] = "cd";
    command_list[2] = "help";
    command_list[3] = "firstword";
    command_list[4] = "maxword";
    command_list[5] = "delwspace";
    command_list[6] = "notcomment";
    command_list[7] = "nolines";
    command_list[8] = "ften";

    for (i = 0; i < 9; i++) {
        if (strcmp(parsed[0], command_list[i]) == 0) {
            switchCommand = i;
            break;
        }
    }

    switch (switchCommand) {
        case 0:
            clear_file();
            exit(0);

        case 1:
            chdir(parsed[1]);
            return 1;

        case 2:
            openHelp();
            return 1;

        case 3:
            parsed[2] = parsed[1];
            parsed[0] = "awk";
            parsed[1] = "{print $1}";
            parsed[3] = NULL;
            return 0;

        case 4:
            parsed[2] = parsed[1];
            parsed[0] = "awk";
            parsed[1] = "{ if (++words[$0] > max) { max = words[$0]; maxW=$0 } } END { print maxW }";
            parsed[3] = NULL;
            return 0;

        case 5:
            parsed[3] = parsed[1];
            parsed[0] = "perl";
            parsed[1] = "-pe";
            parsed[2] = "s/\\s+//g";
            parsed[4] = NULL;
            return 0;

        case 6:
            parsed[2] = parsed[1];
            parsed[0] = "grep";
            parsed[1] = "^[^#]";
            parsed[3] = NULL;
            return 0;

        case 7:
            parsed[3] = parsed[1];
            parsed[0] = "sed";
            parsed[1] = "-n";
            parsed[2] = "$=";
            parsed[4] = NULL;
            return 0;

        case 8:
            parsed[0] = "head";
            parsed[2] = NULL;
            return 0;

        default:
            break;
    }

    return 0;
}


int parsePipe(char *str, char **strpiped) {
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }

    if (strpiped[1] == NULL)
        return 0; // there is no pipe
    else {
        return 1;
    }
}


void parseSpace(char *str, char **parsed) {
    int i, j;

    for (i = 0; i < 100; i++) {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}


int processString(char *str, char **parsed, char **parsedpiped) {

    char *strpiped[2];
    int piped;

    piped = parsePipe(str, strpiped);

    if (piped) {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpiped);
    } else
        parseSpace(str, parsed);

    if (commandHandler(parsed))
        return 0; // own commands
    else
        return 1 + piped; // system or pipe command
}

int main() {
    char inputString[1000], *parsed[100], *parsedPiped[100];
    int execFlag;
    init();

    signal(SIGINT, signalHandler);

    while (1) {

        printDir();
        if (takeInput(inputString))
            continue; // there is no command

        execFlag = processString(inputString, parsed, parsedPiped);

        // system command
        if (execFlag == 1)
            execArgs(parsed);
        // pipe command
        if (execFlag == 2)
            execArgsPiped(parsed, parsedPiped);
    }
}