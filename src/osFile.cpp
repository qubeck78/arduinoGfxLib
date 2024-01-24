
#include "osFile.h"


/*

//win
#ifdef _GFXLIB_WINDOWS

in = fopen( fileName, "rb" );
fgetc( in );
fread( dest->buffer, 2, dest->width * dest->height, in );
fclose( in );

//esp
#ifdef _GFXLIB_ESP32_FFAT

in = FFat.open( fileName, FILE_READ );
in.read( (uint8_t*)dest->buffer, ( dest->width * dest->height ) * 2 );
in.read();
in.close();

//stm
#ifdef _GFXLIB_STM32_FATFS

res = f_open( &file, fileName, FA_OPEN_EXISTING | FA_READ );
if( res == FR_OK )
---
f_read( &file, buf, 2, &br );
f_close( &file );


*/


ulong osFOpen( tosFile *file, char *path, ulong mode )
{
  #ifdef _GFXLIB_WINDOWS

  char    winMode[8];

  #endif

  #ifdef _GFXLIB_STM32_FATFS

  FRESULT res;
    BYTE    stmMode;
  #endif
  
  if( !file )
  {
    return 1;
  }

  #ifdef _GFXLIB_WINDOWS

  switch( mode )
  {
    case OS_FILE_READ:

      strcpy( winMode, "rb" );
      break;

    case OS_FILE_WRITE:

      strcpy( winMode, "wb" );
      break;

    case OS_FILE_APPEND:

      strcpy( winMode, "ab" );
      break;

    default:

      return 2;

  }


  file->fd = fopen( path, winMode );

  if( file->fd )
  {
    return 0;
  }
  else
  {
    return 3;
  }

  #endif


  #ifdef _GFXLIB_STM32_FATFS

  switch( mode )
  {
    case OS_FILE_READ:

      stmMode = FA_READ | FA_OPEN_EXISTING;
      break;

    case OS_FILE_WRITE:

      stmMode = FA_WRITE | FA_CREATE_ALWAYS;
      break;

    case OS_FILE_APPEND:

            stmMode = FA_OPEN_APPEND | FA_OPEN_EXISTING;
      break;

    default:

      return 2;

  }



  res = f_open( &file->fd, path, stmMode );

  if( res == FR_OK )
  {
    return 0;
  }
  else
  {
    return 3;
  }


  #endif


  #ifdef _GFXLIB_ESP32_FFAT
 
  switch( mode )
  {
    case OS_FILE_READ:

      file->fd = FFat.open( path, FILE_READ );
      
      break;

    case OS_FILE_WRITE:

      file->fd = FFat.open( path, FILE_WRITE );
      break;

    case OS_FILE_APPEND:

      file->fd = FFat.open( path, FILE_APPEND );
      break;

    default:

      return 2;

  }

  if( file->fd )
  {
    return 0;
  }
  else
  {
    return 3;
  }
  
  #endif

  #ifdef _GFXLIB_ESP32_SD_FAT

  switch( mode )
  {
    case OS_FILE_READ:

      file->fd = SD.open( path, FILE_READ );
      
      break;

    case OS_FILE_WRITE:

      file->fd = SD.open( path, FILE_WRITE );
      break;

    case OS_FILE_APPEND:

      file->fd = SD.open( path, FILE_APPEND );
      break;

    default:

      return 2;

  }

  if( file->fd )
  {
    return 0;
  }
  else
  {
    return 3;
  }

  #endif
  
  return 255;
}

ulong osFClose( tosFile *file )
{

  if( !file )
  {
    return 1;
  }


  #ifdef _GFXLIB_WINDOWS

  fclose( file->fd );

  return 0;

  #endif

  #ifdef _GFXLIB_STM32_FATFS

  f_close( &file->fd );

  return 0;

  #endif

  #ifdef _GFXLIB_ESP32_FFAT
  
  file->fd.close();
  return 0;
  
  #endif
  
  #ifdef _GFXLIB_ESP32_SD_FAT

  file->fd.close();
  return 0;
  
  #endif
  
  return 255;
}

ulong osFWrite( tosFile *file, uchar *buffer, ulong numBytesToWrite )
{
  #ifdef _GFXLIB_STM32_FATFS

  FRESULT res;
  UINT    nbw;

  #endif

  if( ( !file ) || ( !buffer ) )
  {
    return 1;
  }

  #ifdef _GFXLIB_WINDOWS

  if ( fwrite( buffer, numBytesToWrite, 1, file->fd ) != numBytesToWrite )
  {
    return 2;
  }
  else
  {
    return 0;
  }

  #endif


  #ifdef _GFXLIB_STM32_FATFS

  res = f_write( &file->fd, buffer, numBytesToWrite, &nbw );

  if( res == FR_OK )
  {
    return 0;
  }
  else
  {
    return 2;
  }
  #endif

  #ifdef _GFXLIB_ESP32_FFAT
  
  file->fd.write( (uint8_t*)buffer, numBytesToWrite );
  return 0;
  
  #endif

  #ifdef _GFXLIB_ESP32_SD_FAT
  
  file->fd.write( (uint8_t*)buffer, numBytesToWrite );
  return 0;
  
  #endif
  
  return 255;
}

ulong osFRead( tosFile *file, uchar *buffer, ulong numBytesToRead, ulong *numBytesRead )
{

  #ifdef _GFXLIB_WINDOWS

  int   nbr;

  #endif

  #ifdef _GFXLIB_STM32_FATFS

  FRESULT res;
  UINT    nbr;

  #endif

  #ifdef _GFXLIB_ESP32_FFAT

  int   nbr;

  #endif

  #ifdef _GFXLIB_ESP32_SD_FAT

  int   nbr;

  #endif

  if( ( !file ) || ( !buffer ) )
  {
    return 1;
  }


  #ifdef _GFXLIB_WINDOWS

  nbr = fread( buffer, numBytesToRead, 1, file->fd );

  if( numBytesRead )
  {
    *numBytesRead = nbr;

    return 0;
  }
  else
  {
    if( nbr == numBytesToRead )
    {
      return 0;
    }
    else
    {
      return 2;
    }
  }

  #endif


  #ifdef _GFXLIB_STM32_FATFS

  res = f_read( &file->fd, buffer, numBytesToRead, &nbr );

  if( numBytesRead )
  {
    *numBytesRead = nbr;

    return 0;
  }
  else
  {
    if( nbr == numBytesToRead )
    {
      return 0;
    }
    else
    {
      return 2;
    }
  }
  #endif
  
  #ifdef _GFXLIB_ESP32_FFAT
  
  nbr = file->fd.read( (uint8_t*)buffer, numBytesToRead );
  
  if( numBytesRead )
  {
    *numBytesRead = nbr;

    return 0;
  }
  else
  {
    if( nbr == numBytesToRead )
    {
      return 0;
    }
    else
    {
      return 2;
    }
  }
  
  #endif

  #ifdef _GFXLIB_ESP32_SD_FAT
  
  nbr = file->fd.read( (uint8_t*)buffer, numBytesToRead );
  
  if( numBytesRead )
  {
    *numBytesRead = nbr;

    return 0;
  }
  else
  {
    if( nbr == numBytesToRead )
    {
      return 0;
    }
    else
    {
      return 2;
    }
  }

  #endif
  
  return 255;
}
