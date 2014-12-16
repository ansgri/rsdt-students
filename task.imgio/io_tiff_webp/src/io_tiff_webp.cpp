#include <libtiff/tiffio.h>
#include <libwebp/webp/encode.h>
#include <cstdlib>
#include <cstdint>
#include <cstdio>

void convert_tiff_to_webp_gray(const char* file_name, const char* out_file_name)
{
  TIFF* tif = TIFFOpen(file_name, "r");
  if (tif)
  {
    uint32 w, h;
    
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

    uint32 imagelength;
    unsigned char gray_value;
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imagelength);
    tsize_t scanline = TIFFScanlineSize(tif);

    unsigned char *buf = (unsigned char*)_TIFFmalloc(scanline);
    unsigned char *result = (unsigned char*)malloc(3 * w * h + 1);

    int iterator = 0;

    for (uint32 row = 0; row < h; ++row)
    {
      TIFFReadScanline(tif, buf, row);
      for (uint32 col = 0; col < w * 3; col+=3)
      {
        gray_value = (unsigned char)(((float)buf[col] + (float)buf[col + 1] + (float)buf[col + 2]) / 3.0 + 0.5);
        result[iterator] = gray_value;
        result[iterator + 1] = gray_value;
        result[iterator + 2] = gray_value;

        iterator += 3;
      }
    }

    uint8_t* output;
    FILE *op_file;
    size_t d_size;

    d_size = WebPEncodeRGB(result, w, h, 3 * w, 100.0, &output);
    op_file=fopen(out_file_name,"wb");
    fwrite(output,1,(int)d_size, op_file);

    free(result);
    _TIFFfree(buf);
    TIFFClose(tif);
  }
  else
  {
    printf("I can't open the TIF file\n");
  }

}

int main(int argc, const char** argv)
{
  convert_tiff_to_webp_gray(argv[1], argv[2]);
  return 0;
}