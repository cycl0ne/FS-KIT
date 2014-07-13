#include "filesystem.h"

int32_t pfs_InitSuperBlock(pGD gd)
{
	gd->sb.id	= SUPE_ID;
	gd->sb.id2	= RBLO_ID;
	gd->sb.id3	= CKXX_ID;
	
	gd->sb.ownblock	= 1;
	gd->sb.block_size	= 512;
	gd->sb.block_shift	= 9;
	gd->sb.num_blocks	= GetNumDeviceBlocks(gd);
	gd->sb.used_blocks	= 2;
	pfs_WriteSuperBlock(gd);
	return 0;
}

int32_t pfs_WriteSuperBlock(pGD gd)
{
	RawWrite(gd, 1, &gd->sb, 1);
	return 0;
}

int32_t pfs_ReadSuperBlock(pGD gd)
{
	RawRead(gd, 1, &gd->sb, 1);
	return 0;
}
