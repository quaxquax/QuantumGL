/*!
	\file		PngExport.cpp
	\author		Roman Weinberger
	\date		15-11-2002
	
	png exporter
*/
#include "PngExport.h"
#include <string>
#include <cstdio>

void Progress(double x);

bool exportToPng(int w, int h, unsigned long *buffer, const std::string &name)
{   
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL, NULL, NULL);
    if (!png_ptr) return false;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
       png_destroy_write_struct(&png_ptr,
         (png_infopp)NULL);
       return false;
    }
       
    FILE *fp = NULL;
   
    // filename checking
    if( name.find(".png") == name.npos )
        fp = fopen((name+".png").c_str(), "wb");
    else 
        fp = fopen(name.c_str(), "wb");
    
    if( !fp ) return false;    
                          
    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, w, h,
       8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
       
    png_write_info(png_ptr, info_ptr);

    // flipping image vertically
    for(int i=0; i<h; i++) {
        png_write_row(png_ptr, (unsigned char *)&(buffer[(h-(i+1))*w]));
        Progress(double(i) / double(h));
    }

    png_write_end(png_ptr, info_ptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    fclose(fp);
    
    return true;
};
