////////////////////////////////////////////////////////////////////////////////
// This File:        cache1D.c
// Other Files:      none
// Semester:         CS 354 Spring 2018
//
// Author:           Shawn Westveer
// Email:            westveer@wisc.edu
// CS Login:         westveer
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//
// Persons:          None
//
// Online sources:  https://www.tutorialspoint.com/cprogramming/c_operators.htm
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int arr[100000];

int main(int argc, char* argv[]) {
  
  for(int i = 0; i < 100000; i++) {
    arr[i] = i;
  }
  return 0;
}