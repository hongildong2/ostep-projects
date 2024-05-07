#include <stdio.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

#include "explorer.h"
#include "types.h"
#include "validator.h"
#include "queue.h"
#include "resource_manager.h"


int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("usage fsck <fs_image_file_name>\n");
        exit(1);
    }

    const char* const file_name = argv[1];
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        exit_on_failure("ERROR : INPUT FILE OPEN FAILED");
    }
    register_fd(fd);

    // get file system size first for mapping
    uint fs_size;
    lseek(fd, BSIZE, SEEK_SET);
    if(read(fd, &fs_size, sizeof(uint)) < 0) {
        exit_on_failure("ERROR: INPUT FILE READ FAILED");
    }

    // mapping fs.img to my address space. Starting from superblock
    block_t* bp = mmap(0, BSIZE * fs_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (bp == MAP_FAILED) {
        exit_on_failure("ERROR: MMAP FAILED");
    }
    register_mmap(bp, BSIZE * fs_size);
    

    
    // 된다. 블록 진행시키면서 그냥 정보 뽑으면됨.
    struct superblock* sb;
    sb = (struct superblock*) (bp + 1); // advance to sector1 to get superblock information
    printf("mmaped at %p@@ \ninfo size : %d, nblocks : %d, logstart: %d, inode start sector : %d\n", bp,fs_size, sb->nblocks, sb->logstart, sb->inodestart);

    block_t* initiated = init_explorer(bp, sb);
    if (initiated != bp) {
        exit_on_failure("ERROR: FS IMAGE IS INCONSISTENT WITH SUPERBLOCK DESCRIPTION");
    }

    // root inode number is 1, thus starting at disk location 0x00004040, 0x00004000 에는 inode 0번 자리인데 그런건 없음
    uint inodes_count = sb->ninodes;
    struct dinode* inode_p = (void*) (bp + sb->inodestart); // fucking segfault 아마 어제는 0참조한듯
    struct dinode* root_inode = inode_p + 1; // to inode number 1, inode number 0, 블록시작자리에는 아무것도 없어서 어제 0 참조함

    struct dirent* addrs = (struct dirent*)root_inode->addrs; // contains inode's data block's file block number
    // TODO : root inode verification
    while (addrs->inum != 0) {
        // if ".", ".." continue

        // TODO : if not ".", ".." => add inode number to queue for BFS
        addrs++;
    }
    // root verification done, start BFS
    

    // TODO : BFS on child inodes

    while (!is_empty()) {
        uint inode_number = dequeue();
        struct dinode* inode = inode_num_to_user_address(inode_number);

        validate_inode(inode);


        struct dirent** entries;
        int* fbns;
        switch (inode->type) {
            case T_DIR:
                entries = (struct dirent**)inode->addrs;
                while (*entries != NULL) {
                    enqueue((*entries)->inum);
                    mark_refer_countmap(*entries++);
                    // entries++;
                }
                break;
            case T_FILE:
                fbns = inode->addrs;
                while (*fbns != 0) {
                mark_fbn_bitmap(*fbns++);
                    // fbn++;
                }
                break;
            default:
                exit(1);
                // violation
        }
    }
    
    // traverse inode sector and validate
    // validate_ref_count(int type, struct dinode *inode);

    
    // validate bitmap consistency
    char* bmap_p = (char*) (bp + sb->bmapstart);
    validate_bitmap(bmap_p);

    // ??
    uint data_block_count = sb->nblocks;
    
  
    release_resources();
    return 0;
}