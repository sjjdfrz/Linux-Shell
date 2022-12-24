#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>


#define clear() printf("\033[H\033[J")

void init() {
    clear();
    printf("\n\n\n\n******************"
           "************************");
    printf("\n\n\n\t****Welcome To My Linux Shell****");
    sleep(5);
    clear();
}


int takeInput(char *str) {
    char *input;

    input = readline(" ~$ ");
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
    printf("\n@%s:%s", username, cwd);
}

// function for system commands
void execArgs(char **parsed) {

    pid_t pid = fork();

    if (pid == -1) {
        fprintf(stderr, "\nFailed!");
        return;
    } else if (pid == 0) {

        if (execvp(parsed[0], parsed) < 0) {
            fprintf(stderr, "\nCould not execute command..");
        }
        exit(0);
    } else {
        wait(NULL);
        return;
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

    int i, switchCommand = 0;
    char *command_list[3];

    command_list[0] = "exit";
    command_list[1] = "cd";
    command_list[2] = "help";

    for (i = 0; i < 3; i++) {
        if (strcmp(parsed[0], command_list[i]) == 0) {
            switchCommand = i + 1;
            break;
        }
    }

    switch (switchCommand) {
        case 1:
            printf("\nGoodbye\n");
            exit(0);
        case 2:
            chdir(parsed[1]);
            return 1;
        case 3:
            openHelp();
            return 1;

        default:
            break;
    }

    return 0;
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


int processString(char *str, char **parsed) {


    parseSpace(str, parsed);

    if (commandHandler(parsed))
        return 0; // own commands
    else
        return 1; // system or pipe command
}


int main() {
    char inputString[1000], *parsed[100];
    int execFlag;
    init();

    while (1) {

        printDir();
        if (takeInput(inputString))
            continue; // there is no command

        execFlag = processString(inputString, parsed);

        // system command
        if (execFlag == 1)
            execArgs(parsed);

    }
}