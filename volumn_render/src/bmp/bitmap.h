#ifndef _BITMAP_H_
#define _BITMAP_H_

typedef long  LONG;
typedef unsigned long       DWORD;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;



#pragma pack(push,2)
struct  BITMAPFILEHEADER1
{
	unsigned short    bfType;
    unsigned long   bfSize;
	unsigned short    bfReserved1;
	unsigned short    bfReserved2;
	unsigned long   bfOffBits;  
    
	DWORD      biSize;
    LONG       biWidth;
    LONG       biHeight;
    WORD       biPlanes;
    WORD       biBitCount;
    DWORD      biCompression;
    DWORD      biSizeImage;
    LONG       biXPelsPerMeter;
    LONG       biYPelsPerMeter;
    DWORD      biClrUsed;
    DWORD      biClrImportant;
};
#pragma pack(pop)


//#define BI_RGB       0             
//#define BI_RLE8      1           
//#define BI_RLE4      2             
//#define BI_BITFIELDS 3    


GLubyte *LoadBitmapFile(const char *fileName,BITMAPFILEHEADER1	*bitmapFileHeader,GLubyte alpha = 255,GLubyte r = 0,GLubyte g = 255,GLubyte b = 0);
#endif	// _BITMAP_H_

