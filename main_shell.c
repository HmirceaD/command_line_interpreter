#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>

#include <readline/readline.h>
#include <readline/history.h>

typedef int bool;
#define true 1
#define false 0

extern int alphasort();//from direct.h

//all the supported commands
char* univ_commands[4] = {"exit", "-help", "cd", "ls", NULL};

//exit - 0
//-help - 1
//cd - 2
//ls -3


/**help function**/
void printHelp(){

    //you know what this does
    printf("\n\nType:\n\n#exit to exit the terminal\n#ls (-l, -r) to get a list of all\n\n");

}


/**Actuall implementation of cd**/
int cd_func(char **arg){

    if(arg[1] == NULL){

        return chdir(getenv("HOME"));

    } else if((strcmp(arg[1], "~") == 0) ||  (strcmp(arg[1], " ") == 0) || (strcmp(arg[1], "") == 0)){

        return chdir(getenv("HOME"));

    } else {

        return chdir(arg[1]);
    }

}

/*Helper function for the ls*/

/*static int file_select(const struct dirent *temp){


    if(strcmp(temp->d_name, ".") == 0 || strcmp(temp->d_name, "..") == 0){

        return false;

    } else {

        return true;
    }

}*/

/**Implementation of Ls**/
int ls_func(char **arg){

    char *crr_dir;
    int count;
    struct dirent **files;

    char* d = getcwd(crr_dir, sizeof(crr_dir));

    count = scandir(".", &files, NULL, alphasort);

    if(count <= 0){

        printf("\nNo files in the directory");
        return 1;
    }

    printf("\nFiles:\n\n");
    for(int i = 2; i < count; i++){//starts from 2 so it can skip over "." and ".."

        printf("%s ", files[i]->d_name);
    }

    printf("\n");

    return 1;



}

bool interpretLine(char* buffer, char** tokens){

    //flag for function return
    int result = 0;

    //condition on if the loop will keep running
    int ok = true;

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

            /*cd*/
            /*correct syntax*/
            ok = 2;
            write(teava[1], &ok, sizeof(int));
            close(teava[1]);

        }else if(strcmp(tokens[0], "ls") == 0){
            /*ls function implementation*/

            result = ls_func(tokens);

            ok = true;
            write(teava[1], &ok, sizeof(int));
            close(teava[1]);

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

        } else if(conditie == 2){

            cd_func(tokens);
            return true;

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

    char* buf;
    long size = pathconf(".", _PC_PATH_MAX);

    bool status = true;
    char* line;
    char* arguments[1024];

    while(status){

        printf("\n%s", getcwd(buf, (size_t)size));
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


