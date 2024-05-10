#include <stdlib.h>
#include <stdio.h>

#include "explorer.h"
#include "types.h"
#include "assert.h"



static void* __base_address;
static void* __inode_sector_address;
static void* __bitmap_sector_address;
static void* __data_sector_address;
static void* __log_sector_address;

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]


block_t* init_explorer(void* base_address, struct superblock* sb)
{
    __base_address = base_address;

    block_t* bp = __base_address;

    __log_sector_address = bp + sb->logstart;


    assert (__log_sector_address == (void*)sb + BSIZE); // log sector is 1 block after sb start address

    __inode_sector_address = bp + sb->inodestart;
    assert (__inode_sector_address == __log_sector_address + sb->nlog * BSIZE); // inode sector is after log sector
    
    int ninodeblocks = NINODES / IPB + 1;
    __bitmap_sector_address = bp + sb->bmapstart;
    assert (__bitmap_sector_address == __inode_sector_address + ninodeblocks * BSIZE);
    

    int nbitmap = FSSIZE / (BSIZE*8) + 1;
    __data_sector_address = bp + sb->bmapstart + nbitmap;
    assert (__data_sector_address == __bitmap_sector_address + nbitmap * BSIZE); 
    
    return bp;
}


void* fbn_to_user_address(int fbn)
{  
    assert (__base_address != NULL); 
    char offset_in_byte = BSIZE * fbn;
    return __data_sector_address + offset_in_byte;
}

struct dinode* inode_num_to_user_address(int inode_num)
{
    assert (__base_address != NULL);
    return (struct dinode*)__inode_sector_address + inode_num;
}

int inode_user_address_to_inode_num(struct dinode* inode)
{
    // Get block number that contains inode.
    // inum = inode - (struct dinode*)inode sector start address;
    return inode - (struct dinode*)__inode_sector_address;
}

uint* get_offsetted_fbns(uint addrs[NDIRECT + 1])
{
    // is addrs in inode sector?
    return addrs + 1;
}
