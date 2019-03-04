////////////////////////////////////////////////////////////////////////////////
// Main File:        divison.c
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

int count = 0;

// Declare handler for SIGFPE
void handler_SIGFPE() {
  printf("Error: a division by 0 operation was attempted.\n");
  printf("Total number of operations completed successfully: %d\n", count);
  printf("The program will be terminated.\n");
  exit(1);
}

// Declare handler for SIGINT
void handler_SIGINT(){
  printf("\nTotal number of operations completed successfully: %d\n", count);
  printf("The program will be terminated.\n");
  exit(1); 
}

int main() {
  // Sigaction() for SIGFPE
  struct sigaction sa;
  sa.sa_handler = handler_SIGFPE;
  
  if (sigaction(SIGFPE, &sa, NULL) != 0){
    printf("Error with sigaction() for SIGFPE. \n");
  }
  
  // Sigaction() for SIGINT
  struct sigaction sa2;
  sa2.sa_handler = handler_SIGINT;
  sa2.sa_flags = 0;

  if (sigaction(SIGINT, &sa2, NULL) != 0) {
    printf("Error with sigaction() for SIGINT. \n");
  }
  
  while (1) {
    int num1, num2;
    char input[100];
    
    printf("Enter first integer: ");
    if (fgets(input, 100, stdin) != NULL) {    
      num1 = atoi(input);          
    }
    
    printf("Enter second integer: ");
    if (fgets(input, 100, stdin) != NULL) {
      num2 = atoi(input);
    }
        
    printf("%d / %d is %d with a remainder of %d\n", 
    num1, num2, num1/num2, num1%num2);
      count++;
    }
    
    return 0;
}
