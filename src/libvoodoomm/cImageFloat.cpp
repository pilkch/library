// Standard headers
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

// Spitfire headers
#include <spitfire/spitfire.h>

// libvoodoomm headers
#include <libvoodoomm/cImageFloat.h>

#ifdef PLATFORM_LINUX_OR_UNIX
#include <sys/stat.h>
#endif

namespace voodoo
{
  #ifdef PLATFORM_LINUX_OR_UNIX
  bool FileExists(const voodoo::string_t& strFilename);
  #endif

  string_t GetFileExtension(const string_t& sFilePath);


  // ** PFM reader and writer
  // Floating point format stored as either greyscale or RGB channels in 32 bit floats
  // http://netpbm.sourceforge.net/doc/pfm.html
  // http://barnesc.blogspot.com.au/2007/04/pfm-readwrite-1-and-3-channel-pfm-files.html

  bool read_pfm_file3(const char* filename, std::vector<float>& buffer, size_t& width, size_t& height)
  {
    char buf[256];
    FILE* f = fopen(filename, "rb");
    int result = fscanf(f, "%s\n", buf);
    if (result != 1) {
      std::cerr<<"read_pfm_file3 fscanf FAILED result="<<result<<std::endl;
      return false;
    }
    if (strcmp(buf, "PF") != 0) {
      std::cerr<<"read_pfm_file3 Not a 3 channel PFM file."<<std::endl;
      return false;
    }
    int w = 0;
    int h = 0;
    result = fscanf(f, "%d %d\n", &w, &h);
    if (result != 2) {
      std::cerr<<"read_pfm_file3 fscanf FAILED result="<<result<<std::endl;
      return false;
    }
    width = w;
    height = h;

    double scale = 1.0;
    result = fscanf(f, "%lf\n", &scale);
    if (result != 1) {
      std::cerr<<"read_pfm_file3 fscanf FAILED result="<<result<<std::endl;
      return false;
    }
    bool bIsFileLittleEndian = false;
    if (scale < 0.0) {
      bIsFileLittleEndian = true;
      scale = -scale;
    }
    int channels = 3;
    uint8_t* data = new uint8_t[width * height * sizeof(float) * channels];
    int count = fread((void*)data, sizeof(float), width * height * channels, f);
    if (count != int(width * height * channels)) {
      std::cerr<<"read_pfm_file3 Error reading PFM file "<<count<<" "<<width<<"x"<<height<<" "<<channels<<std::endl;
      delete [] data;
      return false;
    }
    #ifdef BUILD_ENDIAN_LITTLE
    const bool bIsLittleEndian = true;
    #else
    const bool bIsLittleEndian = false;
    #endif
    buffer.resize(width * height * channels, 0);
    for (size_t i = 0; i < width * height * channels; i++) {
      uint8_t* p = &data[i * sizeof(float)];
      if (bIsFileLittleEndian != bIsLittleEndian) {
        uint8_t temp;
        temp = p[0]; p[0] = p[3]; p[3] = temp;
        temp = p[1]; p[1] = p[2]; p[2] = temp;
      }
      buffer[i] = *((float*)p);
    }
    fclose(f);
    delete[] data;

    return true;
  }

  bool write_pfm_file3(const char* filename, const float* buffer, size_t width, size_t height)
  {
    FILE* f = fopen(filename, "wb");

    const double scale = -1.0;

    fprintf(f, "PF\n");
    fprintf(f, "%d %d\n", int(width), int(height));
    fprintf(f, "%lf\n", scale);

    int channels = 3;
    for (size_t i = 0; i < width * height * channels; i++) {
      float d = buffer[i];
      fwrite((void*)&d, 1, sizeof(float), f);
    }
    fclose(f);

    return true;
  }

  bool read_pfm_file(const char* filename, std::vector<float>& buffer, size_t& width, size_t& height)
  {
    char buf[256];
    FILE* f = fopen(filename, "rb");
    int result = fscanf(f, "%s\n", buf);
    if (result != 1) {
      std::cerr<<"read_pfm_file fscanf FAILED result="<<result<<std::endl;
      return false;
    }
    if (strcmp(buf, "Pf") != 0) {
      std::cerr<<"read_pfm_file Not a 1 channel PFM file."<<std::endl;
      return false;
    }
    int w = 0;
    int h = 0;
    result = fscanf(f, "%d %d\n", &w, &h);
    if (result != 2) {
      std::cerr<<"read_pfm_file fscanf FAILED result="<<result<<std::endl;
      return false;
    }
    width = w;
    height = h;

    double scale = 1.0;
    result = fscanf(f, "%lf\n", &scale);
    if (result != 1) {
      std::cerr<<"read_pfm_file fscanf FAILED result="<<result<<std::endl;
      return false;
    }
    bool bIsFileLittleEndian = false;
    if (scale < 0.0) {
      bIsFileLittleEndian = true;
      scale = -scale;
    }
    uint8_t* data = new uint8_t[width * height * sizeof(float)];
    int count = fread((void*)data, sizeof(float), width * height, f);
    if (count != int(width * height)) {
      std::cerr<<"read_pfm_file Error reading PFM file "<<count<<" "<<width<<"x"<<height<<std::endl;
      delete [] data;
      return false;
    }
    #ifdef BUILD_ENDIAN_LITTLE
    const bool bIsLittleEndian = true;
    #else
    const bool bIsLittleEndian = false;
    #endif
    buffer.resize(width * height, 0);
    for (size_t i = 0; i < width * height; i++) {
      uint8_t* p = &data[i * sizeof(float)];
      if (bIsFileLittleEndian != bIsLittleEndian) {
        uint8_t temp;
        temp = p[0]; p[0] = p[3]; p[3] = temp;
        temp = p[1]; p[1] = p[2]; p[2] = temp;
      }
      buffer[i] = *((float*)p);
    }
    fclose(f);
    delete[] data;

    return true;
  }

  bool write_pfm_file(const char* filename, const float* buffer, size_t width, size_t height)
  {
    FILE* f = fopen(filename, "wb");

    const double scale = -1.0;

    fprintf(f, "Pf\n");
    fprintf(f, "%d %d\n", int(width), int(height));
    fprintf(f, "%lf\n", scale);

    for (size_t i = 0; i < width * height; i++) {
      float d = buffer[i];
      fwrite((void*)&d, 1, sizeof(float), f);
    }
    fclose(f);

    return true;
  }


  // ** cImageFloat

  cImageFloat::cImageFloat() :
    pixelFormat(PIXELFORMAT::H32)
  {
  }

  cImageFloat::~cImageFloat()
  {
    buffer.clear();
  }

  cImageFloat::cImageFloat(const cImageFloat& rhs)
  {
    Assign(rhs);
  }

  cImageFloat& cImageFloat::operator=(const cImageFloat& rhs)
  {
    Assign(rhs);
    return *this;
  }

  void cImageFloat::Assign(const cImageFloat& rhs)
  {
    width = rhs.width;
    height = rhs.height;

    pixelFormat = rhs.pixelFormat;

    buffer = rhs.buffer;
  }

  bool cImageFloat::IsSameFormat(const cImageFloat& rhs) const
  {
    return (width == rhs.width) && (height == rhs.height) && (pixelFormat == rhs.pixelFormat);
  }

  size_t cImageFloat::GetBytesPerPixel() const
  {
    switch (pixelFormat) {
      case PIXELFORMAT::H8: return 1;
      case PIXELFORMAT::H16: return 2;
    };
    assert(pixelFormat == PIXELFORMAT::H32);
    return 4;
  }

  const float* cImageFloat::GetPointerToBuffer() const
  {
    assert(!buffer.empty());
    return buffer.data();
  }

  bool cImageFloat::IsLoadingSupported(const string_t& sFilePath)
  {
    const string_t sExtension = GetFileExtension(sFilePath);

    // Check the supported formats
    bool bIsSupported = (
      (sExtension == TEXT("pfm"))
    );

    return bIsSupported;
  }

  bool cImageFloat::LoadFromFile(const string_t& sFilePath)
  {
    const string_t sExtension = GetFileExtension(sFilePath);
    if (sExtension == TEXT("pfm")) {
      const bool bResult = read_pfm_file(string::ToUTF8(sFilePath).c_str(), buffer, width, height);
      pixelFormat = PIXELFORMAT::H32;
      return bResult;
    }

    return false;
  }

  bool cImageFloat::SaveToPFM(const string_t& sFilePath) const
  {
    return write_pfm_file(string::ToUTF8(sFilePath).c_str(), GetPointerToBuffer(), width, height);
  }
}
