/*!
	\file		PngExport.h
	\author		Roman Weinberger
	\date		15-11-2002
	
	png exporter - libpng.a needed!
*/
#include <string>
#include <png.h>

// exports 32bpp RGBA buffers to .png - .png is added if required
bool exportToPng(int w, int h, unsigned long *buffer, const std::string &name);


