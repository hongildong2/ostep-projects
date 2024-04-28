#ifndef __DATASTRUCTURE_H__
#define __DATASTRUCTURE_H__

#include "lock.h"

// for mapper
typedef struct __kv_t {
    char* key;
    int count;
} kv_t;


// for reducers to record kvs
typedef struct __node_t {
    __node_t* next;
    kv_t key_val;
} node_t;

// for reducer to create single statistics
typedef struct __hash_map_t {
    unsigned long (*hash_function)(char *key, int bucket_size);
    lock_t* locks; // lock for each bucket.
    node_t** buckets; // bucket, equivalent to node_t* nodes[]
    int bucket_size; // allocated bucket size
} hash_map_t;

// hash init put get...


// for mapper
typedef struct __ds_t {
    kv_t* kvs;
    int partition_num; // Mapper index
    int capacity; 
    int size; // allocated kvs
} ds_t;

void init_datastructure(ds_t* ds, int partition_num);
void destroy_datastructure(ds_t* ds);
char* get(ds_t* ds, char* key); // returns value  
void put(ds_t* ds, char* key, char* value); // partition key and put in the bucket.

#endif /* DATASTRUCTURE_H */