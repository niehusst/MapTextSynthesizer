#ifndef PROD_CONS_H

/* Chunks currently defined as follows: 
{
 uint8_t consumed ,  
 uint32_t height  , 
 char buff[MAX_IMAGE_SIZE+1] ,
 uint64_t img_size ,
 unsigned char* buff[img_size]
}
 */
#include <stdint.h>

// 1 GB
#define SHM_SIZE 1073741824

// Upper limit to word length
#define MAX_WORD_LENGTH 31

// Offset into buffer where data chunks are stored
#define START_BUFF_OFFSET (sizeof(uint64_t)*2)

// magic num to specify 'able to consume' ("eat!")
#define SHOULD_CONSUME ((uint64_t)0x21746165)

// magic num to specify that a chunk has already been consumed ("used")
#define ALREADY_CONSUMED ((uint64_t)0x64657375)

// Size of chunk w/o image
#define BASE_CHUNK_SIZE sizeof(uint64_t) + (MAX_WORD_LENGTH + 1)*sizeof(char) \
                        + sizeof(uint32_t) + sizeof(uint64_t)

// Magic number for producers to write to tell consumer to wrap
#define NO_SPACE_TO_PRODUCE (uint64_t)0xc001be9

void* get_shared_buff(int create);
int get_semaphores(int create);
int get_shmid(int create);
void lock_buff(int semid);
void unlock_buff(int semid);

#endif
