#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdint.h>
#include <sys/resource.h>

#include "prod_cons.h"

int main(int argc, char *argv[]) {
  key_t key;
  int shmid;
  char* data;
  int mode;

  if(argc > 2) {
    fprintf(stderr, "usage: shmdemo [data_to_write]\n");
    exit(1);
  }
  
  // init shared memory segment (create)
  void* buff = get_shared_buff(1);

  // Get and init semaphores (create)
  get_semaphores(1);
  
  // Clear out old memory for debugging purposes
  memset(buff, 1, SHM_SIZE);
  
  /* For determining max data size... 
  struct rlimit data_limit;
  getrlimit(RLIMIT_DATA, &data_limit);
  printf("Memory alloc limit: %lu", data_limit.rlim_cur);
  */

  uint64_t start_offset = 8;
  *(uint64_t*)buff = start_offset;

  //cleanup
  /* insert cleanup here */
  
  return 0;
}
