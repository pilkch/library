// Standard headers
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <array>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

// SDL headers
#include <SDL3_image/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

// libvoodoomm headers
#include <libvoodoomm/cImage.h>

#ifdef PLATFORM_LINUX_OR_UNIX
#include <sys/stat.h>
#endif

namespace {

typedef unsigned char RGBE[4];
#define R			0
#define G			1
#define B			2
#define E			3

#define  MINELEN	8				// minimum scanline length for encoding
#define  MAXELEN	0x7fff			// maximum scanline length for encoding

}

namespace voodoo
{

void workOnRGBE(RGBE *scan, int len, float *cols);
bool decrunch(RGBE *scanline, int len, FILE *file);
bool oldDecrunch(RGBE *scanline, int len, FILE *file);

bool LoadHDR(const std::string& sFilePath, cImage& outImage)
{
	int i;
	char str[200];
	FILE *file;

	file = fopen(sFilePath.c_str(), "rb");
	if (!file)
		return false;

	fread(str, 10, 1, file);
	if (memcmp(str, "#?RADIANCE", 10)) {
		fclose(file);
		return false;
	}

	fseek(file, 1, SEEK_CUR);

	i = 0;
	char c = 0, oldc;
	while(true) {
		oldc = c;
		c = fgetc(file);
		if (c == 0xa && oldc == 0xa)
			break;
	}

	char reso[200];
	i = 0;
	while(true) {
		c = fgetc(file);
		reso[i++] = c;
		if (c == 0xa)
			break;
	}

	int w, h;
	if (!sscanf(reso, "-Y %d +X %d", &h, &w)) {
		fclose(file);
		return false;
	}

	if (!outImage.CreateEmptyImage(w, h, PIXELFORMAT::RGB32F)) {
		fclose(file);
		return false;
	}

	float* cols = reinterpret_cast<float*>(outImage.GetPointerToBuffer());

	RGBE *scanline = new RGBE[w];
	if (!scanline) {
		fclose(file);
		return false;
	}

	// convert image 
	for (int y = h - 1; y >= 0; y--) {
		if (decrunch(scanline, w, file) == false)
			break;
		workOnRGBE(scanline, w, cols);
		cols += w * 3;
	}

	delete [] scanline;
	fclose(file);



	/*cols = reinterpret_cast<float*>(outImage.GetPointerToBuffer());
	const size_t n = 3 * size_t(w) * size_t(h);
	std::cout<<"dimensions "<<w<<"x"<<h<<", floats "<<n<<std::endl;
	for (size_t i = 0; i < n; i += 3) {
		cols[i] = 0.0f;
		//cols[i + 1] = 255;
		//cols[i + 2] = 255;
	}*/

	return true;
}

float convertComponent(int expo, int val)
{
	float v = val / 256.0f;
	float d = (float) pow(2, expo);
	return v * d;
}

void workOnRGBE(RGBE *scan, int len, float *cols)
{
	while (len-- > 0) {
		int expo = scan[0][E] - 128;
		cols[0] = convertComponent(expo, scan[0][R]);
		cols[1] = convertComponent(expo, scan[0][G]);
		cols[2] = convertComponent(expo, scan[0][B]);
		cols += 3;
		scan++;
	}
}

bool decrunch(RGBE *scanline, int len, FILE *file)
{
	int  i, j;
					
	if (len < MINELEN || len > MAXELEN)
		return oldDecrunch(scanline, len, file);

	i = fgetc(file);
	if (i != 2) {
		fseek(file, -1, SEEK_CUR);
		return oldDecrunch(scanline, len, file);
	}

	scanline[0][G] = fgetc(file);
	scanline[0][B] = fgetc(file);
	i = fgetc(file);

	if (scanline[0][G] != 2 || scanline[0][B] & 128) {
		scanline[0][R] = 2;
		scanline[0][E] = i;
		return oldDecrunch(scanline + 1, len - 1, file);
	}

	// read each component
	for (i = 0; i < 4; i++) {
	    for (j = 0; j < len; ) {
			unsigned char code = fgetc(file);
			if (code > 128) { // run
			    code &= 127;
			    unsigned char val = fgetc(file);
			    while (code--)
					scanline[j++][i] = val;
			}
			else  {	// non-run
			    while(code--)
					scanline[j++][i] = fgetc(file);
			}
		}
    }

	return feof(file) ? false : true;
}

bool oldDecrunch(RGBE *scanline, int len, FILE *file)
{
	int i;
	int rshift = 0;
	
	while (len > 0) {
		scanline[0][R] = fgetc(file);
		scanline[0][G] = fgetc(file);
		scanline[0][B] = fgetc(file);
		scanline[0][E] = fgetc(file);
		if (feof(file))
			return false;

		if (scanline[0][R] == 1 &&
			scanline[0][G] == 1 &&
			scanline[0][B] == 1) {
			for (i = scanline[0][E] << rshift; i > 0; i--) {
				memcpy(&scanline[0][0], &scanline[-1][0], 4);
				scanline++;
				len--;
			}
			rshift += 8;
		}
		else {
			scanline++;
			len--;
			rshift = 0;
		}
	}
	return true;
}
 
}
