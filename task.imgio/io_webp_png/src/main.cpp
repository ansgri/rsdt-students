#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include <libwebp/webp/decode.h>
#include <png.h>


inline void setGray(png_byte *ptr, float val)
{
}


int main()
{
	FILE * pFile;
	long lSize;
	uint8_t *data;
	uint8_t *buffer;
	size_t result;
	
	pFile = fopen ( "1.webp" , "rb" );
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	data = (uint8_t*) malloc (sizeof(uint8_t)*lSize);
	if (data == NULL) {fputs ("Memory error",stderr); exit (2);}

	// copy the file into the buffer:
	result = fread (data,1,lSize,pFile);
	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}


	int*height;
	int *width;
	buffer = WebPDecodeRGBA(data, result , width, height);
	
	
	
	
	 png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      fprintf(stderr, "Could not allocate write struct\n");
      return 1;
   }

   // Initialize info structure
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      fprintf(stderr, "Could not allocate info struct\n");
      return 1;
   }
   FILE *fp;
   fp = fopen("res.png", "wb");
   png_init_io(png_ptr, fp);

   // Write header (8 bit colour depth)
   png_set_IHDR(png_ptr, info_ptr, (png_uint_32)width, (png_uint_32)height,
         8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	char * title = new char [5];
   // Set title
   if (title != NULL) {
      png_text title_text;
      title_text.compression = PNG_TEXT_COMPRESSION_NONE;
      title_text.key = "Title";
      title_text.text = "test";
      png_set_text(png_ptr, info_ptr, &title_text, 1);
   }

   png_write_info(png_ptr, info_ptr);
   
   png_bytep row = (png_bytep) malloc(3 * (*width) * sizeof(png_byte));

   // Write image data
   int x, y;
   for (y=0 ; y< (*height) ; y++) {
      for (x=0 ; x< (*width) ; x++) {
         setGray(&(row[x*3]), buffer[y* (*width) + x]);
      }
      png_write_row(png_ptr, row);
   }

   // End write
   png_write_end(png_ptr, NULL);
   printf("%s\n","finish");
	fclose (pFile);
	free (buffer);
	return 0;
}
