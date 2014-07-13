#include "filesystem.h"

//read_blocks(myfs_info *myfs, fs_off_t block_num, void *block, size_t nblocks)

int32_t pfs_New_Bitmap_Block(pGD gd, uint64_t blck)
{
	uint32_t	i;

	pfs_bitmap	*bmblck = AllocVec(gd->sb.block_size, 0);//MEMF_FAST|MEMF_CLEAR);
	if (bmblck)
	{
		printf("Create BM @Block: %d\n", (int)blck);
		bmblck->id		 	 = BITMAP_ID;
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

int32_t pfs_Get_Bitmap_Block(pGD gd, uint64_t blck, uint8_t *buffer, uint8_t **start)
{
	// Buffer has to be at least block_size
	/*
	** Each pair = 512 (blocksize), effectivly we can only store 496 bytes per 512Block (1008 on 1024blocks...)
	** Header is 16bytes, rest is data
	**  
	** [Header16][Header16][Header16][Header16]
	** [ BITMAP ][ BITMAP ][ BITMAP ][ BITMAP ]
	** 
	** to calculate the exact position of a bit in the bitmap we have to take the Header16 into account
	*/
/*
	uint32_t bsize = gd->sb.block_size;
	
	read_blocks(gd, blck, buffer, 1);
	start = buffer+16;
*/
	return 0;
}

int32_t pfs_CreateStorageBitmap(pGD gd)
{
    char      *buff = NULL;
    int        ret, i, n, bsize;
    ssize_t    num_bytes, num_blocks;
    ssize_t    amt;
//    BitVector *bv;

    bsize = gd->sb.block_size;

	// first we divide /8 bits to get the number of bytes needed for the harddisk bitmap
    n  = gd->sb.num_blocks / 8;
	//count how many bytes we need additional for the header and add it to our result
    n = (((n / bsize)+1) * 16) + n;
	// round up for next blocksize
    n  = ((n + bsize - 1) & ~(bsize - 1));
	// now divide the bytes throught the blocksize to get the number of blocks needed!
    n /= bsize;

	printf("number of bitmap_blocks: %d (num: %x)\n", n, (int)gd->sb.num_blocks);
	gd->sb.bitmap_start = 2;

	for (i = gd->sb.bitmap_start; i< n+gd->sb.bitmap_start; i++) pfs_New_Bitmap_Block(gd, i);
	gd->sb.num_bitmap_blocks = n;
//	printf("used_blocks: %d\n", gd->sb.used_blocks);
    gd->sb.used_blocks += n;
	gd->bitsperbitmap = (bsize-16)*8;
//	printf("used_blocks: %d\n", gd->sb.used_blocks);
	pfs_WriteSuperBlock(gd);
	return 0;
}

