#include <assert.h>

#include "validator.h"
#include "explorer.h"
#include "resource_manager.h"
#include "stdbool.h"
#include "types.h"

static char s_fbn_bitmap[BSIZE];
static uint s_inode_refer_countmap[NINODES];

// inode on traverse
 // 1 : validate type, 2 : validate address, 4 : validate dir, 5 : bitmap lookup, 10 : when dir, not valid inode
int validate_inode(struct superblock* sb, struct dinode* inode)
{
    assert (sb != NULL && inode != NULL);
    // 1. valid file type
    {
        if (inode->type != T_FILE && inode->type != T_DIR && inode->type != T_DEV) {
            exit_on_failure("ERROR: bad inode.");
        }
    }

    // 2. 0 < fbn < file block count
    {
        int fbn_count = sb->nblocks;
        uint* addrs = inode->addrs;
        
        // direct check
        while (*addrs != 0) {
            if (*addrs <= 0 || *addrs >= fbn_count) {
                exit_on_failure("ERROR: bad direct address in inode.");
            }
            addrs++;
        }

        // indirect check
        // look into inode size, i have to decide this file has INDIRECT fbns or not
        // if inode->size is bigger than NDIRECT * BSIZE, THERE IS INDIRECT addrs
        // ssibal
        int indirect_count = inode->size / BSIZE - NDIRECT + 1;
        if (indirect_count == 0) {
            goto inode_fbn_check_done;
        }

        // for existing INDIRECTed fbns, check their fbn is valid as above.
        for (int i = 0; i < indirect_count; ++i) {
            uint indirects_fbn = addrs[i];
            // fbn 가서 거기있는 fbn들도 순회 해야함
            uint* indirected_fbns = fbn_to_user_address(indirects_fbn);
            assert (indirected_fbns != NULL);
            
            while (*indirected_fbns != 0) {
                if (*indirected_fbns <= 0 || *indirected_fbns >= fbn_count) {
                    exit_on_failure("ERROR: bad indirect address in inode.");
                }
                indirected_fbns++;
            }
        }
    }
inode_fbn_check_done:


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