#include "gfBitmap.h"
#include "gfDrawing.h"
#include "osAlloc.h"
#include "osFile.h"




ulong gfLoadBitmapFS ( tgfBitmap *dest, char *fileName )
{
  tosFile file;
  uchar buf[4];
  int   i;

  if( !dest )
  {
    return 1;
  }

  if (! osFOpen( &file, fileName, OS_FILE_READ ) )
  {

    osFRead( &file, buf, 2, NULL );

    dest->width = buf[0];
    dest->width |= ( buf[1] << 8 );

    osFRead( &file, buf, 2, NULL );

    dest->height = buf[0];
    dest->height |= ( buf[1] << 8 );

    osFRead( &file, buf, 2, NULL );

    dest->flags = buf[0];
    dest->flags |= ( buf[1] << 8 );

    osFRead( &file, buf, 2, NULL );

    dest->transparentColor = buf[0];
    dest->transparentColor |= ( buf[1] << 8 );

    osFRead( &file, buf, 1, NULL );

    if( buf[0] != 12 )
    {

      osFClose( &file );
      return 2;
    }


    osFRead( &file, buf, 3, NULL );

    for( i = 0; i < 3 ; i++ )
    {
      if( buf[i] != 0 )
      {
        osFClose( &file );
        return 2;

      }
    }

    dest->buffer = osAlloc( dest->width * dest->height * 2 );

    if( dest->buffer == NULL )
    {
      osFClose( &file );
      return 3;

    }

    osFRead( &file, (uchar*)dest->buffer, (ulong)( dest->width * dest->height * 2 ), NULL );

    osFClose( &file );
  }

  return 0;
}


ulong gfBlitBitmap( tgfBitmap *dest, tgfBitmap *src, short x, short y )
{
  short cx,cy,bw,bh,sx,sy;

  ushort *fbSrc;
  ushort *fbDest;
  ushort srcColor;



  if(( dest == NULL) || ( src == NULL ))
  {
    return 1;
  }

  bw = src->width;
  bh = src->height;
  sx = 0;
  sy = 0;


  //clip src bitmap if coordinates are negative
  if( x < 0 )
  {
    sx = -x;
    bw -= sx;
    x = 0;
  }

  if( y < 0 )
  {
    sy = -y;
    bh -= sy;
    y = 0;
  }


  //clip src width if exceeds sise of dest bitmap
  if( ( bw + x ) >= dest->width )
  {
    bw = dest->width - x;
  }

  if( bw <= 0 )
  {
    return 0;
  }

  if( ( bh + y ) >= dest->height )
  {
    bh = dest->height - y;
  }

  if( bh <= 0 )
  {
    return 0;
  }

  if( src->flags & GF_BITMAP_FLAG_TRANSPARENT )
  {

    for( cy = 0; cy < bh; cy++ )
    {
      fbSrc = &(( ushort* )src->buffer)[ ( ( cy + sy ) * src->width ) + sx ];
      fbDest = &(( ushort* )dest->buffer)[ ( ( cy + y ) * dest->width ) + x ];

      for( cx = 0; cx < bw; cx++ )
      {
        srcColor = *fbSrc++;

        if( srcColor != src->transparentColor )
        {
          *fbDest++ = srcColor;
        }else{
          fbDest++;
                }

      }
    }

  }else{

      for( cy = 0; cy < bh; cy++ )
    {
      fbSrc = &(( ushort* )src->buffer)[ ( ( cy + sy ) * src->width ) + sx ];
      fbDest = &(( ushort* )dest->buffer)[ ( ( cy + y ) * dest->width ) + x ];

      for( cx = 0; cx < bw; cx++ )
      {
        *fbDest++ = *fbSrc++;
      }
    }

  }

    return 0;
}

ulong gfBlitBitmapA( tgfBitmap *dest, tgfBitmap *src, short x, short y, uchar alpha )
{
  short cx,cy,bw,bh,sx,sy;

  ushort *fbSrc;
  ushort srcColor;


  if(( dest == NULL) || ( src == NULL ))
  {
    return 1;
  }

  bw = src->width;
  bh = src->height;
  sx = 0;
  sy = 0;


  //clip src bitmap if coordinates are negative
  if( x < 0 )
  {
    sx = -x;
    bw -= sx;
    x = 0;
  }

  if( y < 0 )
  {
    sy = -y;
    bh -= sy;
    y = 0;
  }


  //clip src width if exceeds sise of dest bitmap
  if( ( bw + x ) >= dest->width )
  {
    bw = dest->width - x;
  }

  if( bw <= 0 )
  {
    return 0;
  }

  if( ( bh + y ) >= dest->height )
  {
    bh = dest->height - y;
  }

  if( bh <= 0 )
  {
    return 0;
  }

  if( src->flags & GF_BITMAP_FLAG_TRANSPARENT )
  {

    for( cy = 0; cy < bh; cy++ )
    {
      fbSrc = &(( ushort* )src->buffer)[ ( ( cy + sy ) * src->width ) + sx ];

      for( cx = 0; cx < bw; cx++ )
      {
        srcColor = *fbSrc++;

        if( srcColor != src->transparentColor )
        {
          gfPlotA( dest, cx + x, cy + y, srcColor, alpha );
        }

      }
    }

  }else{

    for( cy = 0; cy < bh; cy++ )
    {
      fbSrc = &(( ushort* )src->buffer)[ ( ( cy + sy ) * src->width ) + sx ];

      for( cx = 0; cx < bw; cx++ )
      {
        gfPlotA( dest, cx + x, cy + y, *fbSrc++, alpha );
      }
    }

  }

  return 0;
}

ulong gfBlitBitmapRect( tgfBitmap *dest, tgfBitmap *src, short xd, short yd, short xs, short ys, short ws, short hs )
{

  short cx,cy,bw,bh,sx,sy;

  ushort *fbSrc;
  ushort *fbDest;
  ushort srcColor;



  if(( dest == NULL) || ( src == NULL ))
  {
    return 1;
  }



  bw = ws;  //src->width;
  bh = hs;  //src->height;

  sx = xs;
  sy = ys;


  //clip src bitmap if coordinates are negative
  if( xd < 0 )
  {
    sx = -xd;
    bw -= sx;
    xd = 0;
  }

  if( yd < 0 )
  {
    sy = -yd;
    bh -= sy;
    yd = 0;
  }


  //clip src width if exceeds sise of dest bitmap
  if( ( bw + xd ) >= dest->width )
  {
    bw = dest->width - xd;
  }

  if( bw <= 0 )
  {
    return 0;
  }

  if( ( bh + yd ) >= dest->height )
  {
    bh = dest->height - yd;
  }

  if( bh <= 0 )
  {
    return 0;
  }

  if( src->flags & GF_BITMAP_FLAG_TRANSPARENT )
  {

    for( cy = 0; cy < bh; cy++ )
    {
      fbSrc = &(( ushort* )src->buffer)[ ( ( cy + sy ) * src->width ) + sx ];
      fbDest = &(( ushort* )dest->buffer)[ ( ( cy + yd ) * dest->width ) + xd ];

      for( cx = 0; cx < bw; cx++ )
      {
        srcColor = *fbSrc++;

        if( srcColor != src->transparentColor )
        {
          *fbDest++ = srcColor;
        }else{
          fbDest++;
        }

      }
    }

  }else{

    for( cy = 0; cy < bh; cy++ )
    {
      fbSrc = &(( ushort* )src->buffer)[ ( ( cy + sy ) * src->width ) + sx ];
      fbDest = &(( ushort* )dest->buffer)[ ( ( cy + yd ) * dest->width ) + xd ];

      for( cx = 0; cx < bw; cx++ )
      {
        *fbDest++ = *fbSrc++;
      }
    }

  }

  return 0;
}
