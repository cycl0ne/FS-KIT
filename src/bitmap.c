#include "filesystem.h"

//read_blocks(myfs_info *myfs, fs_off_t block_num, void *block, size_t nblocks)

int32_t pfs_New_Bitmap_Block(pGd gd, uint64_t blck)
{
	pfs_bitmap	*bmblck = malloc(gd->sb.block_size);
	
}

int32_t pfs_Get_Bitmap_Block(pGd gd, uint64_t blck, uint8_t *buffer, uint8_t **start)
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
	uint32_t bsize = gd->sb.block_size;
	
	read_blocks(gd, blck, buffer, 1);
	start = buffer+16;
	return 0;
}

int32_t pfs_CreateStorageBitmap(pGD gd)
{
    char      *buff = NULL;
    int        ret, i, n, bsize;
    ssize_t    num_bytes, num_blocks;
    ssize_t    amt;
    BitVector *bv;

    bsize = gd->sb.block_size;

	// first we divide /8 bits to get the number of bytes needed for the harddisk bitmap
    n  = gd->sb.num_blocks / 8;
	//count how many bytes we need additional for the header and add it to our result
    n = (((n / bsize)+1) * 16) + n;
	// round up for next blocksize
    n  = ((n + bsize - 1) & ~(bsize - 1));
	// now divide the bytes throught the blocksize to get the number of blocks needed!
    n /= bsize;

    gd->num_bitmap_blocks = n;
	num_blocks = gd->num_bitmap_blocks + 1;

    amt = write_blocks(gd, 1, buff, n);

    gd->sb.used_blocks += num_blocks;
	return 0;
#if 0
// below here is old code ---------------------------------------------


    gd->bv = (BitVector *)calloc(1, sizeof(BitVector));
    if (gd->bv == NULL) {
        ret = ENOMEM;
        goto err;
    }
    num_bytes = n * bsize;

    buff = (char *)calloc(1, num_bytes);
    if (buff == NULL) {
        ret = ENOMEM;
        goto err;
    }

    gd->bv->bits    = (chunk *)buff;
    gd->bv->numbits = gd->sb.num_blocks; 


    /* fill in used blocks, including the superblock */
    num_blocks = gd->num_bitmap_blocks + 1;
    if (GetFreeRangeOfBits(gd->bv, num_blocks, NULL) != 0) 
    {
        printf("*** failed to allocate %ld bits in create_bitmap\n", gd->bbm.num_bitmap_blocks + 1);
        ret = ENOSPC;
        goto err;
    }

    /* write out the bitmap blocks, starting at block # 1 */
    amt = write_blocks(gd, 1, buff, n);
    if (amt != n)  {
        ret = EINVAL;       
        goto err;
    }
        
    gd->sb.used_blocks += num_blocks;

    return 0;

 err:
    if (buff) free(buff);
    gd->bv = NULL;
    return ret;
#endif
}

