#include <iostream> 
#include <libjpeg/jpeglib.h>
#include <libtiff/tiffio.h>
#include <stdlib.h> 
#include <csetjmp>
#include <cstdint>
#include <stdexcept>

//Error handling structures.
typedef struct 
{
  jpeg_error_mgr pub;
  jmp_buf buf;
} jem;

METHODDEF(void) jee(j_common_ptr cinfo)
{
  jem *p_jem = (jem *)cinfo->err;
  longjmp(p_jem->buf, 1);
}

void jpeg2tiff(char const* src_path, char const* dst_path)
{
  jpeg_decompress_struct cinfo = {0};
  jem jerr;

  FILE* infile = fopen(src_path, "rb"); // source file 
  if (infile == 0)
    throw std::runtime_error("Error when open src image");

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = jee;

  if (setjmp(jerr.buf)) 
  {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    throw std::runtime_error("Error when read jpeg file");
  }

  jpeg_create_decompress(&cinfo); // initialize the JPEG decompression object
  jpeg_stdio_src(&cinfo, infile); // specify data source
  jpeg_read_header(&cinfo, true); // read file parameters
  jpeg_start_decompress(&cinfo);  // start decompressor

  const int height = cinfo.output_height;
  const int width = cinfo.output_width;
  const int n_channels = cinfo.output_components;
  const int row_stride = width * n_channels;

  TIFF* outfile = TIFFOpen(dst_path, "w");
  if (outfile == 0)
  {
    fclose(infile);
    throw std::runtime_error("Error when open dst image");
  }

  TIFFSetField(outfile, TIFFTAG_IMAGEWIDTH, width);               // set the width of the image
  TIFFSetField(outfile, TIFFTAG_IMAGELENGTH, height);              // set the height of the image
  TIFFSetField(outfile, TIFFTAG_SAMPLESPERPIXEL, n_channels);  // set number of channels per pixel
  TIFFSetField(outfile, TIFFTAG_BITSPERSAMPLE, 8);                 // set the size of the channels
  TIFFSetField(outfile, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT); // set the origin of the image.
  TIFFSetField(outfile, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(outfile, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  
  uint8_t* bufrow = new uint8_t[row_stride]; // Row buffer

  while (cinfo.output_scanline < cinfo.output_height)
  {
    // Read
    jpeg_read_scanlines(&cinfo, &bufrow, 1);

    // Get gray image
    for (int i = 0; i < width; ++i)
    {
      float sum = 0.0F;
      for (int j = 0; j < n_channels; ++ j)
        sum += bufrow[n_channels * i + j];
      const uint8_t gray_pix = static_cast<uint8_t>(sum / n_channels + 0.5F);

      for (int j = 0; j < n_channels; ++ j)
        bufrow[n_channels * i + j] = gray_pix;
    }
    
    // Write
    int tiff_res = TIFFWriteScanline(outfile, bufrow, cinfo.output_scanline-1, 0);
    if (tiff_res < 0)
    {
      fclose(infile);
      TIFFClose(outfile);
      delete [] bufrow;
      throw std::runtime_error("Error when write tiff file");
    }
  }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);
  TIFFClose(outfile);
  delete [] bufrow;
}


int main(int argc, const char** argv)
{
  if (!argv[1])
  {
    std::cout << "You need to specify the input file\n";
    return 1;
  }
  if (!argv[2])
  {
    std::cout << "You need to specify the output file\n";
    return 1;
  }
  try 
  {
    jpeg2tiff(argv[1], argv[2]);
  }
  catch(std::exception e)
  {
    std::cout << e.what() << std::endl;
  }
  return 0;
}