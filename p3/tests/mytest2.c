/* first pointer returned is 8-byte aligned */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"

int main() {
   assert(Mem_Init(4096) == 0);
   void* ptr = Mem_Alloc(9);
   printf("%08x\n", (unsigned int)(ptr));
   Mem_Dump();
  
   exit(0);
}