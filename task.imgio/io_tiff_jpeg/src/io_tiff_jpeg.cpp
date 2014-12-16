#include <iostream>
#include <libjpeg/jpeglib.h>
#include <libtiff/tiffio.h>
#include <stdlib.h>
#include <csetjmp>
#include <cstdint>
#include <stdexcept>

void tiff_to_jpeg(char const * src_path, char const * dst_path)
{
	// Read
	TIFF *tif = TIFFOpen(src_path, "r");
	if (tif == 0)
		throw std::runtime_error("Error when open src image");

	uint32 width;
	uint32 height;
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

	uint32 npixels = width*height;
	uint32* raster = (uint32*)_TIFFmalloc(npixels * sizeof(uint32));

	TIFFReadRGBAImage(tif, width, height, raster, 0);

	uint8* gray_data = new uint8[width * height * 3];

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			uint8 gray_pix = (uint8)(((float)TIFFGetR(raster[i * width + j]) +
				(float)TIFFGetG(raster[i * width + j]) +
				(float)TIFFGetB(raster[i * width + j])) / 3. + 0.5);
			gray_data[((height - 1 - i) * width + j) * 3] = gray_pix;
			gray_data[((height - 1 - i) * width + j) * 3 + 1] = gray_pix;
			gray_data[((height - 1 - i) * width + j) * 3 + 2] = gray_pix;
		}
	}
	_TIFFfree(raster);
	TIFFClose(tif);

	// Write
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;             // target file
	JSAMPROW row_pointer[1];    // pointer to JSAMPLE row[s]

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	if ((outfile = fopen(dst_path, "wb")) == NULL) {
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

	delete[] gray_data;
	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
}

int main(int argc, const char ** argv)
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
	return 0;
}
