#include "validator.h"
#include "explorer.h"
#include "stdbool.h"

static char fbn_bitmap[BSIZE];
// inode on traverse
 // 1 : validate type, 2 : validate address, 4 : validate dir, 5 : bitmap lookup, 10 : when dir, not valid inode
int validate_inode(struct dinode* inode)
{

    // 1

    // 2 inode start < inode < bitmap start

    // 4 if type==dir, check dir ent

    // 5 lookup fbn bitmap, if 0, not valid

    // 10 ??

    // update ref count map

    // mark fbn bitmap

}


// when traverse inode// In this function, if already marked, violation 7
int mark_fbn_bitmap(int fbn)
{
    // TODO : change fbn to bit representation

    // if bitmap[fbn] == 1, violation

    // my_bitmap[fbn] = 1;

}

// refermap, compare with inode_map
int mark_refer_countmap(struct dirent* dir_ent)
{
    /*
            check ent's inode number is valid (within inode number limit)

            refer_countmap[ent->inode_num]++;
    */
}


// traverse inode sector, compare with ref countmap
int validate_ref_count(int type, struct dinode* inode)
{
    /*
        inode->ref_count != refer_countmap[inode->num] => violation
    */
}

// traverse image bitmap, compare with fbn_bitmap
int validate_bitmap(char* bitmap_sector)
{
    /*
        for (char i = 0; i < BSIZE; i++):
            bitmap_sector[i] != fbn_bitmap[i] => violation
    */
}