#include "lock.h"
#include <pthread.h>

void init_lock(lock_t* l, int partition)
{
    pthread_mutex_init(&l->lock, NULL);
    l->partition = partition;
}
void lock(lock_t* l)
{
    pthread_mutex_lock(&l->lock);
}
void unlock(lock_t* l)
{
    pthread_mutex_unlock(&l->lock);
}