#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

#include <stdlib.h>

void register_fd(int fd);

void register_mmap(void* mp, size_t size);

void release_resources();

void exit_on_failure(char* message) __attribute__ ((noreturn));

#endif /* RESOURCE_MANAGER_H */