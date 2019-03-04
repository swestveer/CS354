////////////////////////////////////////////////////////////////////////////////
// Main File:        verify_hetero.c
// This File:        verify_hetero.c
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
// Online sources:   stackoverflow.com, reddit.com/r/C_Programming/, Chegg.com
//                   cprogramming.com, piazza.com
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers

typedef struct _Square {
    int size;
    int **array;
} Square;

/* construct_square reads the input file to initialize a square struct
 * from the contents of the file and returns the square.
 * The format of the file is defined in the assignment specifications
 */
Square * construct_square(char *filename)
{
  Square* sq = (Square*)malloc(sizeof(Square)); //allocate space for structure dynamically
  FILE* fp = fopen(filename, "r"); //open file
  char line[2024]; 
  int size; //size of array
  fscanf(fp, "%d\n", &size); //read first int for square size
  sq->size = size; //assign this to struct
  int *arr[size]; //set size of array based on size
  for(int i = 0; i < size; i++) {
    arr[i] = malloc(size*sizeof(int));
  }
  for(int i = 0; i < size; i++) {  
    int j = 0;
    fgets(line, 2024, fp); //read one line at a time
    char* tok = strtok(line, ","); //split using comma
    while(tok != NULL) { //keep splitting till end of line
      int temp;
      sscanf(tok, "%d", &temp); //convert input to int
      arr[i][j++] = temp; //put in array
      tok = strtok(NULL, ","); //find next one
    }
  }
  sq->array = arr; //assign this array to struct and return
  fclose(fp);
  return sq;

}



/* verify_hetero verifies if the square is a heterosquare
 * 
 * returns 1(true) or 0(false)
 */
 
 void insertion_sort(int* arry[], int size) {
  for(int j = 0; j < ((2*size) + 1); j++) {
    
    for (int i = (j + 1); i > 0; i--) {
      if ( *arry[i] < *arry[(i-1)] ){
        int temp2 = *arry[(i-1)];
        *arry[(i-1)] = *arry[i]; 
        *arry[i] = temp2;
      } 
    }
  } 
}
 
int verify_hetero(Square * nq)               
{
  int *p = malloc(sizeof(int));
  //for(int i = 0; i < nq->size; i++) { //print value of array
    //for(int j = 0; j < nq->size; j++) {
			//printf("%d ", *((*(nq -> array + i) + j)));
    //}
    //printf("\n");
  //}
  int c = 0;
  int size; //size of array;
  size = nq->size; //set size to size of square
  int *sums[(2*size) + 2]; //allocate space for array dynamically
  for(int i = 0; i < ((2*size) + 2); i++) {
   sums[i] = (int*)malloc(size*sizeof(int));
  }
  for(int i = 0; i < size; i++) { //Finds sums for Columns and Rows
    int sum = 0;
    for(int j = 0; j < size; j++) { //For loop for columns
  
      p = ((*(nq -> array + i) + j));
      //printf("Value:  %d\n", *p);
  	  sum = sum +*p;
    } 
	  //printf("Sum:  %d\n", sum);
    *sums[c] = sum; 
    c++;
    
    int sum2 = 0;
    for(int j = 0; j < size; j++) { //For loop for rows
  
      p = ((*(nq -> array + j) + i));
      //printf("Value:  %d\n", *p);
  	  sum2 = sum2 +*p;
    } 
	  //printf("Sum2:  %d\n", sum2);
    *sums[c] = sum2; 
    c++;  
  }
  
  int sum = 0;
  for(int i = 0; i < size; i++) { //Left to right diagonal
    p = ((*(nq -> array + i) + i));
    //printf("Value:  %d\n", *p);
 	  sum = sum +*p; 
  }
  //printf("Sum:  %d\n", sum);
  *sums[c] = sum; 
  c++;
  
  int sum2 = 0;
  int j = 0;
  for(int i = (size - 1); i >= 0; i--) { // Right to left diagonal
    p = ((*(nq -> array + j) + i));
    //printf("Value:  %d\n", *p);
 	  sum2 = sum2 +*p; 
    j++;
  }
  //printf("Sum2:  %d\n", sum2);
  *sums[c] = sum2; 
  
  //for(int i = 0; i < ((2*size) + 2); i++) {
    //printf("ArraySum:  %d\n" ,(*sums[i]));
  //}
  //printf("\n");
  free(p);
  
  insertion_sort(sums, size);

  //for(int i = 0; i < ((2*size) + 2); i++) {
    //printf("ArraySum:  %d\n" ,(*sums[i]));
  //}
  
  for (int i = 0; i < ((2*size) + 1); i++) {
  
    if(*sums[i] == *sums[(i+1)]) {
      printf("%s\n", "false");
      return 0;
    }
  
  }
  printf("%s\n", "true");
  return 0;
}

int main(int argc, char* argv[]) {

  
  if (argc != 2) { //check for correct amount of arguments
    printf("%s\n", "Incorrect amount of arguments");
    return 1;
  }
    FILE* fp = fopen(argv[1], "r"); //check for existing argument
    if (fp == NULL) {
    printf("%s\n", "Cannot open file for reading.");
    return 1;
  }

  Square* nq = construct_square(argv[1]); //call function
  verify_hetero(nq);  


}

