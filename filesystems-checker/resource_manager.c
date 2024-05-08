#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "resource_manager.h"

static int s_fd = 0;
static void* s_mmap_p = NULL;
static size_t s_mmap_size = 0;

void register_fd(int fd)
{
    assert (s_fd == 0 && fd > 0);
    s_fd = fd;
}

void register_mmap(void* mp, size_t size)
{
    assert (s_mmap_p == NULL && s_mmap_size == 0);
    assert (mp != NULL && mp != MAP_FAILED && size > 0);
    s_mmap_p = mp;
    s_mmap_size = size;
}

void release_resources()
{
    if (s_fd != 0)
    {
        if (close(s_fd) != 0)
        {
            fprintf(stderr, "FILE CLOSE ERROR\n");
            
        }
        s_fd = 0;
    }

    if (s_mmap_p != NULL && s_mmap_p != MAP_FAILED)
    {
        if (munmap(s_mmap_p, s_mmap_size) != 0)
        {
            fprintf(stderr, "MUNMAP ERROR\n");
        }
        s_mmap_p = NULL;
        s_mmap_size = 0;
    }
}

void exit_on_failure(char* message)
{
    release_resources();
    fprintf(stderr, "%s\n", message);
    exit(1);
}