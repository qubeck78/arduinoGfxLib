#ifndef _GFBITMAP_H
#define _GFBITMAP_H

#include "gfTypes.h"


#define GF_BITMAP_FLAG_TRANSPARENT  1


typedef struct _tgfBitmap
{

  ushort   width;
  ushort   height;

  ushort   flags;
  ushort   transparentColor;

  void    *buffer;

}tgfBitmap;



ulong gfLoadBitmapFS( tgfBitmap *dest, char *fileName );

ulong gfBlitBitmap( tgfBitmap *dest, tgfBitmap *src, short x, short y );
ulong gfBlitBitmapA( tgfBitmap *dest, tgfBitmap *src, short x, short y, uchar alpha );

ulong gfBlitBitmapRect( tgfBitmap *dest, tgfBitmap *src, short xd, short yd, short xs, short ys, short ws, short hs );



#endif
