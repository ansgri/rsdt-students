#include <iostream>
#include <png.h>
#include <zlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

using namespace std;

int read_png_file(char* file_name)
{
	
	FILE *fp = fopen(file_name, "r");
    if (!fp) 
    {
		cout << "[fopen] error" << endl;
		return 4;
    }
	
	unsigned char sig[8];


    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */

    //fread(sig, 1, 8, fp);
    //if (!png_check_sig(sig, 8))
    //    return 1;   /* bad signature */
        
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        return 4;   /* out of memory */

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 4;   /* out of memory */
    }
    
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 2;
    }
    
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);  /* we already read the 8 signature bytes */

    png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */
    
    
    int  bit_depth, color_type;
    png_uint_32  width, height;
    
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
      NULL, NULL, NULL);
    unsigned long pWidth, pHeight;
    pWidth = width;
    pHeight = height;
    cout << width << " " << height << endl;
}

int main()
{
	cout << read_png_file("../task.imgio/io_png_tiff/test_image/test.png");
	return 0;
}
