
#include "osAlloc.h"

tosAllocPrv  osAllocPrv;


ulong osAllocInit( void *allocStart, ulong memorySize )
{
  ulong i;

  //calculate number of memory blocks
  osAllocPrv.totalBlocksNum = ( memorySize / _OS_ALLOC_BLOCK_SIZE );

  //block bitmap goes first
  osAllocPrv.blockBitmap    = (uchar*)allocStart;


  //calculate block bitmap size
  osAllocPrv.blockBitmapSize  =  osAllocPrv.totalBlocksNum;

  //set alloc start
  osAllocPrv.allocStart       = (void*)( (uchar*)osAllocPrv.blockBitmap + osAllocPrv.blockBitmapSize );

  //align alloc start to block boundary
  i = (ulong)osAllocPrv.allocStart;

  if( i % _OS_ALLOC_BLOCK_SIZE )
  {
    i += _OS_ALLOC_BLOCK_SIZE;
  }

  i = i / _OS_ALLOC_BLOCK_SIZE;
  i = i * _OS_ALLOC_BLOCK_SIZE;

  osAllocPrv.allocStart = (void*)i;

  //calculate alloc end
  osAllocPrv.allocEnd = (void*)( (uchar*) allocStart + memorySize );


  //update total blocks num - now with bitmap allocated and memory aligned to block size
  osAllocPrv.totalBlocksNum = ( (uchar*)osAllocPrv.allocEnd - (uchar*)osAllocPrv.allocStart ) / _OS_ALLOC_BLOCK_SIZE;


  //update alloc end according to blocks num
  osAllocPrv.allocEnd = (void*)( (uchar*)osAllocPrv.allocStart + ( osAllocPrv.totalBlocksNum * _OS_ALLOC_BLOCK_SIZE ) );

  //mark all blocks as free in bitmap
  for( i = 0; i < osAllocPrv.totalBlocksNum; i++ )
  {
    osAllocPrv.blockBitmap[i] = _OS_ALLOC_BLOCK_FREE;
  }


  return 0;
}

void *osAlloc( ulong size )
{

  ulong  numBlocksToAlloc;
  long   freeBlockNum;
  ulong    freeBlocksInChunk;
  void    *rv;

  ulong    i;
  ulong    j;

  //cals amount blocks to allocate

  if( size % _OS_ALLOC_BLOCK_SIZE )
  {
    size += _OS_ALLOC_BLOCK_SIZE;
  }

  size = size / _OS_ALLOC_BLOCK_SIZE;

  numBlocksToAlloc = size;

  freeBlockNum = -1;

  for( i = 0; i < osAllocPrv.totalBlocksNum ; i++ )
  {


    if( freeBlockNum == -1 )
    {
      //we haven't found anything free yet

      if( osAllocPrv.blockBitmap[i] == _OS_ALLOC_BLOCK_FREE )
      {
        //we have a free block, store its number

        freeBlockNum = i;
      }
    }

    if( freeBlockNum != -1 )
    {

      //ok we've got a free block, now check if there is enough space in chunk

      if( osAllocPrv.blockBitmap[i] == _OS_ALLOC_BLOCK_FREE )
      {
        freeBlocksInChunk = ( i - freeBlockNum ) + 1;

        if( freeBlocksInChunk >= numBlocksToAlloc )
        {
          //we have it, now alloc memory


          for( j = freeBlockNum; j < i; j++ )
          {
            osAllocPrv.blockBitmap[j] = _OS_ALLOC_BLOCK_TAKEN;
          }
          osAllocPrv.blockBitmap[i] = _OS_ALLOC_LAST_BLOCK_TAKEN;

          //calc allocated memory address
          rv = (void*)( (uchar*)osAllocPrv.allocStart + ( freeBlockNum * _OS_ALLOC_BLOCK_SIZE ) );

          return rv;

        }

      }
      else
      {
        //chunk too small, forget start index

        freeBlockNum = -1;

      }

    }


  }


  //nothing found, return null
  return NULL;
}

ulong osFree( void *allocMemPtr )
{
  ulong blockNumToFree;
  ulong i;


  blockNumToFree = ( (uchar*)allocMemPtr - (uchar*)osAllocPrv.allocStart ) / _OS_ALLOC_BLOCK_SIZE;

  if( blockNumToFree >= osAllocPrv.totalBlocksNum )
  {
    //address outside alloc pool
    return 1;
  }

  for( i = blockNumToFree; i < osAllocPrv.totalBlocksNum; i++ )
  {
    if( osAllocPrv.blockBitmap[i] == _OS_ALLOC_BLOCK_FREE )
    {

      //trying to free free memory
      return 2;

    }else if( osAllocPrv.blockBitmap[i] == _OS_ALLOC_LAST_BLOCK_TAKEN )
    {
      //end of alloc chunk

      //mark as free
      osAllocPrv.blockBitmap[i] = _OS_ALLOC_BLOCK_FREE;

      //return
      return 0;
    }
    else
    {
      //mark as free and check next block
      osAllocPrv.blockBitmap[i] = _OS_ALLOC_BLOCK_FREE;
    }

  }


  return 0;
}
