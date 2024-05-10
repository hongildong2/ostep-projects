#include <assert.h>
#include <stdio.h> 

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
int validate_inode(struct superblock* sb, struct dinode* inode, uint inum)
{
    assert (sb != NULL && inode != NULL);
    // 1. valid file type
    {
        if (inode->type != T_FILE && inode->type != T_DIR && inode->type != T_DEV)
        {
            exit_on_failure("ERROR: bad inode");
        }
    }

    // 2. 0 < fbn < file block count
    {
        int fbn_count = sb->nblocks;
        uint* addrs = get_offsetted_fbns(inode->addrs);

        for (int i = 0; i < NDIRECT; ++i)
        {
            uint fbn = addrs[i];
            if (fbn == 0)
            {
                break;
            }
            if (fbn < 0 || fbn >= fbn_count)
            {
                exit_on_failure("ERROR: bad direct address in inode.");
            }

            if (mark_fbn_bitmap(fbn) != 0)
            {
                exit_on_failure("ERROR: direct address used more than once.");
            }
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
        for (int i = 1; i < indirect_count + 1; ++i)
        {
            uint indirects_fbn = addrs[i]; // 얘도 1부터?

            // fbn 가서 거기있는 fbn들도 순회 해야함
            uint* indirected_fbns = fbn_to_user_address(indirects_fbn);
            indirected_fbns = get_offsetted_fbns(indirected_fbns);
            assert (indirected_fbns != NULL);
            for (int i = 0; i < NDIRECT; ++i)
            {
                uint fbn = indirected_fbns[i];
                if (fbn == 0)
                {
                    break;
                }
                if (fbn < 0 || fbn >= fbn_count)
                {
                    exit_on_failure("ERROR: bad direct address in inode.");
                }

                if (mark_fbn_bitmap(fbn) != 0)
                {
                    exit_on_failure("ERROR: direct address used more than once.");
                }
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

        uint* dirent_fbns = get_offsetted_fbns(inode->addrs);
        struct dirent* entries = fbn_to_user_address(*dirent_fbns);
            
        // first . entry
        struct dirent dot_entry = *(entries); 
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
            mark_refer_countmap(entries);
            if (dotdot_entry.name[0] != '.' && dotdot_entry.name[1] != '.' && dotdot_entry.name[2] != '\0')
            {
                exit_on_failure("ERROR: directory not properly formmated.");
            }
        }
        entries++; 
        // to next entries

        while (entries->inum != 0)
        {
            // TODO : consider INDIRECT dir entries using dir size
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
    assert (dir_ent != 0 && dir_ent->inum > 0 && dir_ent->inum < NINODES + 1); // bad dirent
    s_inode_refer_countmap[dir_ent->inum]++;
}


// traverse inode sector, compare with ref countmap
void validate_ref_count(struct dinode* inode_sector)
{
    assert (inode_sector != NULL);
    for (int inum = 5; inum < NINODES; ++inum)
    {
        struct dinode inode = inode_sector[inum];
        if (inode.type == 0)
        {
            continue;
        }

        ushort fs_ref_count = inode.nlink;
        int map_ref_count = s_inode_refer_countmap[inum];

        if (s_inode_refer_countmap[inum] == 0)
        {
            // exit_on_failure("ERROR: address used by inode but marked free in bitmap.");
        }

        if (map_ref_count == 0 && fs_ref_count > 0)
        {
            printf("WARNING: orphan or dirty inode leaves here!! inode number : %d, type : %hu\n", inum, inode.type);
            continue;
        }

        if (fs_ref_count != map_ref_count)
        {
            exit_on_failure("ERROR: bad reference count for file.");
        }

        // normal directory, more than 1 link is invalid
        if (inum != 1 && inode.nlink > 1)
        {
            exit_on_failure("ERROR: directory appears more than once in file system.");
        }
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
