TARGETS = makefs

all : $(TARGETS)

#
# change the -O7 to -O3 if your compiler doesn't grok -O7
#
CFLAGS = -O3

SUPPORT_OBJS = io.c
MISC_OBJS    = 

FS_OBJS = bitmap.c superblock.c bitvector.c inode.c

makefs : makefs.o $(FS_OBJS) $(SUPPORT_OBJS) $(MISC_OBJS)
	cc -o $@ makefs.o $(FS_OBJS) $(SUPPORT_OBJS) $(MISC_OBJS)

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f *.o $(TARGETS)
