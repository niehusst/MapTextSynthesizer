#ifndef MTS_IPC_H
#define MTS_IPC_H

// Fix producer to cap its data to this value
// Producer will die & respawn at this value
#define PRODUCER_DATA_LIMIT (uint64_t)2*1073741824

void mts_ipc_init(int num_producers, const char* config_file);
void* mts_ipc_get_sample(void);
void mts_ipc_cleanup(void);


#endif
