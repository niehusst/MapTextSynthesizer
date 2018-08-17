#ifndef MTS_IPC_H
#define MTS_IPC_H

void mts_ipc_init(int num_producers, const char* config_file);
void* mts_ipc_get_sample(void);
void mts_ipc_cleanup(void);


#endif
