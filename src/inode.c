#include <stdio.h>
#include "filesystem.h"

static int32_t pfs_NewInodeBitmap(pGD gd, uint64_t blck)
{
	uint32_t	i;

	pfs_bitmap	*bmblck = AllocVec(gd->sb.block_size, 0);//MEMF_FAST|MEMF_CLEAR);
	if (bmblck)
	{
		printf("Create INode BMP @Block: %d\n", (int)blck);
		bmblck->id		 	 = INODEBITMAP_ID;
		bmblck->ownblock 	 = blck;
		uint32_t longsperbmp = (gd->sb.block_size-16)>>2;
		uint32_t *bitmap	 = bmblck->bitmap;

		for (i = 0; i < longsperbmp; i++) 
		{
			//printf("%d/",i);
			*bitmap++ = 0;
		}
		RawWrite(gd, blck, bmblck, gd->sb.block_size>>9);
		FreeVec(bmblck);
		return 0;
	}
	return -1;
}

static int32_t	pfs_NewInodeBlock(pGD gd, uint64_t block)
{
	uint32_t	i;
	uint32_t	j;
	pfs_inodeheader *head;
	pfs_inode		*inode;
	
	uint8_t	*blck = AllocVec(gd->sb.block_size, 0);//MEMF_FAST|MEMF_CLEAR);
	if (blck)
	{
		head = (pfs_inodeheader *) blck;
		head->id		= INODEHD_ID;
		head->checksum	= 0;
		head->ownblock	= block;
		
		inode = (pfs_inode *) (blck + sizeof(pfs_inodeheader));
		j = (gd->sb.block_size - sizeof(pfs_inodeheader))/sizeof(pfs_inode);
		for (i = 0; i< j; i++)
		{
//			printf("Creating Inode #%d\n", (int)i);
			inode[i].pad1 = INODE_ID;
		}
		RawWrite(gd, block, blck, gd->sb.block_size>>9);
		FreeVec(blck);
	}
	return 0;
}

int32_t pfs_CreateInodes(pGD gd)
{
	uint64_t	map_start;
	uint64_t	inodes_start;
	uint64_t	num_inodes;
	uint64_t	num_map_blocks;
	uint64_t	num_inode_blocks;
	uint64_t	i;

	int32_t		blocksize = gd->sb.block_size;

	// 1 Inode for 4 Blocks
	num_inodes		 = gd->sb.num_blocks >> 2;
	
	num_inode_blocks = (blocksize-sizeof(pfs_inodeheader))/sizeof(pfs_inode);
	num_inode_blocks = num_inodes / num_inode_blocks + 1; 
	num_map_blocks	 = (num_inode_blocks / 8 / (blocksize-16)) + 1;

    printf("num inodes %ld num_map_blocks %ld num_inode blocks %ld\n", num_inodes, num_map_blocks, num_inode_blocks);

	// todo: PReallocate Inodes in bitmap
	// todo: PReallocate InodesMap in bitmap

	map_start	 = gd->sb.bitmap_start + gd->sb.num_bitmap_blocks;
	inodes_start = map_start + num_map_blocks;

	gd->sb.inode_map_start		= map_start;
	gd->sb.num_inode_map_blocks = num_map_blocks;
	gd->sb.inodes_start			= inodes_start;
	gd->sb.num_inode_blocks		= num_inode_blocks;
	
	printf("create @%d inode bitmap\n", (int)map_start);
	while(num_map_blocks--) pfs_NewInodeBitmap(gd, map_start++);
	printf("create @%d inode blocks\n", (int)inodes_start);
	while(num_inode_blocks--) pfs_NewInodeBlock(gd, inodes_start++); 
	printf("End @%d inode blocks\n", (int)inodes_start);

    gd->sb.used_blocks += gd->sb.num_inode_blocks;
    gd->sb.used_blocks += gd->sb.num_inode_map_blocks;
	printf("used blocks: %d\n", (int)gd->sb.used_blocks);
	pfs_WriteSuperBlock(gd);
	return 0;
}
