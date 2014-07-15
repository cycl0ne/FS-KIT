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
    int        i, n, bsize;

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

int32_t pfs_AllocateBlock(pGD gd, uint64_t *num_blocks, uint64_t *start_addr, BOOL do_log_write, int32_t exact)
{
	int32_t		i, n, len, bsize = gd->sb.block_size, nblocks;
	int32_t		biggest_free_chunk = 0, max_free_chunk = 1;
	uint64_t	start = -1;
	char      *ptr;
//	BitVector *bv;

	if (*num_blocks <= 0) return -1;
	do_log_write = FALSE;
	if (*num_blocks > (gd->sb.num_blocks - gd->sb.used_blocks)) return -1; //ENOSPC;

    for(nblocks = *num_blocks; nblocks >= 1; nblocks /= 2) 
    {
        bv = myfs->bbm.bv;
        start = GetFreeRangeOfBits(bv, nblocks, &biggest_free_chunk);
        if (start != -1) break;
 
        if (biggest_free_chunk > max_free_chunk) max_free_chunk  = biggest_free_chunk;

		if (exact == LOOSE_ALLOCATION && biggest_free_chunk > 0 && biggest_free_chunk >= (nblocks>>4)) 
		{
			nblocks = biggest_free_chunk;
			start = GetFreeRangeOfBits(bv, nblocks, NULL);
			if (start != -1) break;
		}

		if (start != -1 || exact == EXACT_ALLOCATION) break;

		if (start == -1 && exact == TRY_HARD_ALLOCATION && max_free_chunk > 1) 
		{
			if (max_free_chunk < nblocks) nblocks = max_free_chunk;

			start = GetFreeRangeOfBits(bv, nblocks, &biggest_free_chunk);
			if (start != -1) 
			{
				i  = 0;
				break;
			}
		}

        if (exact == LOOSE_ALLOCATION && nblocks > max_free_chunk*2) 
        {
            nblocks = max_free_chunk * 2;  /* times 2 because of the loop continuation */
        }

        max_free_chunk = 1;
    }

	if (start == -1) return -1; //ENOSPC;
    
    *start_addr = (uint64_t)start;
    *num_blocks = nblocks;

    /*
       calculate the block number of the bitmap block we just modified.
       the +bitmap_start accounts for the super block.
    */
    n   = (start / 8 / bsize) + gd->sb.bitmap_start;
    len = (nblocks / 8 / bsize) + gd->sb.bitmap_start;
    
    
    ptr = (char *)bv->bits + (((start / 8) / bsize) * bsize);

#if 0
    if (do_log_write)  {
        for(i=0; i < len; i++, ptr += bsize) {
            if (myfs_write_journal_entry(myfs, myfs->cur_je, n+i, ptr) != 1) {
                printf("error:1 failed to write bitmap block run %d:1!\n",n+i);
                release_sem(myfs->bbm_sem);
                return EINVAL;
            }
        }
    } else if (write_blocks(myfs, n, ptr, len) != len) {
        printf("error: 2 failed to write back bitmap block @ block %d!\n", n);
        release_sem(myfs->bbm_sem);
        return EINVAL;
    }
#endif
	if (RawWrite(gd, n, ptr, len) != len)
	{
		printf("error: 2 failed to write back bitmap block @ block %d!\n", n);
		return -1;
	}

    gd->sb.used_blocks += *num_blocks;
    gd->sb.flags = MYFS_DIRTY;

    return 0;


}
