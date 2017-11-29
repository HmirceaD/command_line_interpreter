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
char* univ_commands[3] = {"exit", "-help", "cd", NULL};

/**help function**/
void printHelp(){

    //you know what this does
    printf("\n\nType:\n\n#exit to exit the terminal\n#ls (-l, -r) to get a list of all\n\n");

}

/**'cd'**/
int change_dir(char **arg){

    if(arg[3] != NULL || arg[1] == NULL){

        printf("\nNot the correct format\n");
        return 1;
    }

}

bool interpretLine(char* buffer, char** tokens){

    //flag for function return
    int result = 0;

    //condition on if the loop will keep running
    bool ok = true;

    //pipe
    int teava[2];

    //pipe died
    if(pipe(teava) < 0){

        perror("Pipe died");
        return true;
    }

    pid_t pid;

    pid = fork();

    //process died
    if(pid < 0){

        perror("Error creating the process");
        return false;
    }

    if(pid == 0){
        /*Child*/
        close(teava[0]);

        if(strcmp(buffer, univ_commands[0]) == 0){

            /*exit function*/
            ok = false;
            write(teava[1], &ok, sizeof(int));
            close(teava[1]);

        }else if(strcmp(buffer, univ_commands[1]) == 0){

            /*-help function*/
            printHelp();

        }else if(strcmp(tokens[0], univ_commands[2]) == 0){

            result = change_dir(tokens);


        }else if(execvp(tokens[0], tokens) != -1){

            /*any linux command*/
            ok = true;
            write(teava[1], &ok, sizeof(int));
            close(teava[1]);

        } else {

            /*random garbage*/
            printf("I don't know this command, see -help for info\n");
        }

    }

    if(pid > 0){
        /*Parent*/

        //wait for the child to end
        wait(&pid);

        //check if the status needs to be changed and then return it
        int conditie;
        close(teava[1]);

        read(teava[0], &conditie, sizeof(int));

        if(conditie == true || conditie == false){

            return conditie;

        } else {

        }



    }

}

/*Parse the line and break it into arguments*/
void parse(char* line, char** arg){

    while(*line != '\0'){//end of line

        while(*line == ' ' || *line == '\t' || *line == '\n'){
            //replace empty spaces with 0
            *line++ = '\0';
        }

        //save it in the aarg **
        *arg++ = line;

        while(*line != '\0' && *line != '\n' && *line != '\t' && *line != ' '){
            //skip untill the next space
            line++;
        }

    }

    //add terminator
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

    printf("\n=========== SHELL ===========\n\nSee'-help' for instructions how to use the commands\n\n");

    command_loop();

    return 0;

}


