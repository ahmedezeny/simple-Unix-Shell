#include <iostream>
#include "stdio.h"
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#define MaxWord 10
#define MaxChar 100

using namespace std;

//writes in a log file and records time of the log.
void WriteLogFile(const char* szString);

//handler to the SIGCHLD signel that logs the instance of child termination
void handler(int sig);

//reads the input line and sences the exit statement
void readLine(char line[]);

//fills the arguments array
void processLine(char line[],char* args[]);

//calls the previous 2 functions
int readAndProcessLine(char line[],char* args[]);

//handles the non system call cd
int cd(char *path);


int main()
{
    char line[MaxChar];
    char* args[MaxWord];
    bool parentp=false;

    while(1){
        //first step
        readAndProcessLine(line,args);

        //detecting the & char to eaither do or skip the wait for child
        for(int i=0;i<100&&args[i]!=NULL;i++){
            if(args[i]!=NULL&&strcmp(args[i], "&") == 0){
            parentp=true;
            args[i]="\0";
            }
        }

        //handeling the cd command seperately
        if (line[0]!='\0'&&strcmp(args[0], "cd") == 0&&strcmp(args[1], "/") != 0) {
            if (cd(args[1]) < 0/*executing the command*/) {
                //handeling faild exeution
                perror("faild to change dir");
                WriteLogFile("faild to change dir");
            }
            continue;
        }

        //forking a child process
        pid_t childPid=fork();


        if (childPid < 0) {
            WriteLogFile("Fork failed");
            perror("Fork failed");
            exit(1);
        }else if(childPid==0){
            WriteLogFile("child is running");
            //cout<<"ch"<<endl;
            execvp(args[0],args);
            WriteLogFile("running child process was not successful");
            exit(1);
        }else{
            if(parentp){
                parentp=false;
                continue;
            }
            waitpid(childPid,0,0);
            WriteLogFile("child exited");
            //cout<<"p"<<endl;
        }
        signal(SIGCHLD, handler);
    }
    return 0;
}

void readLine(char line[]){
    fgets(line,MaxChar,stdin);
    //removing \n
    int i=0;
    while(line[i]!='\n'){
        i++;
    }
    line[i]='\0';

    //handeling exit command
    if(strcmp(line,"exit")==0){
        exit(0);
    }

}

void processLine(char line[],char* args[]){
    int i=0;
    args[i] = strtok(line," ");

    //filling args[] chopping by space charecter
    while (args[i]){
        i++;
        args[i] = strtok(NULL," ");
    }

}

int readAndProcessLine(char line[],char* args[]){

    cout<< "Zeny@pop>> ";

    //reading command from the user
    readLine(line);
    if(line[0]=='\0'){
        cout<<"no command"<<endl;
    }

    //filling the arguments array
    processLine(line,args);

    return 1;
}

int cd(char *path) {
    return chdir(path);
}

void handler(int sig) {
    pid_t pid;
    pid = wait(nullptr);
    if(pid>0) {
        WriteLogFile("child is terminated");;
    }
}

void WriteLogFile(const char* szString)
{
    time_t t=time(0);
    FILE* pFile = fopen("logFile.txt", "a");
    fprintf(pFile, "%s %s\n",szString,ctime(&t));
    fclose(pFile);

}
