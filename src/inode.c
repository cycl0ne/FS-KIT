#include <stdio.h>
#include "filesystem.h"

int32_t pfs_create_inodes(pGD gd)
{
	int32_t		blocksize = gd->sb.block_size;
	uint64_t	map_start;
	uint64_t	inodes_start;
	uint64_t	num_inodes;
	uint64_t	num_map_blocks;
	uint64_t	num_inode_blocks;
	uint64_t	i;
	void*		block;
	
	num_inodes		 = gd->sb.num_blocks >> 2;
	num_inode_blocks = num_inodes / (8*(blocksize/512)-1);  // 7 Inode + 1 Hdr = 512bytes;
	num_map_blocks	 = (num_inode_blocks / 8 / blocksize) + 1;
	
    if (pre_allocate_blocks(gd, &num_map_blocks, &map_start) != 0) 
    {
        printf("failed to allocate inode map blocks\n");
        return ENOSPC;
    }

    if (map_start != gd->bbm.num_bitmap_blocks + 1)
        printf("that's weird... map_start == %d, not %d\n", map_start, myfs->bbm.num_bitmap_blocks);


    if (pre_allocate_blocks(gd, &num_inode_blocks, &inodes_start) != 0) 
    {
        printf("failed to allocate inode data blocks\n");
        return ENOSPC;
    }
    
    if (inodes_start != map_start+num_map_blocks)
        printf("that's weird inode_start == %d not %d\n", inodes_start, map_start + num_map_blocks);
    
//
//------------------------------
//    
    block = get_tmp_blocks(myfs, 1);
    memset(block, 0, bsize);
    
    for(i=0; i < num_map_blocks; i++) 
    {
        if (write_blocks(myfs, map_start+i, block, 1) != 1) 
        {
            printf("error writing inode map block %ld\n", map_start+i);
            break;
        }
    }

    for(i=0; i < num_inode_blocks; i++) 
    {
        if (write_blocks(myfs, inodes_start+i, block, 1) != 1) 
        {
            printf("error writing inode data block %ld\n", inodes_start+i);
            break;
        }
    }

    free_tmp_blocks(myfs, block, 1);

    myfs->dsb.num_inodes           = num_inodes;
    myfs->dsb.inodes_start         = inodes_start;
    myfs->dsb.num_inode_blocks     = num_inode_blocks;
    myfs->dsb.inode_map_start      = map_start;
    myfs->dsb.num_inode_map_blocks = num_map_blocks;

    myfs->inode_map.bits    = calloc(1, num_map_blocks * bsize);
    myfs->inode_map.numbits = num_inodes;
    if (myfs->inode_map.bits == NULL) {
        printf("couldn't allocate space for the in memory inode map\n");
        return ENOMEM;
    }

    /* allocate inode 0 so no one else gets (to enable better error checks) */
    GetFreeRangeOfBits(&myfs->inode_map, 1, NULL);
    write_blocks(myfs, map_start, myfs->inode_map.bits, 1);

    return 0;
}
