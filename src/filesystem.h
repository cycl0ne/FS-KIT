#ifndef filesystem_h
#define filesystem_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define AllocVec(a, b) malloc(a)
#define FreeVec(a) free(a)
#define TRUE 1
#define FALSE 0

typedef	unsigned long long	uint64_t;
typedef unsigned int		uint32_t;
typedef unsigned short		uint16_t;
typedef unsigned char		uint8_t;

#if 0
typedef	long long			int64_t;
typedef	unsigned long long	uint64_t;
typedef int					int32_t;
typedef unsigned int		uint32_t;
typedef short				int16_t;
typedef unsigned short		uint16_t;
typedef char				int8_t;
typedef unsigned char		uint8_t;
#endif

#define MAKE_ID(a,b,c,d) ( (int32_t)(d)<<24 | (int32_t)(c)<<16 | (b)<<8 | (a) )

//64 bytes struct
#define INODEHD_ID	MAKE_ID('I','N','H','D')
typedef struct pfs_inodeheader
{
	int32_t			id;
	int32_t			checksum;
	int64_t			ownblock;
//16
	int32_t			pad[12];
} __attribute__((packed)) pfs_inodeheader;


// 64bytes struct.
#define INODE_ID	MAKE_ID('I','N','D','E')
typedef struct pfs_inode
{
	int32_t			number;
	int32_t			uid;
	int32_t			gid;
	int32_t			mode;
	int32_t			flags;
	int32_t			pad1;
//24
	int64_t			createtime;
	int64_t			lastmodifiedtime;
	int64_t			pad2;
//24
	union {
		struct {
			int64_t		size;
			int64_t		datablk;
		}file;
		struct {
			int64_t		hashtable;
			int64_t		dirblk;
		} dir;
	} object;
//16
} __attribute__((packed)) pfs_inode;

#define BITMAP_ID	MAKE_ID('B','T','M','P')
#define INODEBITMAP_ID	MAKE_ID('I','B','M','P')
typedef struct pfs_bitmap
{
	int32_t			id;
	int32_t			checksum;
	int64_t			ownblock;
	int32_t			bitmap[0];
} __attribute__((packed)) pfs_bitmap;

#define DATA_ID	MAKE_ID('D','A','T','A')
typedef struct pfs_data
{
	int32_t			id;
	int32_t			checksum;
	int64_t			ownblock;
	int64_t			nextblock;
	int32_t			data[0];
} __attribute__((packed)) pfs_data;


typedef struct pfs_direntry
{
	uint32_t	inum;
	uint16_t	name_len;
	int8_t		name[1];
} pfs_direntry;


// Superblock NEEDS TO BE <= 512bytes in size! Keep this in mind when changing/adding!
#define SUPE_ID	MAKE_ID('S','U','P','E')
#define RBLO_ID	MAKE_ID('R','B','L','O')
#define CKXX_ID	MAKE_ID('C','K','X','X')
#define MAX_VOLUME_NAME 64
typedef struct pfs_superblock
{
	int32_t			id;		// "supe"
	int32_t			checksum;
	uint64_t		ownblock;
	uint8_t			name[MAX_VOLUME_NAME]; //16+32
	uint32_t		block_size;
	uint32_t		block_shift; //8
	uint64_t		num_blocks;
	uint64_t		used_blocks;
	uint64_t		bitmap_start;
	uint64_t		num_bitmap_blocks;
	uint64_t		inode_map_start;
	uint64_t		num_inode_map_blocks;
	uint64_t		inodes_start;
	uint64_t		num_inode_blocks; //48
	uint32_t		id2;	// "rblo"
	uint32_t		flags;
	uint64_t		reserved[4];
	uint32_t		root_inum;
	uint32_t		id3; 	//"ckxx"
	uint32_t		pad[78]; // if you alter the values ontop (adding .. please change this.. to keep struct size at 512!)
}__attribute__((packed)) pfs_superblock;


typedef struct GlobalData
{
	int				fd;
	pfs_superblock	sb;
//	uint64_t		num_bitmap_blocks;
	uint32_t		bitsperbitmap;
	pfs_inode		*root;
}GD, *pGD;

//---------------------------------------------
int32_t pfs_CreateStorageBitmap(pGD gd);
int32_t pfs_WriteSuperBlock(pGD gd);
int32_t pfs_CreateInodes(pGD gd);


int32_t RawWrite(pGD gd, uint64_t _pos, void *data, uint64_t blocks);
int32_t RawRead(pGD gd, uint64_t _pos, void *data, uint64_t blocks);
int32_t GetNumDeviceBlocks(pGD gd);
int32_t GetDevBlockSize(pGD gd);
int32_t CloseDevice(pGD gd);
int32_t OpenDevice(pGD gd);

#endif
