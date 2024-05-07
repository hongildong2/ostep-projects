#ifndef __VALIDATOR_H__
#define __VALIDATOR_H__

#include "types.h"


// inode on traverse
int validate_inode(struct superblock* sb, struct dinode* inode); // 1 : validate type, 2 : validate address, 4 : validate dir, 5 : bitmap lookup, 10 : when dir, not valid inode

// when traverse inode
int mark_fbn_bitmap(int fbn); // In this function, if already marked, violation 7

// refermap, compare with inode_map
int mark_refer_countmap(struct dirent* dir_ent);


// traverse inode sector, compare with ref countmap
int validate_ref_count(int type, struct dinode* inode);

// traverse image bitmap, compare with fbn_bitmap
int validate_bitmap(char* bitmap_sector);

#endif /* __VALIDATOR_H__ */