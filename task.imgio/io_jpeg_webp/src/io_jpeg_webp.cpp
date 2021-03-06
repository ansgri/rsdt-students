#include <iostream>
#include <libjpeg/jpeglib.h>
#include <libjpeg/jerror.h>
#include <libwebp/webp/encode.h>

void convert_jpg_to_webp_gray(const char* file_name, const char* out_file_name)
{
  unsigned long x, y;       //  width, height
  unsigned long data_size;  // length of the file
  unsigned char * rowptr[1];  // pointer to an array
  unsigned char * jpeg_data;  // data for input image
  unsigned char * webp_data;  // data for output image

  struct jpeg_decompress_struct info;     //  for jpeg info
  struct jpeg_error_mgr err;      //  the error handler

  FILE* file = fopen(file_name, "rb");      //  open the file

  info.err = jpeg_std_error(& err);
  jpeg_create_decompress(& info);       //  fills info structure

  if(!file)  // if the jpeg file doesn't load
  {
    fprintf(stderr, "Error reading JPEG files %s!", file_name);
    return;
  }

  jpeg_stdio_src(&info, file);
  jpeg_read_header(&info, TRUE);  // read jpeg file header

  jpeg_start_decompress(&info);  // decompress the file

  x = info.output_width;  // set width and height
  y = info.output_height;
  data_size = x * y * 3;

  jpeg_data = (unsigned char *)malloc(data_size+1);  // allocating memory
  webp_data = (unsigned char *)malloc(data_size+1);
  int webp_iterator = 0;
  unsigned char gray_value;  // variable for avegare of RGB channels

  while (info.output_scanline < info.output_height)  // loop
  {
    rowptr[0] = (unsigned char *)jpeg_data + 3 * info.output_width * info.output_scanline;
    jpeg_read_scanlines(&info, rowptr, 1);
    for (int i = 0; i < x * 3; i+=3)
    {
      gray_value = (unsigned char)(((int)rowptr[0][i] + (int)rowptr[0][i+1] + (int)rowptr[0][i+2])/3);       //  calc the avrg

      webp_data[webp_iterator] = gray_value;  // all three channels have the same value
      webp_data[webp_iterator + 1] = gray_value;
      webp_data[webp_iterator + 2] = gray_value;

      webp_iterator+=3;
    }
  }
  jpeg_finish_decompress(&info);  // finish decompressing

  uint8_t* output;
  FILE *op_file;
  size_t d_size;

  d_size = WebPEncodeRGB(webp_data, x, y, 3 * x, 100.0, &output);  // encoding image
  op_file=fopen(out_file_name,"w");  // writing file out
  fwrite(output,1,(int)d_size,op_file);
}
int main( int argc, const char** argv )
{
  if (argc != 3)
  {
    std::cout << "Bad usage: must have input image and ouput image as arg\n";
    return 1;
  }
  convert_jpg_to_webp_gray(argv[1], argv[2]);
  return 0;
}
