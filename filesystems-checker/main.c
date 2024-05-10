#include <stdio.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

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
    sb = (struct superblock*) (bp + 1); // advance to sector 1 to get superblock information


    block_t* initiated = init_explorer(bp, sb);
    if (initiated != bp) {
        exit_on_failure("ERROR: FS IMAGE IS INCONSISTENT WITH SUPERBLOCK DESCRIPTION");
    }

    // root inode number is 1, thus starting at disk location 0x00004040 which is 0x00004000 + sizeof(struct inode) * inum
    // 0x00004000 에는 inode 0번 자리인데 그런건 inode는 없다.
    uint inodes_count = sb->ninodes;
    struct dinode* inode_sector_p = (void*) (bp + sb->inodestart); // fucking segfault 아마 어제는 0참조한듯)
    struct dinode* root_inode = inode_sector_p + 1; // to inode number 1, inode number 0, 블록시작자리에는 아무것도 없어서 어제 0 참조함
    assert (root_inode == inode_num_to_user_address(1));

    // addrs[0] is garbage value, IDK why, Should use offsetted one
    uint* addrs = get_offsetted_fbns((uint*)root_inode->addrs); // contains inode's data block's file block number
    
    struct dirent* root_data_block = fbn_to_user_address(addrs[0]);
    {
        // root link verification
        struct dirent dot_entry = root_data_block[0];
        mark_refer_countmap(root_data_block);
        struct dirent dotdot_entry = root_data_block[1];
        mark_refer_countmap(root_data_block + 1);
        if (dot_entry.inum != 1 || dotdot_entry.inum != 1 || dot_entry.name[0] != '.' || dotdot_entry.name[1] != '.')
        {
            exit_on_failure("ERROR: root directory does not exist");
        }
    }
    // passing dot, dotdot entry. Adding children to queue

    char inum_visited[NINODES] = { 0, };
    inum_visited[1] = 1; // hello, root and your children
    for (int i = 2; i < NDIRECT + 1; ++i)
    {
        uint child_inum = root_data_block[i].inum;
        mark_refer_countmap(root_data_block + i);
        if (child_inum == 0)
        {
            break;
        }
        enqueue(child_inum);
        inum_visited[child_inum]++;
    }


    
    
    while (!is_empty()) {
        uint inode_number = dequeue();
        assert (inode_number != 1 && inum_visited[inode_number] == 1);
        struct dinode* inode = inode_num_to_user_address(inode_number);

        validate_inode(sb, inode, inode_number);

        // enqueue
        uint* fbns = get_offsetted_fbns(inode->addrs);
        struct dirent* entries;
        for (int i = 0; i < NDIRECT; ++i) // addrs count is limited
        {

            uint fbn = fbns[i];
            if (fbn < 0 || fbn > sb->nblocks) {
                break;
            }

            switch (inode->type) {
                case T_FILE:
                    // fbn marking is handled in inode validation
                    break;
                case T_DIR:
                    entries = (struct dirent*)fbn_to_user_address(fbn);
                    for (int i = 0; i < NDIRECT + 1; ++i)
                    {
                        struct dirent entry = entries[i];
                        if (entry.name[0] == '.')
                        {
                            continue;
                        }
                        if (entry.inum == 0)
                        {
                            break;
                        }


                        enqueue(entries[i].inum);
                        inum_visited[entries[i].inum]++;
                        mark_refer_countmap(&entries[i]);
                    }

                    break;

                default:
                exit_on_failure("ERROR: bad inode");
            }
        }
    
    }
    
    // traverse inode sector and validate
    validate_ref_count(inode_sector_p);

    // validate bitmap consistency
    char* bmap_sector_p = (char*) (bp + sb->bmapstart);
    validate_bitmap(bmap_sector_p);    
  
    release_resources();
    return 0;
}