#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

typedef int bool;
#define true 1
#define false 0

bool interpretLine(char* buffer){

    if(strncmp(buffer, "exit", strlen(buffer)-1) == 0){

        return false;

    } else {

        printf("I don't know this command, see -help for info\n");
    }

    return true;

}

/*simple function to read the lines written in the prompt !!!Don't need this since I use readline(), keep this just in case*/
/*char* getLine(){

    char *buffer = NULL;
    ssize_t buffSize = 0;

    getline(&buffer, &buffSize, stdin);

    return buffer;

    free(buffer);

}*/

/*main loop*/
void command_loop(){

    bool status = true;
    char* line;

    while(status){

        /*implement arrow keys*/

        line = readline(">_");

        if(strlen(line) != 1){ //empty string

        add_history(line);
        status = interpretLine(line);

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


