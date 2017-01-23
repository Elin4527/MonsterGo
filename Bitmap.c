#include "Bitmap.h"

// This only works with bitmaps encoded in RGB
BitMap* loadBitmapFile(const char* file)
{
	FILE* f;
	BitMapFileHeader bmFileHeader;
	BitMap *bm = (BitMap*)malloc(sizeof(BitMap));
	unsigned char* rawData;
	int* grayScale;

	if(!bm) return NULL;

	f = fopen(file, "rb");
	if(!f) return NULL;

	fread(&bmFileHeader, sizeof(BitMapFileHeader), 1, f);

	// Verify that bmp file is a bitmap
	// Bitmaps are identified by BM (little endian)
	if(bmFileHeader.type != 0x4d42)
	{
		fclose(f);
		return NULL;
	}

	// Read in Info Header
	fread(&(bm->info), sizeof(BitMapInfoHeader), 1, f);
	
	// Allocate Storage for raw bitmap and grayscale bitmap
	rawData = (unsigned char*)malloc(bm->info.sizeImage);
	uint32_t rowSize = ((bm->info.bpp * bm->info.width + 31)/32) * 4;
	grayScale = (int*)malloc(bm->info.width * bm->info.height * sizeof(int));

	if(!rawData || !grayScale)
	{
		free(rawData);
		free(grayScale);
		free(bm);
		fclose(f);
		return NULL;
	}

	// Read in Bitmap
	fseek(f, bmFileHeader.offset, SEEK_SET);
	fread(rawData, bm->info.sizeImage, 1, f);

	if(!rawData)
	{
		free(rawData);
		free(grayScale);
		free(bm);
		fclose(f);
		return NULL;
	}
	
	// Finished with file, close
	fclose(f);
	
	
	// Array is little endian the RGB values are BGR
	// Convert these to grayscale by 0.21R + 0.72G + 0.07B
	// Based on human perception of color
	for(int i = 0; i < bm->info.height; i++)
	{
		for(int j = 0; j < bm->info.width; j++)
		{
			// Bitmaps are stored upside down so process the bottom row first 
			int ind = ((bm->info.height - 1) - i) * rowSize + j * 3;
			grayScale[i * bm->info.width + j] = (0.07 * rawData[ind] + 0.72 * rawData[ind + 1] + 0.21 * rawData[ind + 2] + 0.5);
		}
	}
	free(rawData);

	uint32_t charPerCol = (bm->info.height + 7) / 8; // 1bpp 8 pixels per char round up
	// Storage for 1bpp bitmaps
	bm->bits = (char*)malloc(charPerCol * bm->info.width);
	bm->mask = (char*)malloc(charPerCol * bm->info.width);

	if(!bm->bits || !bm->mask)
	{
		free(grayScale);
		free(bm->bits);
		free(bm->mask);
		free(bm);
		return NULL;
	}

	memset(bm->bits, 0, charPerCol*bm->info.width);
	memset(bm->mask, 0xff, charPerCol*bm->info.width);

	// Only white pixels are set in the mask
	for(int i = 0; i < bm->info.height; i++)
	{
		for(int j = 0; j < bm->info.width; j++)
		{
			if(grayScale[i * bm ->info.width + j] < 255)
			{
				bm->mask[(i / 8) * (bm->info.width) + j] ^= (1 << (i % 8));
			}
		}
	}

	// Convert grayscale to 1bpp black and white
	// Uses Floyd-Steinburg Dithering https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering
	for(int i = 0; i < bm->info.height; i++)
	{
		for(int j = 0; j < bm->info.width; j++)
		{
			int old = grayScale[i * bm->info.width + j];
			int res = (old > 127) * 255;
			
			int error = old - res;
			if(j < bm->info.width - 1)
			{
				grayScale[i * bm->info.width + j + 1] += error * 7 / 16;
			}
			if(i < bm->info.height - 1)
			{
				grayScale[(i+1) * bm->info.width + j] += error * 5 / 16;
				if(j > 0)
				{
					grayScale[(i+1) * bm->info.width + j - 1] += error * 3 / 16;
				}
				if( j < bm->info.width -1)
				{
					grayScale[(i+1) * bm->info.width + j + 1] += error / 16;
				}
			}
			
			if(!res)
			{
				// Vertical Byte Allignment (Bytes are drawn in columns)
				bm->bits[(i / 8) * (bm->info.width) + j] |= (1 << (i % 8));
			}

		}
	}

	free(grayScale);
	return bm;
}


// Free BitMap* and return NULL
BitMap* releaseBitmap(BitMap* bm)
{
	if(bm)
	{
		free(bm->bits);
		free(bm->mask);
		free(bm);
	}
	return NULL;
}
