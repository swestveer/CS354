////////////////////////////////////////////////////////////////////////////////
// Main File:        sendsig.c
// Semester:         CS 354 Spring 2018
//
// Author:           Shawn Westveer
// Email:            westveer@wisc.edu
// CS Login:         westveer
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          None.
//
// Online sources:   piazza.com, stackoverflow.com
//////////////////////////// 80 columns wide ///////////////////////////////////
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int result = 10;

int main(int argc, char *argv[]) {
  // Checks for correct # of args
  if (argc != 3) {
    printf("%s\n", "Usage: <signal type> <pid>");
    return 1;
  }
  const char *sig = (argv[1]);
  
  // Checks for correct input on signal type
  if ((result = strcmp(sig, "-i")) != 0 && (result = strcmp(sig, "-u")) != 0) {
    printf("%s\n", "Usage: <signal type> <pid>");
    return 1;
  }
  
  int pid = atoi(argv[2]);
  
  // Sends SIGINT to requested pid
  if ((result = strcmp(sig, "-i")) == 0){
    kill(pid, SIGINT);
    exit(1);
  }
  
  // Sends SIGUSR1 to requested pid
  if ((result = strcmp(sig, "-u")) == 0){
    kill(pid, SIGUSR1);
    exit(1);
  }
}
