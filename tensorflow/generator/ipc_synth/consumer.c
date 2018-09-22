#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <linux/membarrier.h>

#include "prod_cons.h"
#include "ipc_consumer.h"

/* Consume next available sample and return pointer to heap allocated sample */
sample_t* consume(intptr_t buff, uint64_t* consume_offset,
		  int semid, uint32_t* have_buff_lock) {

  /* For wrapping -- test to see if producer wrapped */
  if(*consume_offset + BASE_CHUNK_SIZE >= SHM_SIZE
     || *(uint64_t*)(buff + *consume_offset) == NO_SPACE_TO_PRODUCE) {
    *consume_offset = START_BUFF_OFFSET;
  }

  /* Nothing available to consume, return NULL */
  if(*(uint64_t*)(buff + *consume_offset) != SHOULD_CONSUME) {
    return NULL;
  }

  /* Light memory fencing to ensure appropriate 
     memory access ordering @ runtime*/
  asm volatile ("mfence" ::: "memory");
  
  /* Cache initial buff value */
  intptr_t start_buff = buff;
  
  /* Jump to the next available element (8 is to skip past `ABLE_TO_CONSUME`) */
  buff += *consume_offset + sizeof(uint64_t);
  
  sample_t* spl = (sample_t*)malloc(sizeof(sample_t));
  if(spl == NULL) {
    perror("malloc");
    exit(1);
  }

  // Stored as 64 bit int for mem alignment's sake, but don't need 64 bits
  uint32_t height = (uint32_t)*((uint64_t*)buff);
  buff += sizeof(uint64_t);

  // Extract label from data chunk
  char* label = strdup((char*)buff);
  if(label == NULL) {
    perror("strdup");
    exit(1);
  }

  // Label is hardcoded to be max MAX_WORD_LENGTH chars
  buff += (MAX_WORD_LENGTH + 1)*sizeof(char);

  // Extract size and update buff
  uint64_t sz = *((uint64_t*)buff);
  buff += sizeof(uint64_t);

  // Allocate enough space to store flat image
  unsigned char* img_flat = (unsigned char*)malloc(sz);
  if(img_flat == NULL) {
    perror("malloc");
    fprintf(stderr, "Requested %lu bytes.\n", sz);
    exit(1);
  }

  // Copy image data into img_flat
  memcpy(img_flat, (void*)buff, sz);
  buff += sz*sizeof(unsigned char);

  // Instantiate spl according to extracted values
  spl->height = height;

  // Ensure no funny business with image height/img_size relationship
  if(sz % height != 0) {
    fprintf(stderr,
	    "invalid image dimensions. size=%lu, height=%u\n",
	    sz, height);
  }
  
  spl->width = sz/height; //should be evenly divisible
  spl->caption = label;
  spl->img_data = img_flat;
  
  if(*consume_offset % 8 != 0 &&
     *consume_offset + (*consume_offset % 8) < SHM_SIZE) {
    *consume_offset += *consume_offset % 8;
  }

  // Buff is now consumed!
  memset((void*)(start_buff+*consume_offset), 1, buff-(start_buff+*consume_offset));
  //*(uint64_t*)(start_buff+*consume_offset) = (uint64_t)ALREADY_CONSUMED;

  // Update consume_offset (local)
  *consume_offset = buff - start_buff;
  
  // Update the consume_offset (visible to producers)
  *((uint64_t*)(start_buff+sizeof(uint64_t))) = *consume_offset;
  
  return spl;
}

/* Exposed via mts_ipc.h -- get sample */
sample_t* ipc_get_sample(void* buff, uint64_t* consume_offset,
		 int semid, uint32_t* have_buff_lock) {

  sample_t* spl = consume((intptr_t)buff, consume_offset,
			  semid, have_buff_lock);
  
  // If this happens, then something broke
  if(*consume_offset >= SHM_SIZE) {
    fprintf(stderr, "Consumer did not wrap appropriately.\n");
    exit(1);
  }

  return spl;
}
