// A simple terminal developed by Mattias Folke and Martin Andersen
// Useful Links:
// http://www.ict.kth.se/courses/ID2206/COURSELIB/labs/smallShell.LabPM.pdf
// http://stackoverflow.com/questions/11198604/c-split-string-into-an-array-of-strings

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>

void signal_handler_parrent(int);

int main(int argc, char *argv[], char **envp)
{
  int MAX_STRING_LEN = 70; //given in the assigment
  char input[MAX_STRING_LEN];
  size_t MAX_PATH_LEN = 70; //Because its a good number
  char pathBuffer[MAX_PATH_LEN];
  char *path;
  char **args;
  char *p;
  int nbrOfSpaces;
  int moreDeadProcesses;

  int status;
  pid_t cpid;

  char EXIT_STRING[] = "exit";
  char CD_STRING[] = "cd";
  char AND_STRING[] = "&";
  int DEBUG = 0;

  printf("Welcome to miniTerm 0.1. Please state your commands\n");

  signal(SIGINT, signal_handler_parrent);


  while(1){
    path = getcwd(pathBuffer, MAX_PATH_LEN);
    printf("%s>",path);
    //Get input
    p = fgets (input, MAX_STRING_LEN, stdin);
    //Remove \n at the end
    if (p != NULL) {
      size_t last = strlen (input) - 1;
      if (input[last] == '\n') input[last] = '\0';
    }

    //Reset Paramaters
    p    = strtok (input, " ");
    args  = NULL;
    nbrOfSpaces = 0;

    //Split the strings into Array
    while(p != NULL){
      args = realloc (args, sizeof (char*) * ++nbrOfSpaces);

      if (args == NULL){
        exit (EXIT_FAILURE); /* memory allocation failed */
      }
      args[nbrOfSpaces-1] = p;

      p = strtok (NULL, " "); //Get next strtok
    }


    //Evaluate if a background process is done
    moreDeadProcesses = 1;
    while(moreDeadProcesses){
      moreDeadProcesses = 0;
      cpid = waitpid(-1, &status, WNOHANG);
      if(cpid>0){
        printf("Process %d executed\n",cpid);
        moreDeadProcesses = 1;
      }
    }

    //If nothing is entered
    if (nbrOfSpaces == 0){
      continue;
    }

    // realloc one extra element for the last NULL
    args = realloc (args, sizeof (char*) * (nbrOfSpaces+1));
    args[nbrOfSpaces] = 0;

    if(DEBUG){
      int i;
      for (i = 0; i < (nbrOfSpaces+1); ++i){
        printf ("args[%d] = %s\n", i, args[i]);
      }
    }


    if (strcmp((char *)args[0],EXIT_STRING) == 0){
      exit(EXIT_SUCCESS);
    }
    if (strcmp((char *)args[0],CD_STRING) == 0){
      if(nbrOfSpaces > 1){ // Checks that we have additional arguments
        chdir((char *)args[1]);
      } else {
        printf("cd is used: cd <path>\n");
      }
      continue;
    }

    time_t startTime = time(NULL);

    cpid = fork(); /* Create a new process */

    /* If processID is invalid value */
    if (cpid == -1) 
    { 
      perror("fork");
      exit(EXIT_FAILURE);
    }

    /* Child */
    if (cpid == 0) 
    { 
      //Handle differense between forground and background process 
      if(nbrOfSpaces > 0 && strcmp((char *)args[nbrOfSpaces-1],AND_STRING) == 0){ //Background process
        args[nbrOfSpaces-1] = NULL;
        printf("PID: %d\tRunning in: Background\n",getpid());
      } else {
        printf("PID: %d\tRunning in: Foreground\n",getpid());
      }
      execvp(args[0], args);
      exit(EXIT_FAILURE);
    } 

    /* Parent */
    else 
    { 
        if(nbrOfSpaces > 0 && strcmp((char *)args[nbrOfSpaces-1],AND_STRING) == 0){ //Background process
          //Do nothing
        } else {
      while(1){ //forground processes
        waitpid(cpid, &status, 0);
         if(WIFSIGNALED(status)){
           free (args);
           printf("Process %d executed for %d seconds\n",cpid, (int) difftime(time(NULL),startTime));
           break;
         }
        if(status == 0){
          free (args);
          printf("Process %d executed for %d seconds\n",cpid, (int) difftime(time(NULL),startTime));
          break; //Terminated in a correct way
        } else {
          if(WIFEXITED(status)){        
            break; //Terminated in a correct way
          }
        }
      }
    }
      // printf("%s\n","I'm the Parent!");
  }
}
}


void signal_handler_parrent(int signal) {
}
