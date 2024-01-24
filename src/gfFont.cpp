
#include "gfFont.h"
#include "gfBitmap.h"
#include "gfDrawing.h"
#include "osAlloc.h"
#include "osFile.h"


ulong gfLoadFontFS( tgfFont *dest, char *fileName )
{
  tosFile file;
  uchar buf[4];
  int   i;

  ushort  fontType;
  ulong charBufOffset;
  ulong charBufSize;

  if ( !osFOpen( &file, fileName, OS_FILE_READ ) )
  {
    osFRead( &file, (uchar*)buf, 2, NULL );

    fontType  = buf[0];
    fontType |= ( buf[1] << 8 );

    switch( fontType )
    {
      case  GF_FONT_TYPE_ALPHA4_BITMAP_VAR_WIDTH:

        dest->type = GF_FONT_TYPE_ALPHA4_BITMAP_VAR_WIDTH;


        osFRead( &file, (uchar*)buf, 2, NULL );

        dest->flags = buf[0];
        dest->flags |= ( buf[1] << 8 );

        osFRead( &file, (uchar*)buf, 2, NULL );

        dest->width = buf[0];
        dest->width |= ( buf[1] << 8 );

        osFRead( &file, (uchar*)buf, 2, NULL );

        dest->height = buf[0];
        dest->height |= ( buf[1] << 8 );

        osFRead( &file, (uchar*)buf, 2, NULL );

        dest->charColor = buf[0];
        dest->charColor |= ( buf[1] << 8 );

        osFRead( &file, (uchar*)buf, 2, NULL );

        dest->backgroundColor = buf[0];
        dest->backgroundColor |= ( buf[1] << 8 );

        osFRead( &file, (uchar*)buf, 2, NULL );

        dest->firstChar = buf[0];
        dest->lastChar  = buf[1];

        //char widths - has to be 22

        osFRead( &file, (uchar*)buf, 4, NULL );

        i  = buf[0];
        i |= ( buf[1] << 8 );
        i |= ( buf[2] << 16 );
        i |= ( buf[3] << 24 );

        if( i != 22)
        {

          osFClose( &file );
          return 2;
        }

        //char buffer offset
        osFRead( &file, (uchar*)buf, 4, NULL );

        charBufOffset  = buf[0];
        charBufOffset |= ( buf[1] << 8 );
        charBufOffset |= ( buf[2] << 16 );
        charBufOffset |= ( buf[3] << 24 );

        dest->charWidths = (uchar *)osAlloc( 256 );
        for( i = 0; i < 256 ; i++ )
        {
          ( ( uchar* ) dest->charWidths )[i] = 0;
        }

        charBufSize = 0;

        //char widths
        for( i = dest->firstChar ; i <= dest->lastChar; i ++ )
        {
          osFRead( &file, (uchar*)buf, 1, NULL );
          ( ( uchar* ) dest->charWidths )[i - dest->firstChar] = buf[0];

          charBufSize += ( ( uchar* ) dest->charWidths )[i - dest->firstChar];
        }

        charBufSize /= 2;   //2 pixels per byte
        charBufSize *= dest->height;

        dest->charBuffer = osAlloc( charBufSize );

        for( i = 0; i < charBufSize; i++ )
        {
          osFRead( &file, (uchar*)buf, 1, NULL );
          ( ( uchar* ) dest->charBuffer )[i] = buf[0];
        }

        osFClose( &file );

        return 0;

      break;


      default:

        osFClose( &file );

        return 2;
        break;
    }
  }

  return 0;

}


short gfPutChar( tgfBitmap *bmp, tgfFont *font, short x, short y, char c )
{
  short  cx;
  short    cy;
  int      i;
  ulong    letterDataIdx;

  uchar   *letterPtr;
  char   letterData;

  if( c < font->firstChar )
  {
    return 0;
  }
  if( c > font->lastChar )
  {
    return 0;
  }



  switch( font->type )
  {

    c -= font->firstChar;

    case GF_FONT_TYPE_MONO_BITMAP_FIXED_WIDTH:

      letterPtr = &((uchar *)font->charBuffer)[ c * font->height ];

      for( cy = 0; cy < font->height ; cy++ )
      {
        letterData = *letterPtr++;

        for( cx = 0; cx < font->width; cx++ )
        {
          if( letterData & 0x80 )
          {
            gfPlot( bmp, cx + x, cy + y, font->charColor );
          }else
          {
            if( ! ( font->flags & GF_FONT_FLAG_TRANSPARENT ) )
            {

              if( font->flags & GF_FONT_FLAG_ALPHA_BACKGROUND )
              {
                gfPlotA( bmp, cx + x, cy + y, font->backgroundColor, 128 );
              }
              else
              {
                gfPlot( bmp, cx + x, cy + y, font->backgroundColor );
              }
            }
          }

          letterData <<= 1;

        }
      }

      break;

    case GF_FONT_TYPE_ALPHA4_BITMAP_VAR_WIDTH:

      letterDataIdx = 0;
        c -= font->firstChar;

      for( i = 0; i < c; i++ )
      {
        cx =  ( ( uchar * )font->charWidths)[ i ];

        if( cx & 1 )
        {
          cx ++;
        }

                letterDataIdx += cx;
      }

      letterDataIdx /= 2;
      letterDataIdx *= font->height;

      for( cy = 0; cy < font->height; cy++ )
      {
        for( cx = 0; cx < ( ( uchar* )font->charWidths)[ c ] ; cx+=2 )
        {
          letterData = (( uchar *)font->charBuffer)[ letterDataIdx++ ];

          if( ! ( font->flags &  GF_FONT_FLAG_TRANSPARENT ) )
          {
            if( font->flags & GF_FONT_FLAG_ALPHA_BACKGROUND )
            {
              gfPlotA( bmp, cx + x, cy + y, font->backgroundColor, 128 );
              gfPlotA( bmp, cx + x + 1, cy + y, font->backgroundColor, 128 );
            }
            else
            {
              gfPlot( bmp, cx + x, cy + y, font->backgroundColor );

              gfPlot( bmp, cx + x + 1, cy + y, font->backgroundColor );
            }
          }

          if( letterData & 0xf0 )
          {
            gfPlotA( bmp, cx + x, cy + y, font->charColor, ( letterData & 0xf0 ) );
          }
          if( letterData & 0x0f )
          {
            gfPlotA( bmp, cx + x + 1, cy + y, font->charColor, ( letterData & 0x0f ) << 4 );
                    }
        }
      }


      return ( ( uchar* )font->charWidths)[ c ];

      break;


    default:

      return 0;

      break;

  }
  return font->width;
}

short gfPutString( tgfBitmap *bmp, tgfFont *font, short x, short y, char *string )
{
  short   stringWidth;
  short   sw;
  char    c;

  stringWidth = 0;

  while( ( c = *string++ ) )
  {
    sw = gfPutChar( bmp, font, x, y, c );

    x += sw;

    stringWidth += sw;

  }

  return stringWidth;
}


ulong gfDrawTextOverlay( tgfBitmap *bmp, tgfTextOverlay *textOverlay, short x, short y )
{
  short  cx;
  short  cy;

  uchar    letter;
  uchar    attributes;
  uchar    r;
  uchar    g;
  uchar    b;
  uchar    alphaBackground;

  uchar   *textDataPtr;
  tgfFont *font;

  font = textOverlay->font;


  font->flags = 0;    //clear flags

  for( cy = 0; cy < textOverlay->height; cy++ )
  {

    textDataPtr = &textOverlay->textBuffer[ cy * textOverlay->width * 2 ];

    for( cx = 0; cx < textOverlay->width; cx++ )
    {
      letter    = *textDataPtr++;
      attributes  = *textDataPtr++;


      //cursor
      if( textOverlay->flags & GF_TEXT_OVERLAY_FLAG_SHOW_CURSOR )
      {
        if( ( cx == textOverlay->cursX ) && ( cy == textOverlay->cursY ) )
        {
          attributes = 0xf0;

          if( letter == 0 )
          {
            //always display, even if no text
            letter = 32;
          }
        }
      }

      if( letter  != 0 )
      {

        //letter color

        r = 0;
        g = 0;
        b = 0;

        if( attributes & 8 )
        {
          //bright
          if( attributes & 1 )
          {
            r = 0xff;
          }

          if( attributes & 2 )
          {
            g = 0xff;
          }

          if( attributes & 4 )
          {
            b = 0xff;
          }

        }
        else
        {
          //dark
          if( attributes & 1 )
          {
            r = 0x80;
          }

          if( attributes & 2 )
          {
            g = 0x80;
          }

          if( attributes & 4 )
          {
            b = 0x80;
          }

        }

        font->charColor = gfColor( r, g, b );


        //background color

        r = 0;
        g = 0;
        b = 0;

        attributes >>= 4;

        if( attributes & 8 )
        {
          //bright
          if( attributes & 1 )
          {
            r = 0xff;
          }

          if( attributes & 2 )
          {
            g = 0xff;
          }

          if( attributes & 4 )
          {
            b = 0xff;
          }

        }
        else
        {
          //dark
          if( attributes & 1 )
          {
            r = 0x80;
          }

          if( attributes & 2 )
          {
            g = 0x80;
          }

          if( attributes & 4 )
          {
            b = 0x80;
          }

        }



        if( ( attributes & 0x0f ) == 0x07 )
        {
          font->flags = GF_FONT_FLAG_ALPHA_BACKGROUND;
          r = 0;
          g = 0;
                    b = 0;
        }
        else
        {
          font->flags = 0;
        }


        font->backgroundColor = gfColor( r, g, b );

        gfPutChar( bmp, font, x + ( cx * font->width ), y + ( cy * font->height ), letter );
      }
    }
  }

  return 0;
}



ulong toSetCursorPos( tgfTextOverlay *overlay, ushort cursX, ushort cursY )
{
  if( !overlay )
  {
    return 1;
  }

  if( ( cursX < overlay->width ) && ( cursY < overlay->height ) )
  {

    overlay->cursX = cursX;
    overlay->cursY = cursY;

  }
  else
  {
    return 2;
  }

  return 0;
}

ulong toCls( tgfTextOverlay *overlay )
{
  ulong   i;

  if( !overlay )
  {
    return 1;
  }


  for( i = 0; i < ( overlay->width * overlay->height * 2 ); i += 2 )
  {
    overlay->textBuffer[i]    = 0;
    overlay->textBuffer[i+1]  = overlay->textAttributes;
  }

  overlay->cursX = 0;
  overlay->cursY = 0;

  return 0;
}


ulong toScrollUp( tgfTextOverlay *overlay )
{
  ulong   idx;
  ulong   idx2;
  ushort  x;
  ushort  y;

  //scroll up
  if( !overlay )
  {
    return 1;
  }

  for( y = 0; y < overlay->height ; y++ )
  {
    //dest
    idx   = ( y * overlay->width * 2 );
    //src
    idx2  = ( ( y + 1 ) * overlay->width * 2 );

    if( y < overlay->height - 1 )
    {
      //scroll

      for( x = 0; x < overlay->width; x++ )
      {
        overlay->textBuffer[ idx++ ] = overlay->textBuffer[ idx2++ ];
        overlay->textBuffer[ idx++ ] = overlay->textBuffer[ idx2++ ];
      }

    }
    else
    {
      //clear last line
      for( x = 0; x < overlay->width; x++ )
      {
        overlay->textBuffer[ idx++ ] = 0;
        overlay->textBuffer[ idx++ ] = overlay->textAttributes;
      }
    }
  }

  return 0;

}

ulong toPrint( tgfTextOverlay *overlay, char *string )
{
  char    c;
  ulong   idx;

  if( !overlay )
  {
    return 1;
  }

  while( ( c = *string++ ) )
  {
    idx = ( overlay->cursX * 2 ) + ( overlay->cursY * overlay->width * 2 );


    if( c >= 32 )
    {
      overlay->textBuffer[idx++] = c;
      overlay->textBuffer[idx++] = overlay->textAttributes;

      overlay->cursX++;

      if( overlay->cursX >= overlay->width )
      {
        overlay->cursX = 0;
        overlay->cursY++;
      }
    }
    else if( c == 10 )
    {
      overlay->cursX = 0;
      overlay->cursY++;
    }


    if( overlay->cursY >= overlay->height )
    {
      overlay->cursY = overlay->height - 1;

      toScrollUp( overlay );

    }
  }

  return 0;
}



ulong toPrintF( tgfTextOverlay *overlay, char *format, ... )
{
  //not implemented yet
  return 1;
}
