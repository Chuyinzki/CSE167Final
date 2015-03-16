//code found here:https://github.com/BenFradet/LO12
#ifndef BITMAP_H
#define BITMAP_H
#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <cstdio>
#include <string>
#include <Windows.h>
#include <GL/glut.h>
#include <gl\gl.h>
#include <gl\glu.h>
using namespace std;
const short BITMAP_MAGIC_NUMBER = 19778;
const int RGB_BYTE_SIZE = 3;
#pragma pack(push, bitmap_data, 1)
typedef struct tagRGBQuad
{
	char rgbBlue;
	char rgbGreen;
	char rgbRed;
	char rgbReserved;
}RGBQuad;
typedef struct tagRGBTriple
{
	char rgbBlue;
	char rgbGreen;
	char rgbRed;
}RGBTriple;
typedef struct tagBitmapFileHeader
{
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
}BitmapFileHeader;
typedef struct tagBitmapInfoHeader
{
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
}BitmapInfoHeader;
#pragma pack(pop, bitmap_data)
class Bitmap
{
public:
	RGBQuad* colors;
	unsigned char* data;
	bool loaded;
	int width, height;
	unsigned short bpp;
	string error;
	char* filename;
	Bitmap(void);
	Bitmap(char*, int);
	~Bitmap();
	bool LoadBmp(char*, int);
	void ConvertToGray(unsigned char*);
	static GLuint LoadTexture(char*);
private:
	BitmapFileHeader bmfh;
	BitmapInfoHeader bmih;
	int byteWidth;
	int padWidth;
	unsigned int dataSize;
	void Reset(void);
	bool Convert24(unsigned char*);
	bool Convert8(unsigned char*);
};
#endif