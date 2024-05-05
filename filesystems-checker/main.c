#include <stdio.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

#include "types.h"


int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("usage fsck <fs_image_file_name>\n");
        exit(1);
    }

    const char* const file_name = argv[1];
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        goto error;
    }

    // get file system size first for mapping
    uint fs_size;
    lseek(fd, BSIZE, SEEK_SET);
    if(read(fd, &fs_size, sizeof(uint)) < 0) {
        goto error;
    }

    // mapping fs.img to my address space. Starting from superblock
    block_t* bp;
    bp = mmap(NULL, sizeof(block_t) * fs_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (bp == MAP_FAILED) {
        goto error;
    }
    bp++; // advance to superblock

    // 된다. 블록 진행시키면서 그냥 정보 뽑으면됨.
    struct superblock* sb;
    sb = (void*) bp;
    printf("mmaped info size : %d, nblocks : %d, logstart: %d\n",sb->size, sb->nblocks, sb->logstart);



    
error:
    perror("error flag setted ");
    if (fd != -1){
        close(fd);
    }
}