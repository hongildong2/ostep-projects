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
    block_t* bp = mmap(0, BSIZE * fs_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (bp == MAP_FAILED) {
        goto error;
    }

    
    // 된다. 블록 진행시키면서 그냥 정보 뽑으면됨.
    struct superblock* sb;
    sb = (struct superblock*) (bp + 1); // advance to sector1 to get superblock information
    printf("mmaped at %p@@ \ninfo size : %d, nblocks : %d, logstart: %d, inode start sector : %d\n", bp,fs_size, sb->nblocks, sb->logstart, sb->inodestart);

    uint log_count = sb->nlog;
    char* log_p = (char*) (bp + sb->logstart);

    uint inodes_count = sb->ninodes;
    // 이게 맞다..
    struct dinode* inode_p = (void*) (bp + sb->inodestart); // fucking segfault 아마 어제는 0참조한듯

    inode_p++; // to inode number 1, inode number 0, 블록시작자리에는 아무것도 없어서 어제 0 참조함

    short a = inode_p->size;
    uint* addrs = inode_p->addrs; // contains inode's data block's file block number
    // if this is file, contains file byte
    // if this is directory, contains directory entry data
        // if dirent, use struct dirent
    // currently this is root directory

    // root inode number is 1, thus starting at offset 0x0 0004040 , 0x 4000 에는 inode 0번 자리인데 그런건 없음
    printf("inode block starting at %p, root inode with number 1 is starting at %p\n", bp + sb->inodestart + 1, inode_p);
    printf("inode struct size : %lu root folder size : %d\n", sizeof(struct dinode), a);
    // root folder size should be 512
    

    uint data_block_count = sb->nblocks;
    char* bmap_p = (char*) (bp + sb->bmapstart);
    

    // for (int i = 0; i < NDIRECT + 1; ++i) {
    //     uint data_block_address = (uint) bp + root_inode->addrs[i];
    //     if (data_block_address == 0) {
    //         break;
    //     }

    //     char* dbp = (char*) data_block_address;
    //     for (int j = 0; j < BSIZE; ++j) {
    //         printf("%c", dbp[j]);
    //     }
    // }
    
    
error:
    perror("error flag setted ");
    if (fd != -1){
        close(fd);
    }
    munmap((void*) bp, sizeof(block_t) * fs_size);
}