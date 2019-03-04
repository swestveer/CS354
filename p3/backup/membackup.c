#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"


      if((last_digit % 8) != 0 && (location->size_status & 1) == 0) {
        fprintf(stdout, "%s\n", "TEST-TEST");
        blk_hdr *temp_blk = location;
        int temp_size = location->size_status;
        while(temp_blk->size_status % 8 != 0) {
          fprintf(stdout, " temp_blk = t0x%08lx\n", (unsigned long int) temp_blk);
          temp_blk++;
          count++;
        }  
        fprintf(stdout, "Count = %d\n", count);
        location->size_status = (count * 4) + 3;
        for(int i = 0; i < count - 1; i++) {
          location++;
        } 
        location++;
        location->size_status = temp_size - (count * 4); 
      }




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
    //fprintf(stdout, "SIZE = %d\n", size);
    if(size % 8 != 0) {
      req_size = size + (8 - (size % 8));
      if(req_size - size < 4) {
        req_size += 8;
        }    
    } else {
        req_size = size + 8;
      }
    //fprintf(stdout, "req_size = %d\n", req_size);
    
    while( location->size_status != 1) {
    
      lo = (char*)location;
      //fprintf(stdout, "Pre-L = t0x%08lx\n", (unsigned long int)location);
      last_digit = (lo % 0x00000008);
      //fprintf(stdout, "last = %d\n", last_digit);
      
      if((last_digit % 8) != 0 && (location->size_status & 1) == 0 
      && location->size_status != 0) {
        //fprintf(stdout, "%s\n", "TEST-TEST");
        blk_hdr *temp_blk = location;
        int temp_size = location->size_status;
        while(temp_blk->size_status % 8 != 0) {
          //fprintf(stdout, " temp_blk = t0x%08lx\n", (unsigned long int) temp_blk);
          temp_blk++;
          count++;
        }  
        //fprintf(stdout, "Count = %d\n", count);
        location->size_status = (count * 4) + 3;
        for(int i = 0; i < count - 1; i++) {
          location++;
        } 
        location++;
        location->size_status = temp_size - (count * 4); 
      }
      
      if( location->size_status > req_size && (location->size_status & 1) == 0 ) {
        hold = location;
        temp = location->size_status - 2;
        //fprintf(stdout, "Temp = %d\n", temp);
        location->size_status = req_size + 3;
        for(int i = 0; i < (req_size / 4) - 1; i++) {
          location++;
        }
          location++;
          location->size_status = (temp - req_size) + 2;
          //fprintf(stdout, "Temp = %d\n", temp);
          //fprintf(stdout, "CLocSize = %d\n", location->size_status);
          //fprintf(stdout, "Free = %d\n", ((temp - req_size) / 4) - 1);
          location += ((temp - req_size) / 4) - 1;
          location->size_status = (temp - req_size);
          //fprintf(stdout, "End = %d\n", location->size_status);
          //fprintf(stdout, "HOLD = t0x%08lx\n", (unsigned long int)hold);
          //fprintf(stdout, "CLocSize = %d\n", location->size_status);
          //fprintf(stdout, "ELoc = t0x%08lx\n", (unsigned long int)location);
          return hold;  
          //break; 
      }      
       
      location++;
      //fprintf(stdout, "LocSize = %d\n", location->size_status);    
    }
      location--;
      //fprintf(stdout, "Finish = %d\n", location->size_status);
      char *t_begin = NULL;
      blk_hdr *test = NULL;
      blk_hdr *loc = first_blk;
      loc += 1021;
      fprintf(stdout, "LsizeL = %d\n", loc->size_status);

      //blk_hdr *current = first_blk;
      for(int i = 0; i < 16; i++) {
        blk_hdr *test = first_blk + i;
        //test->size_status = i;
        t_begin = (char*)test;

        fprintf(stdout, "addr = t0x%08lx\n", (unsigned long int)t_begin);    
        fprintf(stdout, "size = %d\n", test->size_status);                  
      }
 
    //fprintf(stdout, "&1 = %d\n", 0 & 1); = 0
    //fprintf(stdout, "&2 = %d\n", 1 & 1); = 1
    //fprintf(stdout, "&2 = %d\n", 2 & 1); = 0 
    //fprintf(stdout, "&2 = %d\n", 3 & 1); = 1
    //fprintf(stdout, "&1 = %d\n", 0 & 2); = 0
    //fprintf(stdout, "&2 = %d\n", 0 & 8);
    //fprintf(stdout, "&2 = %d\n", 8 & 8);
    //fprintf(stdout, "&2 = %d\n", 16 & 8);
    //fprintf(stdout, "%s\n", "TEST-TEST");     
    return NULL;
}






















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
    blk_hdr *current = first_blk;
    int heapsize = first_blk->size_status;
    fprintf(stdout, "%d\n", heapsize);
    while(current->size_status != 1) {
      fprintf(stdout, "addr = 0x%08lx\n", (unsigned long int)current);
      fprintf(stdout, "addr = %d\n", (int)current);
      if((int)current % 8 == 0) {
        fprintf(stdout, "%s\n", "TEST");
        current->size_status = 10; 
        current++;
        current->size_status = heapsize - 8;
         
      break;
      }
      current->size_status = 10;
      current++;
    }
  
    





      char *t_begin = NULL;
      blk_hdr *test = NULL;
      //blk_hdr *current = first_blk;
      for(int i = 0; i < 10; i++) {
        blk_hdr *test = first_blk + i;
        //test->size_status = i;
        t_begin = (char*)test;
        fprintf(stdout, "addr = t0x%08lx\n", (unsigned long int)t_begin);    
        fprintf(stdout, "size = %d\n", test->size_status);                  
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
    // Your code goes in here 
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





















      fprintf(stdout, "Pass_in = %d\n", location->size_status);
      //if Block is free and large enough for request, fills request
      if( location->size_status > req_size && (location->size_status & 1) == 0 ) {
        hold = location;
        temp = location->size_status - 2;
        location->size_status = req_size + 3;
        fprintf(stdout, "Starting = %d\n", location->size_status);
        for(int i = 0; i < (req_size / 4) - 1; i++) {
          location++;
        }
          location++;
          location->size_status = (temp - req_size) + 2;
          
          fprintf(stdout, "Temp = %d\n", temp);
          fprintf(stdout, "CLocSize = %d\n", location->size_status);
          fprintf(stdout, "Free = %d\n", ((temp - req_size) / 4) - 1);
          
          location += ((temp - req_size) / 4) - 1;
          location->size_status = (temp - req_size);
          
          location++;
          fprintf(stdout, "Next = t0x%08lx\n", (unsigned long int)location);
          fprintf(stdout, "N_S = %d\n", location->size_status);
          
          
          //fprintf(stdout, "End = %d\n", location->size_status);
          //fprintf(stdout, "HOLD = t0x%08lx\n", (unsigned long int)hold);
          //fprintf(stdout, "CLocSize = %d\n", location->size_status);
          //fprintf(stdout, "ELoc = t0x%08lx\n", (unsigned long int)location);
          
          return hold;  
          //break; 
      }      
      location++;
       
    }
    
    return NULL;
}
