#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "prod_cons.h"

void set_key(key_t* key, char uniq) {
  /* make key */
  if((*key = ftok("/home/gaffordb/IPC_generation/base.c", uniq)) == -1) {
    perror("ftok");
    exit(1);
  }
}

void* get_shared_buff(int create) {
  key_t key;
  int shmid;
  char* data;
  int mode;

  /* Get key */
  set_key(&key, 'B');
  
  /* get or create shared memory segment */
  int shmflags = create ? (0666 | IPC_CREAT) : 0666;
  
  if((shmid = shmget(key, SHM_SIZE, shmflags)) == -1) {
    perror("shmget");
    exit(1);
  }


  /* attach to the segment to get a ptr */
  data = shmat(shmid, (void*)0, 0);
  if(data == (char*)(-1)) {
    perror("shmat");
    exit(1);
  }

  return data;
}

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
