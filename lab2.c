#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void printenvp(int, char**, char**);

/* A method for handling enviromentvariables, uses pipes and linux-commands
to make it easier to study envirometvariables */
int main(int argc, char *argv[], char **envp)
{
  int pipePrintGrep[2];
  pid_t cpid;
  char *pager = "less";
  int status;

  /* If we have an unwanted number of arguments */
  if (argc < 1 || argc > 2) {
    fprintf(stderr, "Usage: %s <string>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int i;
  for(i=0; envp[i] != NULL; i++) {
    if(strstr(envp[i], "PAGER") != NULL) {
      pager = envp[i]+6;
      break;
    }
  }

  /* If the creation of a pipe failed */
  if (pipe(pipePrintGrep) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
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
    close(pipePrintGrep[0]); /* Close unused read end */

    /*Write to file instead of STD_OUT */
    dup2(pipePrintGrep[1], STDOUT_FILENO);
    close(pipePrintGrep[1]);

    printenvp(argc, argv, envp); /* Writes environment variables */
    /*_exit(EXIT_SUCCESS); */
  } 
  /* Parent */
  else 
  { 
    int pipeGrepSort[2];

    /* If the creation of a pipe failed */
    if (pipe(pipeGrepSort) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
    
    /* Wait for child processes to finish */
    waitpid(cpid, &status, 0);
    if(WEXITSTATUS(status) != 0) {
      exit(EXIT_FAILURE);
    }
    cpid = fork(); /* Create a pipe */

    if (cpid == -1) { 
      perror("fork");
      exit(EXIT_FAILURE);
    }

    /* Child grep */
    if (cpid == 0) 
    { 
      char *args[] = { "grep", ".*", NULL };

      close(pipePrintGrep[1]); /* Close unused write end */
      close(pipeGrepSort[0]);

      dup2(pipePrintGrep[0], STDIN_FILENO);
      close(pipePrintGrep[0]);

      dup2(pipeGrepSort[1], STDOUT_FILENO);
      close(pipeGrepSort[1]);

      /* Excecute the "sort"-argument, this causes the program to 
      "jump" from this file to another file*/
      if(argc != 1) {   
        execvp(args[0], argv);
      }
      else {
        execvp(args[0], args);
      } 

      /*If we reach this line, it means that the exec-command failed */
      perror("exec failed");
      _exit(EXIT_FAILURE);

    } 
    /* Parent */
    else 
    { 
      int pipeSortLess[2];

    /* If the creation of a pipe failed */
      if (pipe(pipeSortLess) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
      }

      int oldCpid = cpid;
      cpid = fork(); /* Create a pipe */

      if (cpid == -1) { 
        perror("fork");
        exit(EXIT_FAILURE);
      }

    /* Child sort */
      if (cpid == 0) 
      {    
      char *args[] = { "sort", NULL}; /* The sort-argument */
        close(pipePrintGrep[1]); /* Close unused write end */
        close(pipePrintGrep[0]); /* Close unused read end */

        close(pipeGrepSort[1]); /* Close unused write end */
        close(pipeSortLess[0]);

        dup2(pipeGrepSort[0], STDIN_FILENO);
        close(pipeGrepSort[0]);

        dup2(pipeSortLess[1], STDOUT_FILENO);
        close(pipeSortLess[1]);

        execvp(args[0], args); 

      /*If we reach this line, it means that the exec-command failed */
        perror("exec failed");
        _exit(EXIT_FAILURE);

      } 
    /* Parent less */
      else 
      { 
      close(pipeGrepSort[1]); /* Close unused write end */
      close(pipeGrepSort[0]); /* Close unused read end */

      close(pipePrintGrep[1]); /* Close unused write end */
      close(pipePrintGrep[0]); /* Close unused read end */

      close(pipeSortLess[1]); /* Close unused write end */

      char *args[] = { pager, NULL}; /* The less-argument */

      /* Reads from file instead of STD_IN*/
        dup2(pipeSortLess[0], STDIN_FILENO);
        close(pipeSortLess[0]);

      /* Excecute the "less"-argument, this causes the program to 
      "jump" from this file to another file*/

        /* Wait for child processes to finish */
        waitpid(oldCpid, &status, 0);
        if(WEXITSTATUS(status) != 0) {
          if(WEXITSTATUS(status) == 1) /* If grep couldn't find anything */
            fprintf(stderr, "Couldn't find any matches\n");
          exit(EXIT_FAILURE);
        }

        execvp(args[0], args);

        char *args2[] = { "more", NULL};
        execvp(args2[0], args2); 

      /*If we reach this line, it means that the exec-command failed */
        perror("exec failed");
        exit(EXIT_FAILURE);
      }
    }
  }
  exit(EXIT_SUCCESS);
}

/* A method for printing the enviroment variables given as argument */
void printenvp(int arg0, char *argv[], char **envp) {
  int i;
  for(i=0; envp[i] != NULL; i++) {
    printf("%2d:%s\n",i, envp[i]);
  }
}

