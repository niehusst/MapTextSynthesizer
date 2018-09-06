#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/resource.h>

#include "prod_cons.h"
#include "ipc_consumer.h"
#include "mts_ipc.h"

/* Necessary if we(I) don't want to make another class... */
void* g_buff;
int  g_semid;
uint32_t  g_have_buff_lock;
uint64_t  g_consume_offset;

/* For respawning producers via signal handler */
char* g_config_file; 
int g_num_producers;

/* Fork & exec a single producer */
void fork_and_exec_producer(const char* config_file) {
  int fstatus = fork();
  if(fstatus == -1) {
    fprintf(stderr, "Fork failed!");
    exit(1);
  } else if(fstatus == 0) {

    // Send SIGHUP to this process when parent dies
    prctl(PR_SET_PDEATHSIG, SIGHUP);

    // Producer leaks memory, so make it crash by limiting heap
    // (and saving the rest of the system processes)
    struct rlimit data_limit;
    data_limit.rlim_cur = PRODUCER_DATA_LIMIT;
    data_limit.rlim_max = PRODUCER_DATA_LIMIT;
    if(setrlimit(RLIMIT_DATA, &data_limit)) {
      perror("setrlimit");
      exit(1);
    }
    
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
}

/* Spawn producers */
void fork_and_exec_producers(int num_producers, const char* config_file) {
  for(int i = 0; i < num_producers; i++) {
    fork_and_exec_producer(config_file);
    // Sleep for a hot sec to avoid identically-seeded synthesizers
    // (default seeds based on time in seconds, which is kinda lame)
    sleep(1);
  }
}

/* Fork & exec base */
void fork_and_exec_base(int* pid) {
  *pid = fork();
  if(*pid == -1) {
    // Failed
    fprintf(stderr, "Fork failed!");
    exit(1);
  } else if(*pid == 0) {
    // In forked ps...
    
    // Send SIGHUP to this process when parent dies
    // (Shouldn't be necessary because this dies on its own pretty fast)
    prctl(PR_SET_PDEATHSIG, SIGHUP);

    // Prep args and exec `base`
    char* args[2];
    args[0] = "base";
    args[1] = NULL;
    if(execvp(args[0], args)) {
      perror("exec base");
      exit(1);
    }
  }   
}

/* NOTE: This assumes the only child processes are producers */
void dead_child_handler(int signo) {
  int wstatus;
  while(waitpid(0, &wstatus, WNOHANG || WEXITED || WUNTRACED) > 0) {
    /* Not sure where to get this config file from?? */
    fork_and_exec_producer(g_config_file);
  }
}

/* Set up signal handler to spawn a producer when SIGCHLD is received */
void init_producer_respawn(void) {
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = &dead_child_handler;
  sigaction(SIGCHLD, &sa, NULL);
}

/* Perform necessary operations for prepping IPC */
void mts_ipc_init(int num_producers, const char* config_file) {
  /* Prepare shared memory and semaphores */
  pid_t pid;
  int wstatus;
  fork_and_exec_base(&pid);

  // Wait until base terminates
  waitpid(pid, &wstatus, WUNTRACED);

  /* Deal with the inevitable crashing of producers */
  g_config_file = (char*)config_file;
  g_num_producers = num_producers;
  init_producer_respawn();
  
  /* Start producers */
  fork_and_exec_producers(num_producers, config_file);

  /* Prepare for consumption */
  g_buff = (void*)((intptr_t)get_shared_buff(0));
  g_semid = get_semaphores(0);
  g_have_buff_lock = 0;
  g_consume_offset = START_BUFF_OFFSET;

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
