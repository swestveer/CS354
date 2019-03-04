////////////////////////////////////////////////////////////////////////////////
// Main File:        division.c
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

unsigned int alarm(unsigned int seconds);
unsigned int count = 0;

// Declare handler for SIGALRM
void handler_SIGALRM() {
  time_t currentTime;
  time(&currentTime);    
  printf("PID: %d", getpid());
  printf(" | Current Time: %s", ctime(&currentTime));   
  alarm(3);
}

// Declare handler for SIGINT
void handler_SIGINT(){
  printf("\nSIGINT received.\n");
  printf("SIGUSR1 was received ");
  printf("%d", count);
  printf(" times. Exiting now.\n");
  exit(1); 
}

// Declare hander for SIGUSR1
void handler_SIGUSR1(){
  count++;
  printf("SIGUSR1 caught!\n");    
}

int main(int argc, char *argv[]) {
  // Initialize struct to 0
  struct sigaction act;
  memset(&act, 0, sizeof(act)); 
  
  // Sigaction() for SIGALRM
  struct sigaction sa;
  sa.sa_handler = handler_SIGALRM;
  sa.sa_flags = 0;
  if (sigaction(SIGALRM, &sa, NULL) != 0) {
    printf("Error passing sigalrm handler \n");
    exit(1);         
  }

  // Sigaction() for SIGINT
  struct sigaction sa2;
  sa2.sa_handler = handler_SIGINT;
  sa2.sa_flags = 0;

  if (sigaction(SIGINT, &sa2, NULL) != 0) {
    printf("Error with sigaction() for SIGINT. \n");
  }

  // Sigaction() for SIGUSR1
  struct sigaction sa3;
  sa3.sa_handler = handler_SIGUSR1;
  sa3.sa_flags = 0;

  if (sigaction(SIGUSR1, &sa3, NULL) != 0) {
    printf("Error with sigaction() for SIGUSR1. \n");
  }

  // Call alarm function  
  alarm(3);
  printf("Pid and time will be printed every 3 seconds. \n"); 
  printf("Enter ^C to end the program. \n"); 

  while (1) {
  }    
  return 0; 
}
