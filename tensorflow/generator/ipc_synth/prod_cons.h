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

// 1 GB -- NOTE: IF YOU MAKE THIS SMALLER BE SURE PRODUCER_LAP_PREVENTION_NUM is comparable
#define SHM_SIZE 1073741824


// More of a guessed size for a `big` image
// to determine when producers should sleep
// ref 
#define MAX_IMAGE_WIDTH 1000
#define MAX_IMAGE_HEIGHT 32
#define MAX_IMAGE_SIZE MAX_IMAGE_HEIGHT*MAX_IMAGE_WIDTH

// Upper limit to word length
#define MAX_WORD_LENGTH 31

// Offset into buffer where data chunks are stored
#define START_BUFF_OFFSET sizeof(uint64_t)

// magic num to specify 'able to consume' ("eatme")
#define SHOULD_CONSUME (uint64_t)0x6561746d65

// magic num to specify that a chunk has already been consumed ("used")
#define ALREADY_CONSUMED (uint64_t)0x75736564

// Size of chunk w/o image
#define BASE_CHUNK_SIZE sizeof(SHOULD_CONSUME) + (MAX_WORD_LENGTH + 1)*sizeof(char) \
                        + sizeof(uint32_t) + sizeof(uint64_t)

// Magic number for producers to write to tell consumer to wrap
#define NO_SPACE_TO_PRODUCE (uint64_t)0xc001be9

// Producers should stop producing when they are this many images away from lapping consumer
// NOTE: should be tuned according to SHM_SIZE and the slow-ness of the consumer
// Sufficiently large SHM_SIZE and large enough PRODUCER_LAP_PREVENTION_NUM should lead
// to appropriate behavior in all cases

// Preventing race condition where producers overwrite their old data before consumer gets to it,
// and corrupts memory. The consumer will check this every time it consumes. Therefore, if the
// consumer is slow and the PRODUCER_LAP_PREVENTION_NUM is insufficient,
// then the producers can overwrite data before the consumer is able to check offsets
#define PRODUCER_LAP_PREVENTION_NUM 1000
#define PRODUCER_LAP_PREVENTION_SIZE PRODUCER_LAP_PREVENTION_NUM*(MAX_IMAGE_SIZE+BASE_CHUNK_SIZE)

void* get_shared_buff(int create);
int get_semaphores(int create);
int get_shmid(int create);
void lock_buff(int semid);
void unlock_buff(int semid);

#endif
