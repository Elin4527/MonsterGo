#ifndef BITMAP_H
#define BITMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

	// Bitmap arranged as Bitmap file header --> Bitmap Info Header --> Palette Data -> Bitmap Data

	// Pack the structs to prevent padding as exact size is important for transferring data from BMP
#pragma pack(push, 1)
	typedef struct
	{
		uint16_t type;                  // File Type
		uint32_t size;                  // File Size
		uint16_t reserved1, reserved2;  // Reserved; must be 0
		uint32_t offset;                // Offset from bitmap file header to bitmap bits
	} BitMapFileHeader;

	typedef struct 
	{
		uint32_t size;                 // Number of bytes required by struct
		int32_t width;                 // Width in pixels
		int32_t height;                // Height in pixels
		uint16_t planes;               // Number of color planes, must be 1
		uint16_t bpp;                  // Number of bits per pixel
		uint32_t compression;          // Type of compression
		uint32_t sizeImage;            // Size of image in bytes
		int32_t xPelsPerMeter;         // number of pixels per meter in x-axis
		int32_t yPelsPerMeter;         // number of pixels per meter in y-axis
		uint32_t clrCount;             // Number of colours used by bitmap
		uint32_t clrImportant;         // Number of colors that are important
	} BitMapInfoHeader;
#pragma pack(pop)

	// Struct to hold final bitmap and info
	typedef struct
	{
		char* bits;
		char* mask;
		BitMapInfoHeader info;
	} BitMap;

	BitMap* loadBitmapFile(const char* filename);
	BitMap* releaseBitmap(BitMap*);

#ifdef __cplusplus
}
#endif
#endif
