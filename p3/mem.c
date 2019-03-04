////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
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
// Online sources:   reddit.com/r/C_Programming/
//                   cprogramming.com, piazza.com, 
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses 
 */
typedef struct blk_hdr {                         
        int size_status;
  
    /*
    * Size of the block is always a multiple of 8
    * => last two bits are always zero - can be used to store other information
    *
    * LSB -> Least Significant Bit (Last Bit)
    * SLB -> Second Last Bit 
    * LSB = 0 => free block
    * LSB = 1 => allocated/busy block
    * SLB = 0 => previous block is free
    * SLB = 1 => previous block is allocated/busy
    * 
    * When used as the footer the last two bits should be zero
    */

    /*
    * Examples:
    * 
    * For a busy block with a payload of 20 bytes (i.e. 20 bytes data + an additional 4 bytes for header)
    * Header:
    * If the previous block is allocated, size_status should be set to 27
    * If the previous block is free, size_status should be set to 25
    * 
    * For a free block of size 24 bytes (including 4 bytes for header + 4 bytes for footer)
    * Header:
    * If the previous block is allocated, size_status should be set to 26
    * If the previous block is free, size_status should be set to 24
    * Footer:
    * size_status should be 24
    * 
    */
} blk_hdr;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
blk_hdr *first_blk = NULL;

/*
 * Note: 
 *  The end of the available memory can be determined using end_mark
 *  The size_status of end_mark has a value of 1
 *
 */

/* 
 * Function for allocating 'size' bytes
 * Returns address of allocated block on success 
 * Returns NULL on failure 
 * Here is what this function should accomplish 
 * - Check for sanity of size - Return NULL when appropriate 
 * - Round up size to a multiple of 8 
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks
 * Tips: Be careful with pointer arithmetic 
 */
void* Mem_Alloc(int size) {
    blk_hdr *location = first_blk;
    blk_hdr *hold = first_blk;
    int req_size = 0;
    int temp = 0;
    int count = 0;
    int last_digit = 0;
    char lo = NULL;
    
    //Ensure size is valid
    if(size <= 0) {
      return NULL;
    }
    
    //Sets req_size 
    if(size % 8 != 0) {
      req_size = size + (8 - (size % 8));
      if(req_size - size < 4) {
        req_size += 8;
        }    
    } 
    if(size % 8 == 0) {
      req_size = size + 8;
    }

    while( location->size_status != 1) {
    
      lo = (char*)location;
      last_digit = (lo % 0x00000008);
      
      //ensures double-word(8 bytes) aligned
      if((last_digit % 8) != 0 && (location->size_status & 1) == 0 
      && location->size_status != 0) {
        blk_hdr *temp_blk = location;
        int temp_size = location->size_status;
        while(temp_blk->size_status % 8 != 0) {
          temp_blk++;
          count++;
        }  
        location->size_status = (count * 4) + 3;
        for(int i = 0; i < count - 1; i++) {
          location++;
        } 
        location++;
        location->size_status = temp_size - (count * 4); 
      }

      //if Block is free and large enough for request, fills request
      if( location->size_status > req_size && (location->size_status & 1) == 0 ) {
        hold = location;
        temp = location->size_status - 2;
        location->size_status = req_size + 3;
        
        for(int i = 0; i < (req_size / 4) - 1; i++) {
          location++;
        }
          location++;
          if(temp - req_size > 0){
            location->size_status = (temp - req_size) + 2;
          }
          if(temp - req_size == 0){
            location->size_status += 2;
          }
          location += ((temp - req_size) / 4) - 1;
          location->size_status = (temp - req_size);
          
          location++;

          return hold;  
          //break; 
      }      
      location++;
       
    }
    
    return NULL;
}

/* 
 * Function for freeing up a previously allocated block cd ..
 * Argument - ptr: Address of the block to be freed up 
 * Returns 0 on success 
 * Returns -1 on failure 
 * Here is what this function should accomplish 
 * - Return -1 if ptr is NULL
 * - Return -1 if ptr is not 8 byte aligned or if the block is already freed
 * - Mark the block as free 
 * - Coalesce if one or both of the immediate neighbours are free 
 */
int Mem_Free(void *ptr) {                        
    blk_hdr *location = first_blk;
    blk_hdr *prior = first_blk;
    blk_hdr *next = first_blk;
    blk_hdr *hold = first_blk;
    int size = 0;
    int p_size = 0;
    int n_size = 0;
    int total = 0;
    //int temp = 0;
    int count = 0;
    int last_digit = 0;
    char lo = NULL;

    //Ensure pointer it not NULL
    if(ptr == NULL) {
      return -1;
    }
    
    
    lo = (char*)ptr;
    last_digit = (lo % 0x00000008);
    if(last_digit % 8 != 0) {
      return -1;
    }
    
    while( location->size_status != 1 ) {
      if(location == ptr){
        location = ptr;
        count++;
        break;
      }
      prior = location;
      p_size = location->size_status;
      location++;
    }
    //ensure ptr matches adressess in range
    if(count == 0){
      return -1;
    }
    
    //Find size of block to be freed
    size = location->size_status;
    if (size & 1) {
      size = size - 1;
    } 
    if (size & 2) {
      size = size - 2;
    }
    
    //if prior block if busy
    if(prior->size_status == 0 || prior->size_status == 7){
      blk_hdr *next = location +(size / 4);
      n_size = next->size_status;

      //if next block is free
      if ((n_size % 2) == 0) {

        total = size + n_size;
        location->size_status = total;
      
        for(int i = 0; i < (n_size / 4) + 1; i++) {
          location++;
        } 
          
        location->size_status = total;

        return 0;
      } 

      //if next block is busy
      location->size_status = size + 2;
    
      for(int i = 1; i < size / 4; i++) {
        location++;
        }
      location->size_status = size;
      next->size_status = next->size_status - 2;
      return 0;
        
    }
    
    //if prior block is free
    if(prior->size_status > 0 && prior->size_status != 7){
      blk_hdr *next = location +(size / 4);
      n_size = next->size_status;
      
      //if next block is free
      if ((n_size % 2) == 0) {
          n_size = n_size - 2;
      
          for(int i = 0; i < p_size / 4; i++) {
            location--;
          }
          
          total = size + location->size_status + n_size;
          location->size_status = total;
      
          for(int i = 0; i < p_size / 4; i++) {
            location++;
          } 
          
          location->size_status = total -2;
          return 0;
      } 
      
      //if next block is busy
      for(int i = 0; i < p_size / 4; i++) {
        location--;
      }
      total = size + location->size_status;
      location->size_status = total;
      
      for(int i = 1; i < (total -2) / 4; i++) {
        location++;
      }
      location->size_status = total - 2; 
      location++;
      location->size_status = location->size_status - 1;
      
     return 0; 
    }


    return -1;
}

/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 * Returns 0 on success and -1 on failure 
 */
int Mem_Init(int sizeOfRegion) {                         
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    blk_hdr* end_mark;
    static int allocated_once = 0;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: Mem_Init has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 
                    fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
     allocated_once = 1;

    // for double word alignement and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that first block meets 
    // double word alignement requirement
    first_blk = (blk_hdr*) space_ptr + 1;
    end_mark = (blk_hdr*)((void*)first_blk + alloc_size);
  
    // Setting up the header
    first_blk->size_status = alloc_size;

    // Marking the previous block as busy
    first_blk->size_status += 2;

    // Setting up the end mark and marking it as busy
    end_mark->size_status = 1;

    // Setting up the footer
    blk_hdr *footer = (blk_hdr*) ((char*)first_blk + alloc_size - 4);
    footer->size_status = alloc_size;
  
    return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information i
 * for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header) (including the header/footer)
 */ 
void Mem_Dump() {                        
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    blk_hdr *current = first_blk;
    counter = 1;

    int busy_size = 0;
    int free_size = 0;
    int is_busy = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => busy block
            strcpy(status, "Busy");
            is_busy = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_busy = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "Busy");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_busy) 
            busy_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blk_hdr*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total busy size = %d\n", busy_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", busy_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}
