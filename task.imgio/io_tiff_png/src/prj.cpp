#include "tiffio.h"

int main( int argc, const char** argv )
{
    TIFF* tif = TIFFOpen("foo.tif", "r");
    if (tif) {
        uint32 imagelength;
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imagelength);
        tdata_t buf; = _TIFFmalloc(TIFFScanlineSize(tif));
        
        for (uint32 row = 0; row < imagelength; ++row)
        {
            TIFFReadScanline(tif, buf, row);
            
            
            //http://www.libpng.org/pub/png/libpng-1.4.0-manual.pdf
            //http://www.libtiff.org/libtiff.html#FIO
        }
        
        
        _TIFFfree(buf);
        TIFFClose(tif);
    }
    return 0;
}
