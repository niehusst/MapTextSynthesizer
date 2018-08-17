#ifndef IPC_CONSUMER_H
#define IPC_CONSUMER_H

#include <stdint.h>

// Contains all of the raw data of a sample
typedef struct sample {
  unsigned char* img_data;
  size_t height;
  size_t width;
  char* caption;
} sample_t;

sample_t* ipc_get_sample(void* buff, uint64_t* consume_offset,
			 int semid, char* have_buff_lock);

#endif
