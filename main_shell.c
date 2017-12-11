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
char* univ_commands[5] = {"exit", "-help", "cd", "ls", "tac", NULL};

//exit - 0
//-help - 1
//cd - 2
//ls -3
//tac -4


/**help function**/
void printHelp(){

    //you know what this does
    printf("\n\nType:\n\n#exit to exit the terminal\n#ls (-l, -r) to get a list of all\n\n");

}

/**helper function to get the number of args**/
int arg_num(char** arg){

    int i = 0;
    int num = 0;

    while(arg[i] != NULL){

       // if( *(*arg[i]) == '\0' || *(*arg[i]) == ' ' || *(*arg[0]) == '\n' || *(*arg[0]) == '\t'){
        if(arg[i][0] == '\0' || arg[i][0] == ' ' || arg[i][0] == '\n' || arg[i][0] == '\t' ){

            break;
        }
            num++;
            i++;
    }

    return num;
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

/**Implementation of Ls**/
int ls_func(int argc, char **arg){

    char *crr_dir;
    DIR *dp = NULL;
    int count;
    struct dirent *files;

    crr_dir = getenv("PWD");

    if(crr_dir == NULL){

        perror("Something went wrong :(");
        return -1;
    }

    dp = opendir((const char*) crr_dir);

    if(dp == NULL){

        perror("Something went wrong :(");
        return -1;
    }


    /*check for flags*/
     if(strcmp(arg[1], "-a") == 0){

        printf("\n");

        for(count = 0; (files = readdir(dp)) != NULL; count++){

            printf("%s ", files->d_name);

        }

        printf("\n");

        return 1;

    }

    printf("\n");

    for(count = 0; (files = readdir(dp)) != NULL; count++){

        if(files->d_name[0] != '.'){

            printf("%s ", files->d_name);
        }
    }

    printf("\n");

    return 1;

}

/*helper function to get the number of lines in a file*/
int get_lines(FILE* p){

    int num = 0;
    char c = fgetc(p);

    while(c != EOF){

        c = fgetc(p);

        if(c == '\n'){
            num++;
        }
    }

    return num;

}

/*Implementation of cat function*/
int tac_func(int argc, char **arg){


    FILE* fp = fopen(arg[1], "r");

    if(fp == NULL){

        perror("Could not open file");
        return -1;
    }

    int line_num = get_lines(fp);

    char lines[line_num][1024];
    rewind(fp);

    char* crrLine = NULL;
    size_t len = 0;
    ssize_t read;

    int index = 0;

    /*tac txt.txt*/
    if(strcmp(arg[1], "-b ") != 0 || strcmp(arg[1], "-s") != 0){
        //read line by line

        while((read = getline(&crrLine, &len, fp)) != -1){

            strcpy(lines[index], crrLine);
            index++;

        }



    /*tac -b txt.txt*/
    }else if(strcmp(arg[1], "-b ") == 0){

        //TODO
    }

    /* print the list backwards */

    if((argc == 4 && strcmp(arg[2], ">") == 0) || (argc == 5 && strcmp(arg[3], ">") == 0)){

        FILE* toFp;

        if(argc == 4){

            toFp = fopen(arg[3], "w");

            if(toFp == NULL){

                printf("Something went wrong when opening the file '%s'", arg[3]);
                return -1;

            }
        }else if(argc == 5){

            toFp = fopen(arg[4], "w");


            if(toFp == NULL){

                printf("Something went wrong when opening the file '%s'", arg[4]);
                return -1;

            }
        }


        int j = index - 1;

        int res;

        while(j >= 0){

            res = fputs(lines[j], toFp);

            if(res == EOF){

                perror("Somethig went wrong when writting to the second file mate");
                return -1;
            }

            j--;
        }

        fclose(toFp);


    }else{

        printf("\n");

        for(int i = index -1; i >= 0; i--){

            printf("%s", lines[i]);
        }
    }


    free(crrLine);
    fclose(fp);

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

            result = ls_func(arg_num(tokens), tokens);

            ok = true;
            write(teava[1], &ok, sizeof(int));
            close(teava[1]);

        } else if(strcmp(tokens[0], "tac") == 0){

            result = tac_func(arg_num(tokens), tokens);

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

        //save it in the arg **
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


