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

        case 4:
            parsed[2] = parsed[1];
            parsed[0] = "awk";
            parsed[1] = "{ if (++words[$0] > max) { max = words[$0]; maxW=$0 } } END { print maxW }";
            parsed[3] = NULL;
            return 0;

        case 5:
            parsed[2] = parsed[1];
            parsed[0] = "awk";
            parsed[1] = "{print $1}";
            parsed[3] = NULL;
            return 0;


        case 6:
            parsed[3] = parsed[1];
            parsed[0] = "perl";
            parsed[1] = "-pe";
            parsed[2] = "s/\\s+//g";
            parsed[4] = NULL;
            return 0;

        case 7:
            parsed[2] = parsed[1];
            parsed[0] = "grep";
            parsed[1] = "^[^#]";
            parsed[3] = NULL;
            return 0;

        case 8:
            parsed[3] = parsed[1];
            parsed[0] = "sed";
            parsed[1] = "-n";
            parsed[2] = "$=";
            parsed[4] = NULL;
            return 0;

        case 9:
            parsed[0] = "head";
            parsed[2] = NULL;
            return 0;

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