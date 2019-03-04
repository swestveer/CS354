/* first pointer returned is 8-byte aligned */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "mem.h"

int main() {
  assert(Mem_Init(4096) == 0);
  int* ptr[4];

  ptr[0] = (int*) Mem_Alloc(4);
  ptr[1] = (int*) Mem_Alloc(8);
  ptr[2] = (int*) Mem_Alloc(16);
  ptr[3] = (int*) Mem_Alloc(24);
  Mem_Dump();
  
  
  assert(Mem_Free(ptr[1]) == 0);
  Mem_Dump();
  assert(Mem_Free(ptr[0]) == 0);
  Mem_Dump();
  exit(0);
}