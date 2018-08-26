#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>

#include "prod_cons.h"

int main(void) {

  // Get and remove semaphore by ID
  int semid = get_semaphores(0);
  semctl(semid, 0, IPC_RMID);

  // Get and remove shared memory by ID
  int shmid = get_shmid(0);
  shmctl(shmid, IPC_RMID, NULL);
  
  return 0;
}
