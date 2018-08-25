#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#include <string.h>
#include <signal.h>

#include "prod_cons.h"
#include "ipc_consumer.h"
#include "mts_ipc.h"

/* Necessary if we don't want to make another class */
void* g_buff;
int  g_semid;
uint32_t  g_have_buff_lock;
uint64_t  g_consume_offset;

/* Spawn producers */
void fork_and_exec_producers(int num_producers, const char* config_file) {
  for(int i = 0; i < num_producers; i++) {
    /* Fork & exec producers */
    int fstatus = fork();
    if(fstatus == -1) {
      fprintf(stderr, "Fork failed!");
    } else if(fstatus == 0) {

      // Send SIGHUP to this process when parent dies
      prctl(PR_SET_PDEATHSIG, SIGHUP);

      // Exec a new producer
      char* args[3];
      args[0] = "producer";
      args[1] = (char*)config_file;
      args[2] = NULL;

      if(execvp(args[0], args)) {
	perror("producer exec");
	exit(1);
      }
    }

    // Sleep for a hot sec to avoid identically-seeded synthesizers
    // (default seeds based on time in seconds)
    sleep(1);
  }
}

/* Might need this eventually if used in diff environment?? */
void wait_for_children(void) {
  /* This should catch everything, hopefully */
  int wstatus;
  pid_t pid = waitpid(-1, &wstatus, WUNTRACED);
}


/* Legacy code for easier debugging */
void fork_and_exec_consumer(void) {
  /* Fork & exec consumer */
  int fstatus = fork();
  if(fstatus == -1) {
    fprintf(stderr, "Fork failed!");
  } else if(fstatus == 0) {
    //child ps //TODO FIX
    char* args[2];
    args[0] = "consumer";
    args[1] = NULL;
    execvp(args[0], args);
  }   
}

/* Fork & exec base */
void fork_and_exec_base(int* pid) {
  *pid = fork();
  if(*pid == -1) {
    fprintf(stderr, "Fork failed!");
  } else if(*pid == 0) {

    // Send SIGHUP to this process when parent dies
    // (Shouldn't be necessary because it dies on its own)
    prctl(PR_SET_PDEATHSIG, SIGHUP);
    
    char* args[2];
    args[0] = "base";
    args[1] = NULL;
    if(execvp(args[0], args)) {
      perror("exec base");
      exit(1);
    }
  }   
}

void mts_ipc_init(int num_producers, const char* config_file) {
  /* Prepare shared memory and semaphores */
  pid_t pid;
  int wstatus;
  fork_and_exec_base(&pid);
  waitpid(pid, &wstatus, WUNTRACED);

  /* Start producers */
  fork_and_exec_producers(num_producers, config_file);

  /* Prepare for consumption */
  g_buff = (void*)((intptr_t)get_shared_buff(0));
  g_semid = get_semaphores(0);
  g_have_buff_lock = 0;
  g_consume_offset = START_BUFF_OFFSET;

  // Set buffer-visible consume_offset
  *(uint64_t*)(g_buff + sizeof(uint64_t)) = g_consume_offset;
}

/* Get a sample from shared memory */
void* mts_ipc_get_sample(void) {
  sample_t* spl;

  // Poll until you get a non-null sample
  while(!(spl = (void*)ipc_get_sample(g_buff, &g_consume_offset,
				      g_semid, &g_have_buff_lock))) {
    /* tryin2consume */
  }
  return spl;
}

/* Currently unused -- retained for potential future use */
void mts_ipc_cleanup(void) {
  printf("cleanin up!\n");
}
