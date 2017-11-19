#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

//#include <readline/readline.h>
//#include <readline/history.h>


typedef int bool;
#define true 1
#define false 0


bool interpretLine(char* buffer){

    if(strncmp(buffer, "exit", strlen(buffer)-1) == 0){

        return false;

    } else {

        //add_history(buffer);
        printf("I don't know this command, see -help for info\n");
    }

    return true;

}

/*simple function to read the lines written in the prompt*/
char* getLine(){

    char *buffer = NULL;
    ssize_t buffSize = 0;

    getline(&buffer, &buffSize, stdin);

    return buffer;

    free(buffer);

}

/*main loop*/
void command_loop(){

    bool status = true;
    char* line;
    char** history;

    history = (char**)malloc(sizeof(char*)*50);
    int index = 0;

    while(status){

    printf(">_ ");

    line = getLine();

    status = interpretLine(line);

        if(status){

            history[index] = (char*)malloc(sizeof(char*));

            history[index] = line;

            index++;
        }


    }

    /*HISTORY WORKS*/

    free(line);

}

/*main*/
int main(int argc, char **argv){

    printf("\n=========== SHELL ===========\n\n");


    command_loop();

    return 0;

}


