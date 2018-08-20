#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "prod_cons.h"

/* Determine key, given character */
void set_key(key_t* key, char uniq) {
  /* make key */
  /* Note: get inode + character to hopefully get unique key */
  char* filename = getenv("MTS_IPC");
  if(filename == NULL) {
    fprintf("Missing MTS_IPC environmental variable.\n");
    exit(1);
  }
  if((*key = ftok(filename, uniq)) == -1) {
    perror("ftok");
    exit(1);
  }
}

int get_shmid(int create) {
  key_t key;
  int shmid;

  /* Get key */
  set_key(&key, 'B');
  
  /* get or create shared memory segment */
  int shmflags = create ? (0666 | IPC_CREAT) : 0666;
  
  if((shmid = shmget(key, SHM_SIZE, shmflags)) == -1) {
    perror("shmget");
    exit(1);
  }
  return shmid;
}

void* get_shared_buff(int create) {
  char* data;

  int shmid = get_shmid(create);

  /* attach to the segment to get a ptr */
  data = shmat(shmid, (void*)0, 0);
  if(data == (char*)(-1)) {
    perror("shmat");
    exit(1);
  }

  return data;
}

/* Really get semid for semaphores */
int get_semaphores(int create) {
  key_t key;
  struct semid_ds buf;
  struct sembuf sb;
  int semid;
  
  /* Get key */
  set_key(&key, 'B');

  int semflags = create ? 0666 | IPC_CREAT : 0666;
  semid = semget(key, 1, semflags);

  if(create) {
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;

    if(semop(semid, &sb, 1) == -1) {
      perror("semop: semaphore initialization");
      exit(1);
    }
  }

  return semid;
}

/* Abstract away some semaphore details to lock */
void lock_buff(int semid) {
  struct sembuf sop;
  
  // index of desired sem
  sop.sem_num = 0;

  // op of desired sem (in this case, wait until 1 'resource' is available
  sop.sem_op = -1;

  // flag to ensure semaphore gets reset in the case of unclean exit
  sop.sem_flg = SEM_UNDO;

  // apply sop to semaphore `semid`
  semop(semid, &sop, 1); 
}

/* Abstract away some semaphore detail to unlock */
void unlock_buff(int semid) {
  struct sembuf sop;
  
  // index of desired sem
  sop.sem_num = 0;

  // op of desired sem (in this case, release 1 'resource')
  sop.sem_op = 1;

  // flag to ensure semaphore gets reset in the case of unclean exit
  sop.sem_flg = SEM_UNDO;

  // apply sop to semaphore `semid`
  semop(semid, &sop, 1); 
}
