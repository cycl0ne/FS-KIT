#include "filesystem.h"
#include <limits.h>

typedef int32_t chunk;
#define BITS_IN_CHUNK   (sizeof(chunk)*8)


int32_t pfs_SetBit(pGD gd, int32_t *bits, int32_t which)
{
	int32_t i,j;
	if (which >= gd->bitsperbitmap || which < 0) return FALSE;
	i = which / BITS_IN_CHUNK;
	j = which % BITS_IN_CHUNK;
	bits[i] |= (1 << j);
	return TRUE;
}

int32_t pfs_UnSetBit(pGD gd, int32_t *bits, int32_t which)
{
	int32_t i,j;
	if (which >= gd->bitsperbitmap || which < 0) return FALSE;
	i = which / BITS_IN_CHUNK;
	j = which % BITS_IN_CHUNK;

	bits[i]	&= ~(1 << j);
	return TRUE;
}

int32_t pfs_IsSetBit(pGD gd, int32_t *bits, int32_t which)
{
	int32_t i,j;
	if (which >= gd->bitsperbitmap || which < 0) return FALSE;
	i = which / BITS_IN_CHUNK;
	j = which % BITS_IN_CHUNK;

	return (int32_t)(bits[i] & (1 << j));
}

