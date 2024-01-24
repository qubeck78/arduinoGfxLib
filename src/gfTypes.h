#ifndef _GFTYPES_H
#define _GFTYPES_H

//#define _GFXLIB_ESP32_FFAT
#define _GFXLIB_ESP32_SD_FAT


typedef unsigned char	uchar;
typedef unsigned char	ubyte;
typedef unsigned short	ushort;
typedef unsigned long	ulong;

#ifndef NULL
#define NULL 0
#endif

#define gfAbs(x) ((x)>0?(x):-(x))

#endif
