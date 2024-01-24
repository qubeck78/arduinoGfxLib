#ifndef _OSALLOC_H
#define _OSALLOC_H

#include "gfTypes.h"

#define _OS_ALLOC_BLOCK_SIZE    256

#define _OS_ALLOC_BLOCK_FREE     0x00

#define _OS_ALLOC_LAST_BLOCK_TAKEN  0x80
#define _OS_ALLOC_BLOCK_TAKEN     0x81

typedef struct _tosAllocPrv
{
  uchar   *blockBitmap;
  ulong    blockBitmapSize;

  void    *allocStart;
  void    *allocEnd;

  ulong    totalBlocksNum;


}tosAllocPrv;

ulong  osAllocInit( void *allocStart, ulong memorySize );
void  *osAlloc( ulong size );
ulong    osFree( void *allocMemPtr );



#endif
