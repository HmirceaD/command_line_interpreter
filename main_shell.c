#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

bool interpretLine(char* buffer){

    if(strcmp(buffer, "exit") == 0){

        return false;

    } else {
        printf("I don't know this command\n");
        return false;
        printf("%s", buffer);
    }

    return true;

}

/*simple function to read the lines written in the prompt*/
char* getLine(){

    char *buffer = NULL;
    ssize_t buffSize = 0;

    getline(&buffer, &buffSize, stdin);

    return buffer;

}

/*main loop*/
void command_loop(){

    bool status = true;
    char* line;


    while(status){

    printf(">_ ");

    line = getLine();

    status = interpretLine(line);

    }

}

/*main*/
int main(int argc, char **argv){

    printf("\n=========== SHELL ===========\n\n");


    command_loop();

    return 0;

}


