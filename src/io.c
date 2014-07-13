#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include "filesystem.h"

int32_t OpenDevice(pGD gd)
{
	uint8_t device[] = "filesystem.bin";

	gd->fd = open(device, O_RDWR);
	if (gd->fd < 0) 
	{
		printf("can't open device %s\n", device);
		return -1;
	}	
	return 0;
}

int32_t CloseDevice(pGD gd)
{
	close(gd->fd);
}

int32_t GetDevBlockSize(pGD gd)
{
	return 512;
}

int32_t GetNumDeviceBlocks(pGD gd)
{
	struct stat st;
	fstat(gd->fd, &st);
	return st.st_size / GetDevBlockSize(gd);
}

int32_t RawRead(pGD gd, uint64_t _pos, void *data, uint64_t blocks)
{
	int		fd = gd->fd;
	int		nbytes = blocks<<9;
	off_t 	pos = (off_t)_pos <<9;
//	printf("pos: %d, _pos: %d\n", pos, _pos);
	int32_t ret;
	
	if (lseek(fd, pos, SEEK_SET) < 0)
	{
		printf("Error Seek\n");
		return -1;
	}
	ret = read(fd, data, nbytes);

//	printf("read_pos: %d, got %d\n", nbytes, ret);
	
	if (ret != nbytes)
	{
		printf("read_pos: wanted %d, got %d\n", nbytes, ret);
		return -1;
	}
	return ret;
}

int32_t RawWrite(pGD gd, uint64_t _pos, void *data, uint64_t blocks)
{
	int		fd = gd->fd;
	int		nbytes = blocks<<9;
	off_t 	pos = (off_t)_pos <<9;
//	printf("pos: %d, _pos: %d\n", pos, _pos);
	int32_t ret;
	
	if (lseek(fd, pos, SEEK_SET) < 0)
	{
		printf("Error Seek\n");
		return -1;
	}
	ret = write(fd, data, nbytes);

//	printf("write_pos: %d, got %d\n", pos, nbytes);
	
	if (ret != nbytes)
	{
		printf("write_pos: wanted %d, got %d\n", nbytes, ret);
		return -1;
	}
	return ret;
}
