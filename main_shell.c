#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <readline/readline.h>
#include <readline/history.h>

typedef int bool;
#define true 1
#define false 0

//all the supported commands
char* univ_commands[3] = {"exit", "-help", NULL};

void printHelp(){

    printf("\n\nType:\n\n#exit to exit the terminal\n#ls (-l, -r) to get a list of all\n\n");

}

bool interpretLine(char* buffer, char** tokens){

    bool ok = true;
    int teava[2];

    if(pipe(teava) < 0){

        perror("Pipe died");
    }

    pid_t pid;

    pid = fork();

    if(pid < 0){

        perror("Error creating the process");
    }

    if(pid == 0){
        /*Child*/
        close(teava[0]);

        if(strcmp(buffer, univ_commands[0]) == 0){

            ok = false;
            write(teava[1], &ok, sizeof(int));
            close(teava[1]);

        }else if(strcmp(buffer, univ_commands[1]) == 0){

            printHelp();

        }else if(execvp(tokens[0], tokens) != -1){

            ok = true;
            write(teava[1], &ok, sizeof(int));
            close(teava[1]);

        } else {

            printf("I don't know this command, see -help for info\n");
        }

    }

    if(pid > 0){
        /*Parent*/

        wait(&pid);
        bool conditie;
        close(teava[1]);

        read(teava[0], &conditie, sizeof(int));

        if(conditie == false){

            return false;

        }else {

            return true;
        }

    }

}

void parse(char* line, char** arg){

    while(*line != '\0'){

        while(*line == ' ' || *line == '\t' || *line == '\n'){

            *line++ = '\0';
        }

        *arg++ = line;

        while(*line != '\0' && *line != '\n' && *line != '\t' && *line != ' '){

            line++;
        }

    }

    *arg = '\0';

}

/*main loop*/
void command_loop(){

    bool status = true;
    char* line;
    char* arguments[1024];

    while(status){

        line = readline(">>");

        if(strlen(line) != 0){ //empty string

        add_history(line);
        parse(line, arguments);
        status = interpretLine(line, arguments);

        }

    }

    free(line);

}

/*main*/
int main(int argc, char **argv){

    printf("\n=========== SHELL ===========\n\n");

    command_loop();

    return 0;

}


