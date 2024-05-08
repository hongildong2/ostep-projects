#include <assert.h>

#include "validator.h"
#include "explorer.h"
#include "resource_manager.h"
#include "stdbool.h"
#include "types.h"

static char s_fbn_bitmap[BSIZE];
static uint s_inode_refer_countmap[NINODES] = { 0, };
int __validate_inum(int inum);

// inode on traverse
 // 1 : validate type, 2 : validate address, 4 : validate dir, 5 : bitmap lookup, 10 : when dir, not valid inode
int validate_inode(struct superblock* sb, struct dinode* inode)
{
    assert (sb != NULL && inode != NULL);
    // 1. valid file type
    {
        if (inode->type != T_FILE && inode->type != T_DIR && inode->type != T_DEV)
        {
            exit_on_failure("ERROR: bad inode.");
        }
    }

    // 2. 0 < fbn < file block count
    {
        int fbn_count = sb->nblocks;
        uint* addrs = inode->addrs;
        

        // direct entry check
        while (addrs != 0)
        {
            if (*addrs <= 0 || *addrs >= fbn_count)
            {
                exit_on_failure("ERROR: bad direct address in inode.");
            }

            if (mark_fbn_bitmap(*addrs) != 0)
            {
                exit_on_failure("ERROR: direct address used more than once.");
            }
            addrs++;
        }

        // indirect check
        // look into inode size, i have to decide this file has INDIRECT fbns or not
        // if inode->size is bigger than NDIRECT * BSIZE, THERE IS INDIRECT addrs
        // ssibal
        int indirect_count = inode->size / BSIZE - NDIRECT + 1;
        if (indirect_count == 0)
        {
            goto INODE_FBN_CHECK_DONE;
        }

        // validate INDIRECT and its entries
        for (int i = 0; i < indirect_count; ++i)
        {
            uint indirects_fbn = addrs[i];
            if (mark_fbn_bitmap(indirects_fbn) != 0)
            {
                exit_on_failure("ERROR: indirect address used more than once.");
            }

            // fbn 가서 거기있는 fbn들도 순회 해야함
            uint* indirected_fbns = fbn_to_user_address(indirects_fbn);

            assert (indirected_fbns != NULL);
            while (indirected_fbns != 0)
            {
                if (*indirected_fbns <= 0 || *indirected_fbns >= fbn_count)
                {
                    exit_on_failure("ERROR: bad indirect address in inode.");
                }

                if (mark_fbn_bitmap(*indirected_fbns) != 0)
                {
                    exit_on_failure("ERROR: indirect address used more than once.");
                }
                indirected_fbns++;
            }
        }
    }
INODE_FBN_CHECK_DONE:


    // 4.  if type==dir, check dir ent
    {
        if (inode->type != T_DIR)
        {
            goto INODE_DIR_CHECK_DONE;
        }

        uint* dirent_fbns = inode->addrs;
        struct dirent* entries = fbn_to_user_address(*dirent_fbns);
        int inum = inode_user_address_to_inode_num(inode);
            
        // first . entry
        struct dirent dot_entry = *entries; 
        if (dot_entry.inum != inum || dot_entry.name[0] != '.' || dot_entry.name[1] != '\0')
        {
            exit_on_failure("ERROR: directory not properly formmated.");
        }
        entries++; 
        // to second entry

        // second .. entry
        if (entries != 0)
        {
            struct dirent dotdot_entry = *entries; // second entry
            if (dotdot_entry.name[0] != '.' && dotdot_entry.name[1] != '.' && dotdot_entry.name[2] != '\0')
            {
                exit_on_failure("ERROR: directory not properly formmated.");
            }
        }
        entries++; 
        // to next entries

        while (entries != 0)
        {
            mark_refer_countmap(entries);
            entries++;
        }
    }
INODE_DIR_CHECK_DONE:
    return 0;
}


// when traverse inode// In this function, if already marked, violation 7
int mark_fbn_bitmap(int fbn)
{
    if (s_fbn_bitmap[fbn] == 1)
    {
        return -1;
    }
    s_fbn_bitmap[fbn] = 1;
    return 0;
}

// refermap, compare with inode_map
void mark_refer_countmap(struct dirent* dir_ent)
{
    assert (dir_ent == 0 && dir_ent->inum > 0 && dir_ent->inum < NINODES + 1); // bad dirent
    s_inode_refer_countmap[dir_ent->inum]++;
}


// traverse inode sector, compare with ref countmap
void validate_ref_count(struct dinode* inode_sector)
{
    char* inode_sector_start_block = (char*)inode_sector;

    assert (inode_sector != NULL);
    inode_sector++; // ignore inode 0 entry

    int inode_sector_block_counts = NINODES * sizeof(struct dinode) / BSIZE + 1;
    while (((char*) inode_sector) - inode_sector_start_block < inode_sector_block_counts * BSIZE)
    {
        if (inode_sector->type == 0)
        {
            inode_sector++; // Deleted or inum 0, advance to next sector
            continue;
        }
        int inum = inode_user_address_to_inode_num(inode_sector);
        int fs_ref_count = inode_sector->nlink;
        int map_ref_count = s_inode_refer_countmap[inum];

        if (s_inode_refer_countmap[inum] == 0)
        {
            exit_on_failure("ERROR: address used by inode but marked free in bitmap.");
        }

        // inconsistent
        if (fs_ref_count != map_ref_count)
        {
            exit_on_failure("ERROR: bad reference count for file.");
        }

        // normal directory, more than 1 link is invalid
        if (inum != 1 && inode_sector->nlink > 1)
        {
            exit_on_failure("ERROR: directory appears more than once in file system.");
        }
        inode_sector++; // advance to next sector
    }


}

// traverse image bitmap, compare with fbn_bitmap
void validate_bitmap(char* bitmap_sector)
{
    for (int i = 0; i < BSIZE; i++)
    {
        if (bitmap_sector[i] > s_fbn_bitmap[i])
        {
            // exist on fs bitmap, not in my checker
            exit_on_failure("ERROR: bitmap marks block in use but it it not in use.");
        }
        else if (bitmap_sector[i] < s_fbn_bitmap[i])
        {
            exit_on_failure("");
        }
    }
}
