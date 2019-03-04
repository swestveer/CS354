/* Name:Shawn Westveer  
 * CS login: westveer@wisc.edu
 * Section(s):001
 *
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss plus a possible eviction.
 *  2. Instruction loads (I) are ignored.
 *  3. Data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus a possible eviction.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * This is crucial for the driver to evaluate your work. 
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/****************************************************************************/
/***** DO NOT MODIFY THESE VARIABLE NAMES ***********************************/

/* Globals set by command line args */
int s = 0; /* set index bits */
int E = 0; /* associativity */
int b = 0; /* block offset bits */
int verbosity = 0; /* print trace if set */
char* trace_file = NULL;

/* Derived from command line args */
int B; /* block size (bytes) B = 2^b */
int S; /* number of sets S = 2^s In C, you can use the left shift operator */

/* Counters used to record cache statistics */
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;
/*****************************************************************************/


/* Type: Memory address 
 * Use this type whenever dealing with addresses or address masks
 */
typedef unsigned long long int mem_addr_t;

/* Type: Cache line
 * TODO 
 * 
 * NOTE: 
 * You might (not necessarily though) want to add an extra field to this struct
 * depending on your implementation
 * 
 * For example, to use a linked list based LRU,
 * you might want to have a field "struct cache_line * next" in the struct 
 */
typedef struct cache_line {                     
    char valid;
    mem_addr_t tag;
    int counter;
} cache_line_t;

typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;


/* The cache we are simulating */
cache_t* cache;  

// Initiates cache based on input
void initCache() {
    // Allocate an array of size S 
    cache = malloc(S*sizeof(cache_set_t));
    // Allocate E lines 
    for (int i = 0; i < S; i++) { 
      cache[i] = malloc(E*sizeof(cache_line_t));
      // For eash line setup up cache_line structure
      for (int j = 0; j < E; j++) {
        cache[i][j] = (cache_line_t*)malloc (sizeof(cache_line_t));
        cache[i][j]->valid = 0;
        cache[i][j]->tag = 0;
        cache[i][j]->counter = 0;
      }
    }
}
// Frees the initiated Cache
void freeCache() { 
    // iterate through sets
    for (int i = 0; i < S; i++) { 
      // interate through lines   
      for (int j = 0; j < E; j++) {
        // free the cache_line
        free(cache[i][j]);
      }
      // free set
      free(cache[i]);
    }
    // free cache
    free(cache);
}

void accessData(mem_addr_t addr) {
    mem_addr_t s_Mask = (S - 1);
    mem_addr_t b_Mask = (B - 1);
    s_Mask <<= (b);
    mem_addr_t t_Mask = ~(b_Mask | s_Mask);
    mem_addr_t s_Index = (s_Mask & addr) >> b;
    mem_addr_t tag = (t_Mask & addr) >> (s + b);
    
    char hit = 0;
    char full_Set = 1;
    int emptyl_Index = 0; 
    
    // look through cache for hits
    for (int l_Index = 0; l_Index < E; l_Index++) {
      // Check for valid bit and check tag for match
      if ((cache[s_Index][l_Index])->valid) {
        if ((cache[s_Index][l_Index])->tag == tag) {
          hit_cnt++;
          
          // Increase counter for lines and reset current line
          for (int i = 0; i < E; i++) {  
            if ((cache[s_Index][i])->valid){ 
              ((cache[s_Index][i])->counter)++;
              (cache[s_Index][l_Index])->counter = 0;
            }
            hit = 1;
          }
          // Break if there is a hit
          break;       
        }
      }else {
          emptyl_Index = l_Index;
          full_Set = 0;
        }
    }
      // if there are no hits
      if (hit == 0) {
        miss_cnt++;;
        // Checks to see if all lines are full
        if (full_Set == 0) {
          (cache[s_Index][emptyl_Index])->valid = 1;
          (cache[s_Index][emptyl_Index])->tag = tag;
          // Increase counter for lines and reset current line
          for (int i = 0; i < E; i++) {
            if ((cache[s_Index][i])->valid) {
              ((cache[s_Index][i])->counter)++;
              (cache[s_Index][emptyl_Index])->counter = 0;
            }
          }
        }else {
          // When set is full and eviction is needed
          evict_cnt++;
          int count_l_Index = 0;
          int high_Count = 0;
          // Finds the line with the highest count for eviction 
          for (int i = 0; i < E; i++) {
            if ((cache[s_Index][i])->counter > high_Count) {
              high_Count = (cache[s_Index][i])->counter;
              count_l_Index = i;
            }
          }
          (cache[s_Index][count_l_Index])->tag = tag;
          for (int i = 0; i < E; i++) {
            ((cache[s_Index][i])->counter)++;
          }
          (cache[s_Index][count_l_Index])->counter = 0;                   
        }
      }                        
}

void replayTrace(char* trace_fn) {                      
    char buf[1000];
    mem_addr_t addr = 0;

    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if (!trace_fp) {
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while (fgets(buf, 1000, trace_fp) != NULL) {
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
          sscanf(buf+3, "%llx,%u", &addr, &len);
            
          int memAccess = 1;
          if (verbosity)
            printf("%c %llx,%u ", buf[1], addr, len);
          if (buf[1] == 'M') {
              memAccess = 2;
          }
          // call accessData need times based on type of call
          for (int i = 0; i < memAccess; i++) {
            accessData(addr);
          }

          if (verbosity)
            printf("\n");
        }
    }

    fclose(trace_fp);
}

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[]) {                 
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * printSummary - Summarize the cache simulation statistics. Student cache simulators
 *                must call this function in order to be properly autograded.
 */
void printSummary(int hits, int misses, int evictions) {                        
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

/*
 * main - Main routine 
 */
int main(int argc, char* argv[]) {                      
    char c;
    
    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                printUsage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                printUsage(argv);
                exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }
    
    B = pow(2, b);
    S = pow(2, s);
    
    /* Initialize cache */
    initCache();

    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_cnt, miss_cnt, evict_cnt);
    return 0;
}
