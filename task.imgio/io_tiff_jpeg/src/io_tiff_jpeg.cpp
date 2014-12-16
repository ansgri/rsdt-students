#include <iostream>
#include <libjpeg/jpeglib.h>
#include <libtiff/tiffio.h>
#include <stdlib.h>
#include <csetjmp>
#include <cstdint>
#include <stdexcept>

TIFF* tif_read(char const* src_path, uint32& width, uint32& height)
{
  // Read
  TIFF* tif = TIFFOpen(src_path, "r");
  if (tif == 0)
  {
    throw std::runtime_error("Error when open src image");
  }
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, & width);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, & height);

  return tif;
}

uint8* graying(TIFF* tif, uint32* raster, uint8* gray_data, uint32 width, uint32 height)
{
  TIFFReadRGBAImage(tif, width, height, raster, 0);
  for (int i = 0; i < height; ++i)
  {
    for (int j = 0; j < width; ++j)
    {
      uint8 gray_pix = (uint8)(((float)TIFFGetR(raster[i * width + j]) +
                                (float)TIFFGetG(raster[i * width + j]) +
                                (float)TIFFGetB(raster[i * width + j])) / 3. + 0.5);
      gray_data[((height - 1 - i) * width + j) * 3] = gray_pix;
      gray_data[((height - 1 - i) * width + j) * 3 + 1] = gray_pix;
      gray_data[((height - 1 - i) * width + j) * 3 + 2] = gray_pix;
    }
  }
  return gray_data;
}

void jpeg_write(uint8* gray_data, uint32& width, uint32& height, const char* dst_path)
{
  // Write
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE* outfile;             // target file
  JSAMPROW row_pointer[1];    // pointer to JSAMPLE row[s]

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  if ((outfile = fopen(dst_path, "wb")) == NULL)
  {
    delete[] gray_data;
    throw std::invalid_argument("Invalid dst image");
  }
  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = 3;        // of color components per pixel
  cinfo.in_color_space = JCS_RGB;

  int quality = 100;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);

  jpeg_start_compress(&cinfo, TRUE);
  int row_stride = width * 3;

  while (cinfo.next_scanline < cinfo.image_height)
  {
    row_pointer[0] = &gray_data[cinfo.next_scanline * row_stride];
    (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  fclose(outfile);
  jpeg_destroy_compress(&cinfo);
}

void tiff_delete(TIFF* tif, uint32* raster)
{
  _TIFFfree(raster);
  TIFFClose(tif);
}

void tiff_to_jpeg(char const* src_path, char const* dst_path)
{
  uint32 width = 0;
  uint32 height = 0;

  TIFF* tif = tif_read(src_path, width, height);

  uint32* raster = (uint32*)_TIFFmalloc(width * height* sizeof(uint32));
  uint8* gray_data = new uint8[width * height * 3];

  gray_data = graying(tif, raster, gray_data, width, height);
  
  jpeg_write(gray_data, width, height, dst_path);
  
  tiff_delete(tif, raster);
  delete[] gray_data;
}

int task_main (int argc, const char** argv)
{
  if (!argv[1])
  {
    std::cout << "Please, specify the input file\n";
    return 1;
  }
  if (!argv[2])
  {
    std::cout << "Please, specify the output file\n";
    return 1;
  }
  try
  {
    tiff_to_jpeg(argv[1], argv[2]);
  }
  catch (std::exception e)
  {
    std::cout << e.what() << std::endl;
  }
}

int main(int argc, const char** argv)
{
  int io_tiff_jpeg = task_main(argc, argv);
  return 0;
}
