//code found here:https://github.com/BenFradet/LO12
#include "bitmap.h"
Bitmap::Bitmap()
{
	Reset();
}
Bitmap::Bitmap(char* file, int terrain)
{
	Reset();
	LoadBmp(file, terrain);
}
Bitmap::~Bitmap()
{
	if (colors != 0)
		delete[] colors;
	if (data != 0)
		delete[] data;
}
bool Bitmap::LoadBmp(char* file, int terrain)
{
	FILE* in;
	unsigned char* tempData;
	int numColors;
	loaded = false;
	this->filename = file;
	if (colors != 0)
		delete[] colors;
	if (data != 0)
		delete[] data;
	in = fopen(file, "rb");
	if (in == NULL)
	{
		error = "file not found";
		fclose(in);
		return false;
	}
	fread(&bmfh, sizeof(BitmapFileHeader), 1, in);
	cout << "sizeof(BitmapFileHeader)=" << sizeof(BitmapFileHeader) << endl;
	if (bmfh.bfType != BITMAP_MAGIC_NUMBER)
	{
		error = "not bmp";
		cout << error << endl;
		fclose(in);
		return false;
	}
	fread(&bmih, sizeof(BitmapInfoHeader), 1, in);
	cout << "sizeof(BitmapInfoHeader)=" << sizeof(BitmapInfoHeader) << endl;
	width = bmih.biWidth;
	height = bmih.biHeight;
	bpp = bmih.biBitCount;
	cout << "biBitCount =" << bmih.biBitCount << endl;
	cout << "biClrImportant =" << bmih.biClrImportant << endl;
	cout << "biClrUsed =" << bmih.biClrUsed << endl;
	cout << "biClrCompression =" << bmih.biCompression << endl;
	cout << "biHeight =" << bmih.biHeight << endl;
	cout << "biPlanes =" << bmih.biPlanes << endl;
	cout << "biSize =" << bmih.biSize << endl;
	cout << "biSizeImage =" << bmih.biSizeImage << endl;
	cout << "biWidth =" << bmih.biWidth << endl;
	cout << "biXPelsPerMeter =" << bmih.biXPelsPerMeter << endl;
	cout << "biYPelsPerMeter =" << bmih.biYPelsPerMeter << endl;
	dataSize = width * height *(unsigned int)(bmih.biBitCount / 8.0);
	numColors = 1 << bmih.biBitCount;
	if (bpp<8)
	{
		error = "file is not 8 or 24 bits per pixel";
		fclose(in);
		return false;
	}
	if (bpp == 8)
	{
		colors = new RGBQuad[numColors];
		fread(colors, sizeof(RGBQuad), numColors, in);
	}
	tempData = new unsigned char[dataSize];
	if (tempData == NULL)
	{
		error = "not enough memory";
		fclose(in);
		return false;
	}
	fread(tempData, sizeof(char), dataSize, in);
	fclose(in);
	byteWidth = padWidth = (int)((float)width*(float)bpp / 8.0);
	while (padWidth % 4 != 0)
		padWidth++;
	if (bpp == 8)
		loaded = Convert8(tempData);
	if (bpp == 24)
		loaded = Convert24(tempData);
	if (terrain)
		ConvertToGray(tempData);
	delete[] tempData;
	error = "bmp loaded";
	return loaded;
}
void Bitmap::Reset()
{
	loaded = false;
	colors = 0;
	data = 0;
	error = "";
}
bool Bitmap::Convert24(unsigned char* tempData)
{
	int offset, diff;
	diff = width * height * RGB_BYTE_SIZE;
	data = new unsigned char[diff];
	if (data == NULL)
	{
		error = "not enough memory";
		delete[] data;
		return false;
	}
	if (height > 0)
	{
		offset = padWidth - byteWidth;
		for (int i = 0; i<dataSize; i += 3)
		{
			if ((i + 1) % padWidth == 0)
				i += offset;
			*(data + i + 2) = *(tempData + i);
			*(data + i + 1) = *(tempData + i + 1);
			*(data + i) = *(tempData + i + 2);
		}
	}
	else
	{
		offset = padWidth - byteWidth;
		int j = dataSize - 3;
		for (int i = 0; i<dataSize; i += 3)
		{
			if ((i + 1) % padWidth == 0)
				i += offset;
			*(data + j + 2) = *(tempData + i);
			*(data + j + 1) = *(tempData + i + 1);
			*(data + j) = *(tempData + i + 2);
			j -= 3;
		}
	}
	return true;
}
bool Bitmap::Convert8(unsigned char* tempData)
{
	int offset, diff;
	diff = width * height * RGB_BYTE_SIZE;
	data = new unsigned char[diff];
	if (data == NULL)
	{
		error = "not enough memory";
		delete[] data;
		return false;
	}
	if (height > 0)
	{
		offset = padWidth - byteWidth;
		int j = 0;
		for (int i = 0; i<dataSize * RGB_BYTE_SIZE; i += 3)
		{
			if ((i + 1) % padWidth == 0)
				i += offset;
			*(data + i) = colors[*(tempData + j)].rgbRed;
			*(data + i + 1) = colors[*(tempData + j)].rgbGreen;
			*(data + i + 2) = colors[*(tempData + j)].rgbBlue;
			j++;
		}
	}
	else
	{
		offset = padWidth - byteWidth;
		int j = dataSize - 1;
		for (int i = 0; i<dataSize * RGB_BYTE_SIZE; i += 3)
		{
			if ((i + 1) % padWidth == 0)
				i += offset;
			*(data + i) = colors[*(tempData + j)].rgbRed;
			*(data + i + 1) = colors[*(tempData + j)].rgbGreen;
			*(data + i + 2) = colors[*(tempData + j)].rgbBlue;
			j--;
		}
	}
	return true;
}
void Bitmap::ConvertToGray(unsigned char* tempData)
{
	unsigned int size = width * height;
	data = new unsigned char[size];
	for (int i = 0; i < size * 3; i += 3)
	{
		unsigned int current = (unsigned int)i / 3.0;
		data[current] = tempData[i]/**0.2126 + data[i+1]*0.7152 + data[i+2]*0.0722*/;
	}
}
GLuint Bitmap::LoadTexture(char* filename)
{
	GLuint tex;
	FILE* file;
	unsigned char* texture;
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	RGBTriple rgb;
	if ((file = fopen(filename, "rb")) == NULL)
		return (-1);
	fread(&fileHeader, sizeof(fileHeader), 1, file);
	fseek(file, sizeof(fileHeader), SEEK_SET);
	fread(&infoHeader, sizeof(infoHeader), 1, file);
	texture = new byte[infoHeader.biWidth * infoHeader.biHeight * 4]();
	memset(texture, 0, infoHeader.biWidth * infoHeader.biHeight * 4);
	int j = 0;
	for (int i = 0; i < infoHeader.biWidth * infoHeader.biHeight; i++)
	{
		fread(&rgb, sizeof(rgb), 1, file);
		texture[j + 0] = rgb.rgbRed;
		texture[j + 1] = rgb.rgbGreen;
		texture[j + 2] = rgb.rgbBlue;
		texture[j + 3] = 255;
		j += 4;
	}
	fclose(file);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, infoHeader.biWidth, infoHeader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoHeader.biWidth, infoHeader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, texture);
	delete texture;
	return tex;
}