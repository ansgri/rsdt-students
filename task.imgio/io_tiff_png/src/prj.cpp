#pragma once

#include <libtiff/tiffio.h>
#include <libpng/png.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>


int writeToPNG(png_byte *buf, uint32 height, uint32 width, const char *path)
{
  FILE *fp;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  png_byte **row_pointers = NULL;
  int status = -1;
  int pixel_size = 3;
  int depth = 8;
  fp = fopen(path, "wb");
  if (!fp)
  {
    printf("error during the opening of png file\n");
    return status;
  }
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL)
  {
    printf("error during png_create_write_struct\n");
    return status;
  }
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
  {
    printf("error during png_create_info_struct\n");
    return status;
  }
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    printf("error during setjmp\n");
    return status;
  }
  png_set_IHDR(png_ptr,
               info_ptr,
               width,
               height,
               depth,
               PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);

  row_pointers = (png_byte**)png_malloc(png_ptr, height * sizeof(png_byte*));
  for (uint32 y = 0; y < height; ++y)
  {
    png_byte *row = (png_byte*)png_malloc(png_ptr, sizeof(png_byte) * width * pixel_size);
    row_pointers[y] = row;
    for (uint32 x = 0; x < width; ++x)
    {
      png_byte pixel = buf[y * width + x];
      *row++ = pixel;
      *row++ = pixel;
      *row++ = pixel;
    }
  }

  png_init_io(png_ptr, fp);
  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  status = 0;

  for (uint32 y = 0; y < height; ++y)
  {
    png_free(png_ptr, row_pointers[y]);
  }
  png_free(png_ptr, row_pointers);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return status;
}

int main(int argc, const char** argv)
{
  if (!argv[1])
  {
    printf("You need to specify the file\n");
    return -1;
  }
  TIFF* tif = TIFFOpen(argv[1], "r");
  if (tif)
  {
    uint32 imagelength;
    unsigned char gray_value;
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imagelength);
    tsize_t scanline = TIFFScanlineSize(tif);
    unsigned char *buf = (unsigned char*)_TIFFmalloc(scanline);
    unsigned char *result = (unsigned char*)malloc(scanline / 3 * imagelength);

    for (uint32 row = 0; row < imagelength; ++row)
    {
      TIFFReadScanline(tif, buf, row);
      for (uint32 col = 0; col < scanline / 3; ++col)
      {
        gray_value = (unsigned char)(((int)buf[col * 3] + (int)buf[col * 3 + 1] + (int)buf[col * 3 + 2]) / 3);
        result[row * scanline / 3 + col] = gray_value;
      }
    }

    if (writeToPNG((png_byte*)result, imagelength, scanline / 3, "result.png") == 0)
    {
      printf("Done!\n");
    }

    free(result);
    _TIFFfree(buf);
    TIFFClose(tif);
  }
  else
  {
    printf("can not open the TIF file\n");
  }
  return 0;
}