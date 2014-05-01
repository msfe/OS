#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int main(int argc, char *argv[], char **envp)
{
  int MAX_STRING_LEN = 70; //given in the assigment
  char input[MAX_STRING_LEN];
  int status;
  pid_t cpid;

  char EXIT_STRING[] = "exit";

  printf("Welcome to miniTerm 0.1. Please state your commands\n");


  while(1){
    printf(">");
    scanf("%s", input);
    if (strcmp(input,EXIT_STRING) == 0){
      exit(EXIT_SUCCESS);
    }

    cpid = fork(); /* Create a pipe */

    /* If processID is invalid value */
    if (cpid == -1) 
    { 
      perror("fork");
      exit(EXIT_FAILURE);
    }

    /* Child */
    if (cpid == 0) 
    {    
    char *args[] = {input, NULL}; /* execute the argument */
      execvp(args[0], args);
      exit(EXIT_SUCCESS);
    } 
    
    /* Parent */
    else 
    { 
      while(1){
        waitpid(cpid, &status, 0);
        if(status == 0){
          break; //Terminated in a correct way
        } else {
        //Duno
        // exit(EXIT_FAILURE);
        //Do nothing
        }
      }
      // printf("%s\n","I'm the Parent!");
    }
  }
}