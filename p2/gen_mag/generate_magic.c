////////////////////////////////////////////////////////////////////////////////
// Main File:        verify_hetero.c
// This File:        verify_hetero.c)
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
// Online sources:   stackoverflow.com, reddit.com/r/C_Programming/
//                   cprogramming.com, piazza.com, 
//                   youtube.com/watch?v=D13dFLpHFJg
//                   geeksforgeeks.org/magic-square/
////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
    int size;
    int **array;
} Square;

void write_to_file(char *filename, Square * sq)              
{

  FILE * fp;
  fp = fopen (filename, "w+");
  fprintf(fp, "%d\n", sq->size);
  for(int i = 0; i < sq->size; i++) { //print value of array
    for(int j = 0; j < sq->size; j++) {
			fprintf(fp, "%d ", *((*(sq -> array + i) + j)));
    }
    fprintf(fp, "\n");
  }
  fclose(fp);
}

/* generate_magic constructs a magic square of size n
 * using the Siamese algorithm and returns the Square struct
 */
Square * generate_magic(int n, char *filename)           
{

  Square* sq = (Square*)malloc(sizeof(Square));
  int size = n; //size of array
  sq->size = size; //assign this to struct
  int *arr[size]; //set size of array based on size
  for(int i = 0; i < size; i++) {
    arr[i] = malloc(size*sizeof(int));
  }

  sq->array = arr; //assign this array to struct and return  
  int x = size/2;
  int y = size-1;
  int count = 1;

  while (count  <= (size * size) ) { //counts from 1 to size^2
    if (x == -1 && y == size) {
      y = size - 2;
      x = 0;
    }
    else {
      if (y == size)
        y = 0;
      if (x < 0)
        x = size -1 ;
    }
    if (*((*(sq -> array + x) + y))){
      y -= 2;
      x++;
      continue;
    }
    else {
      *((*(sq -> array + x) + y)) = count++; 
      y++; 
      x--; 
    }
  }
  write_to_file(filename, sq);

  return 0;

}

/* get_square_size prompts the user for the magic square size
 * checks if it is an odd number >= 3 and returns the number
 */
int get_square_size()            
{
  int conditions = 0;
  int size = 0;
  char input[100];
  printf("%s\n", "Enter size of magic square, must be odd");
  fgets(input,100,stdin);
  size = atoi(input); 
  while (conditions == 0){

    if(size % 2 == 0) {
      printf("%s\n", "Enter size of magic square, must be odd"); 
      fgets(input,100,stdin);
      size = atoi(input);          
    }                  
    if( size % 2 != 0 && size <= 2) {            
      printf("%s\n", "Size must be an odd number >= 3.");
      fgets(input,100,stdin);
      size = atoi(input);
    }            

    if((size % 2) != 0 && size > 2) {
      conditions = 1; 
    }  
  }   
  return size;
}

int main(int argc, char *argv[])                 
{
  if (argc != 2) { //check for correct amount of arguments
    printf("%s\n", "Incorrect amount of arguments");
    return 1;
  } 
  int size = get_square_size();         
 
  generate_magic(size, argv[1]); //call function

    return 0;
}



