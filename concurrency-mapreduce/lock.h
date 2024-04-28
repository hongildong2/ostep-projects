#ifndef __LOCK_H__
#define __LOCK_H__

#include <pthread.h>

typedef struct __lock_t {
    pthread_mutex_t lock;
    int partition;
} lock_t;

void init_lock(lock_t* l, int partition);
void lock(lock_t* l);
void unlock(lock_t* l); 

#endif /* LOCK_H */