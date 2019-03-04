////////////////////////////////////////////////////////////////////////////////
// This File:        cache2Drows.c
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

int arr2D[3000][500];

int main(int argc, char* argv[]) {
  
  for(int row = 0; row < 3000; row++) {
    for(int col = 0; col < 500; col++) {
      arr2D[row][col] = row + col;
    }
  }
  return 0;
}