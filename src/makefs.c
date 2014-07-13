#include <stdio.h>
#include <limits.h>
#include "filesystem.h"

typedef int chunk;
#define BITS_IN_CHUNK   (sizeof(chunk)*CHAR_BIT)

typedef struct rootblock
{
    int32_t disktype;
    uint32_t options;          /* bit 0 is harddisk mode           */
    uint32_t datestamp;        /* current datestamp */
    uint16_t creationday;      /* days since Jan. 1, 1978 (like ADOS; INT16 instead of LONG) */
    uint16_t creationminute;   /* minutes past modnight            */
    uint16_t creationtick;     /* ticks past minute                */
    uint16_t protection;       /* protection bits (ala ADOS)       */
    uint8_t diskname[32];     /* disk label (pascal string)       */
    uint32_t lastreserved;     /* reserved area. blocknumbers      */
    uint32_t firstreserved;
    uint32_t reserved_free;    /* number of reserved blocks (blksize blocks) free  */
    uint16_t blksize;          /* size of reserved blocks in bytes */
    uint16_t rblkcluster;      /* number of blocks in rootblock, including bitmap  */
    uint32_t blocksfree;       /* blocks free                      */
    uint32_t alwaysfree;       /* minimum number of blocks free    */
    uint32_t roving_ptr;       /* current int32_t bitmapfield nr for allocation       */
    uint32_t deldir;           /* deldir location (<= 17.8)        */
    uint32_t disksize;         /* disksize in sectors              */
    uint32_t extension;        /* rootblock extension (16.4)       */
    uint32_t not_used;
    union
    {
        uint16_t anodeblocks[208];         /* SMALL: 200*84 = 16800 anodes */
        struct
        {
            uint32_t bitmapindex[5];       /* 5 bitmap indexblocks with 253 bitmap blocks each */
            uint32_t indexblocks[99];      /* 99 index blocks with 253 anode blocks each       */
        } small;
        struct 
        {
            uint32_t bitmapindex[104];		/* 104 bitmap indexblocks = max 104 G */
        } large;
    } idx;
} rootblock_t;


void main(void)
{
	int bsize = 512;
	int n = 1000;
	printf("Size of SuperBlock: %d\n", (int)sizeof(pfs_superblock));
	printf("Size of : %d\n", (int)sizeof(pfs_inodeheader));	
	printf("Size of : %d\n", (int)sizeof(pfs_inode));	
	printf("---%d---\n", (int) (((n / bsize)+1) * 16) + n);
	printf("bits in chunk: %d\n", (int)BITS_IN_CHUNK);
	if (sizeof(pfs_superblock) != 512)
	{
		printf("Superblock not 512bytes\n");
		return;
	}
	
	pGD gd = AllocVec(sizeof(GD), 0);
	
	if (gd) 
	{
		memset(gd, 0, sizeof(GD));
		OpenDevice(gd);
		pfs_InitSuperBlock(gd);
		pfs_CreateStorageBitmap(gd);
		pfs_CreateInodes(gd);
		CloseDevice(gd);
		FreeVec(gd);
	}
	return;
}
