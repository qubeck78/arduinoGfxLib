#include "gfGouraud.h"
#include "gfDrawing.h"

#ifdef _GFXLIB_WINDOWS

  #include <stdio.h>
  #include <vcl.h>

#endif

#ifdef _GFXLIB_ESP32_FFAT

  #define NDEBUG

  #include "FS.h"
  #include "FFat.h"


#endif

//https://austinmorlan.com/posts/drawing_a_triangle/


long gfGouraudEdge( tgfPoint3D *e1, tgfPoint3D *e2, tgfPoint3D *p )
{
  tgfPoint3D a,b;

/*float Edge(Point v0, Point v1, Point p)
{
  // Vector from edge origin to test point
  Vector a = p - v0;

  // Vector from edge origin to edge end
  Vector b = v1 - v0;

  // 2D cross product
  // Zero: Point is on edge
  // Positive: Point is right of edge
  // Negative: Point is left of edge
  return a.x * b.y - a.y * b.x;
}*/


  a.x2D = p->x2D - e1->x2D;
  a.y2D = p->y2D - e1->y2D;

  b.x2D = e2->x2D - e1->x2D;
  b.y2D = e2->y2D - e1->y2D;

  return ( ( a.x2D * b.y2D ) - ( a.y2D * b.x2D ) );
}


ulong gfGouraudDrawTriangle( tgfBitmap *bmp, tgfTriangle3D *triangle )
{
  short       x;
  short   y;

  short       xMin;
  short       xMax;
  short       yMin;
  short       yMax;

  long        eba;
  long        ecb;
  long        eac;

  float       area;

  float       wba;
  float       wcb;
  float       wac;

  short       r;
  short       g;
  short       b;

  tgfPoint3D  p;


  xMin = bmp->width - 1;
  yMin = bmp->height - 1;
  xMax = 0;
  yMax = 0;

  if( triangle->a->x2D > xMax )
  {
    xMax = triangle->a->x2D;
  }
  if( triangle->b->x2D > xMax )
  {
    xMax = triangle->b->x2D;
  }
  if( triangle->c->x2D > xMax )
  {
    xMax = triangle->c->x2D;
  }

  if( triangle->a->x2D < xMin )
  {
    xMin = triangle->a->x2D;
  }
  if( triangle->b->x2D < xMin )
  {
    xMin = triangle->b->x2D;
  }
  if( triangle->c->x2D < xMin )
  {
    xMin = triangle->c->x2D;
  }

  if( triangle->a->y2D > yMax )
  {
    yMax = triangle->a->y2D;
  }
  if( triangle->b->y2D > yMax )
  {
    yMax = triangle->b->y2D;
  }
  if( triangle->c->y2D > yMax )
  {
    yMax = triangle->c->y2D;
  }

  if( triangle->a->y2D < yMin )
  {
    yMin = triangle->a->y2D;
  }
  if( triangle->b->y2D < yMin )
  {
    yMin = triangle->b->y2D;
  }
  if( triangle->c->y2D < yMin )
  {
    yMin = triangle->c->y2D;
  }

  if ( xMin < 0 )
  {
    xMin = 0;
  }
  if ( yMin < 0 )
  {
    yMin = 0;
  }
  if ( xMax >= bmp->width )
  {
    xMax = bmp->width - 1;
  }
  if ( yMax >= bmp->height )
  {
    yMax = bmp->height - 1;
  }

  area = gfGouraudEdge( triangle->c, triangle->b, triangle->a );

  if( area > 0 )
  for( y = yMin; y <= yMax; y++ )
  {
    for( x = xMin; x <= xMax; x++ )
    {

      p.x2D = x;
      p.y2D = y;

      eba = gfGouraudEdge( triangle->b, triangle->a, &p );
      ecb = gfGouraudEdge( triangle->c, triangle->b, &p );
      eac = gfGouraudEdge( triangle->a, triangle->c, &p );

      //check if point inside triangle

      if( ( eba >= 0 ) && ( ecb >= 0 ) && ( eac >= 0 ) )
      {

        wba = eba / area;
        wcb = ecb / area;
        wac = eac / area;

        r = ( wcb * ( float )triangle->a->r ) + ( wac * ( float )triangle->b->r ) + ( wba * ( float )triangle->c->r );
        g = ( wcb * ( float )triangle->a->g ) + ( wac * ( float )triangle->b->g ) + ( wba * ( float )triangle->c->g );
        b = ( wcb * ( float )triangle->a->b ) + ( wac * ( float )triangle->b->b ) + ( wba * ( float )triangle->c->b );


        gfPlotF( bmp, x, y, gfColor( r, g, b ) );

      }

    }
  }

  return 0;
}


ulong gfGouraudDrawTexturedTriangle( tgfBitmap *bmp, tgfTriangle3D *triangle )
{

  short       x;
  short       y;

  short       xMin;
  short       xMax;
  short       yMin;
  short       yMax;

  long        eba;
  long        ecb;
  long        eac;

  float       area;

  float       wba;
  float       wcb;
  float       wac;

  short       tx;
  short       ty;

  tgfPoint3D  p;

  xMin = bmp->width - 1;
  yMin = bmp->height - 1;
  xMax = 0;
  yMax = 0;

  if( triangle->a->x2D > xMax )
  {
    xMax = triangle->a->x2D;
  }
  if( triangle->b->x2D > xMax )
  {
    xMax = triangle->b->x2D;
  }
  if( triangle->c->x2D > xMax )
  {
    xMax = triangle->c->x2D;
  }

  if( triangle->a->x2D < xMin )
  {
    xMin = triangle->a->x2D;
  }
  if( triangle->b->x2D < xMin )
  {
    xMin = triangle->b->x2D;
  }
  if( triangle->c->x2D < xMin )
  {
    xMin = triangle->c->x2D;
  }

  if( triangle->a->y2D > yMax )
  {
    yMax = triangle->a->y2D;
  }
  if( triangle->b->y2D > yMax )
  {
    yMax = triangle->b->y2D;
  }
  if( triangle->c->y2D > yMax )
  {
    yMax = triangle->c->y2D;
  }

  if( triangle->a->y2D < yMin )
  {
    yMin = triangle->a->y2D;
  }
  if( triangle->b->y2D < yMin )
  {
    yMin = triangle->b->y2D;
  }
  if( triangle->c->y2D < yMin )
  {
    yMin = triangle->c->y2D;
  }

  if ( xMin < 0 )
  {
    xMin = 0;
  }
  if ( yMin < 0 )
  {
    yMin = 0;
  }
  if ( xMax >= bmp->width )
  {
    xMax = bmp->width - 1;
  }
  if ( yMax >= bmp->height )
  {
    yMax = bmp->height - 1;
  }

  area = gfGouraudEdge( triangle->c, triangle->b, triangle->a );

  if( area > 0 )
  for( y = yMin; y <= yMax; y++ )
  {
    for( x = xMin; x <= xMax; x++ )
    {

      p.x2D = x;
      p.y2D = y;

      eba = gfGouraudEdge( triangle->b, triangle->a, &p );
      ecb = gfGouraudEdge( triangle->c, triangle->b, &p );
      eac = gfGouraudEdge( triangle->a, triangle->c, &p );

      //check if point inside triangle

      if( ( eba >= 0 ) && ( ecb >= 0 ) && ( eac >= 0 ) )
      {

        wba = eba / area;
        wcb = ecb / area;
        wac = eac / area;


        tx = ( wcb * ( float )triangle->aTx2D ) + ( wac * ( float )triangle->bTx2D ) + ( wba * ( float )triangle->cTx2D );
        ty = ( wcb * ( float )triangle->aTy2D ) + ( wac * ( float )triangle->bTy2D ) + ( wba * ( float )triangle->cTy2D );


        gfPlotF( bmp, x, y, gfGetPixel( triangle->texture, tx, ty ) );

      }

    }
  }

  return 0;


}
