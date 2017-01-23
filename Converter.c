#include "Bitmap.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	// Input paths, and array name
	char path[100], path2[100], name[100];
	scanf("%[^\n]%*c", path);
	scanf("%[^\n]%*c", path2);
	scanf("%[^\n]%*c", name);

	// BMP Path to load BitMap into array
	BitMap *bm = loadBitmapFile(path);
	
	// Output file for prototype
	FILE *f = fopen(path2, "a");
	
	// Only execute if bitmap read was successful
	if(bm)
	{	
		// Vertical Bytes so size of image is height rounded up to nearest 8 * width
		int size = (bm->info.height + 7)/8 * bm->info.width;
		
		// Output function prototype to file
		fprintf(f, "extern char %s[2][%d]; \t//%d x %d\n", name,size, bm->info.width, bm->info.height);

		// Output bitmaps as a 2D array of chars, print out a commented image preview as well to quickly check each bitmap
		printf("char %s[2][%d] = { {\t//%d x %d", name,size, bm->info.width, bm->info.height);
		for(int i = 0; i < bm->info.height; i++)
		{
			printf("\n\t// ");
			for(int j = 0; j < bm->info.width; j++)
			{
				unsigned char c = (unsigned char)bm->bits[(i / 8) * (bm->info.width) + j];
				if(c & 1 << (i % 8)) printf("#");
				else printf("-");
			}
		}
		for(int i = 0; i < size; i++)
		{	
			if(i % 32 == 0)
			{
				printf("\n\t");
			}
			printf("0x%02x", (unsigned char)bm->bits[i]);
			if(i < size - 1)
			{
				printf(", ");
			}
		}
		printf("},\n\t{");
		for(int i = 0; i < bm->info.height; i++)
		{
			printf("\n\t// ");
			for(int j = 0; j < bm->info.width; j++)
			{
				unsigned char c = (unsigned char)bm->mask[(i / 8) * (bm->info.width) + j];
				if(c & 1 << (i % 8)) printf("#", 219);
				else printf("-");
			}
		}
		for(int i = 0; i < size; i++)
		{	
			if(i % 32 == 0)
			{
				printf("\n\t");
			}
			printf("0x%02x", (unsigned char)bm->mask[i]);
			if(i < size - 1)
			{
				printf(", ");
			}
		}
		printf("}};\n\n");
		
		// Free the memory
		bm = releaseBitmap(bm);
	}
	return 0;
}
