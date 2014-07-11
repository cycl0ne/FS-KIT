
typedef int chunk;
#define BITS_IN_CHUNK   (sizeof(chunk)*CHAR_BIT)


typedef struct BitVector
{
  int    numbits;         /* total number of bits in "bits" */
  int    next_free;       /* index of the next free bit */
  int    is_full;
  chunk *bits;            /* the actual bitmap */
} BitVector;

