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
  
  printf("Welcome to miniTerm 0.1. Please state your commands\n");
  printf(">");
  scanf("%s", input);

  char *args[] = {input, NULL}; /* The less-argument */
  execvp(args[0], args);
}