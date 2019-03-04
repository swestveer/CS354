#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int pid = 0;

int main(int argc, char *argv[]) {

  if (argc != 3) { //check for correct amount of arguments
    printf("%s\n", "Usage: <signal type> <pid>");
    return 1;
  }
  char sig = atoi(argv[1]);
  sscanf (argv[2],"%d",&pid);

  if(*sig == 'i'){
    kill(pid, SIGINT);
  }
  
  if(*sig == 'u'){
    kill(pid, SIGUSR1);
  }
  printf("%s\n", sig);
  printf("%d\n", pid);
  

}