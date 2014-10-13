
#include <stdio.h>
#include <stdlib.h>
#include "libjpeg/jpeglib.h"

int main()
{
  unsigned long x, y;
  unsigned long data_size;     // length of the file
  int channels;               //  3 =>RGB   4 =>RGBA
    unsigned char * rowptr[1];  // pointer to an array
    unsigned char * jdata;      // data for the image
  struct jpeg_decompress_struct info; //for our jpeg info
  FILE* file = fopen("img.jpg", "rb");  //open the file
 /* jpeg_create_decompress(& info);   //fills info structure
  jpeg_stdio_src(&info, file);
  jpeg_read_header(&info, TRUE);   // read jpeg file header
  jpeg_start_decompress(&info);    // decompress the file
  //set width and height
    x = info.output_width;
    y = info.output_height;
    channels = info.num_components;
    data_size = x * y * 3;

//--------------------------------------------
// read scanlines one at a time & put bytes
//    in jdata[] array. Assumes an RGB image
//--------------------------------------------
  jdata = (unsigned char *)malloc(data_size);
  while (info.output_scanline < info.output_height) // loop
    {
// Enable jpeg_read_scanlines() to fill our jdata array
    rowptr[0] = (unsigned char *)jdata +  // secret to method
            3* info.output_width * info.output_scanline;

   jpeg_read_scanlines(&info, rowptr, 1);
  }
//---------------------------------------------------
   jpeg_finish_decompress(&info);   //finish decompressing*/

}

