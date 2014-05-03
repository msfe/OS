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

  int status;
  pid_t cpid;

  char EXIT_STRING[] = "exit";
  char CD_STRING[] = "cd";
  int DEBUG = 0;

  printf("Welcome to miniTerm 0.1. Please state your commands\n");


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

    // realloc one extra element for the last NULL
    args = realloc (args, sizeof (char*) * (nbrOfSpaces+1));
    args[nbrOfSpaces] = 0;

    if(DEBUG){
      int i;
      for (i = 0; i < (nbrOfSpaces+1); ++i){
        printf ("args[%d] = %s\n", i, args[i]);
      }
    }

    //scanf("%s", input); //Tar bara till första mellanslaget

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
      execvp(args[0], args);
      exit(EXIT_FAILURE);
    } 

    /* Parent */
    else 
    { 
      while(1){
        waitpid(cpid, &status, 0);
        if(status == 0){
          free (args);
          printf("Program executed for %d seconds\n", (int) difftime(time(NULL),startTime));
          break; //Terminated in a correct way
        } else {
          if(WIFEXITED(status)){        
            break; //Terminated in a correct way
          }
        }
      }
      // printf("%s\n","I'm the Parent!");
    }
  }
}