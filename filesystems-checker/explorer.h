#ifndef __EXPLORER_H__
#define __EXPLORER_H__

#include "types.h"

// Should Utilize BLOCK_SIZE
block_t* init_explorer(void* base_address, struct superblock* sb);
void* fbn_to_user_address(int fbn);
struct dinode* inode_num_to_user_address(int inode_num);
int inode_user_address_to_inode_num(struct dinode* inode);


#endif /* EXPLORER_H__ */