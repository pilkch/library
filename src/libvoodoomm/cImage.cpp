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

// stb headers
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// SDL headers
#include <SDL3_image/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

// libvoodoomm headers
#include <libvoodoomm/cImage.h>
#include <libvoodoomm/hdr.h>

#ifdef PLATFORM_LINUX_OR_UNIX
#include <sys/stat.h>
#endif

namespace voodoo
{
  #ifdef __WIN__
  bool FileExists(const voodoo::string_t& sFilePath)
  {
    DWORD dwAttrib = GetFileAttributes(sFilePath.c_str());

    return ((dwAttrib != INVALID_FILE_ATTRIBUTES) && ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0));
  }
  #elif defined(PLATFORM_LINUX_OR_UNIX)
  bool FileExists(const voodoo::string_t& strFilename)
  {
    struct stat statInfo;

    int iResult = stat(voodoo::string::ToUTF8(strFilename).c_str(), &statInfo);

    return (iResult == 0);
  }
  #endif

  string_t GetFileExtension(const string_t& sFilePath)
  {
    const string_t::size_type found = sFilePath.find(TEXT("."));
    if (found != string_t::npos) return sFilePath.substr(found + 1);

    return TEXT("");
  }



  class cSurface
  {
  public:
    cSurface();
    ~cSurface();

    //cSurface(const cSurface& rhs);
    //cSurface& operator=(const cSurface& rhs);

    constexpr bool IsSurfaceValid() const { return (pSurface != nullptr); }

    inline size_t GetWidth() const { return image.GetWidth(); }
    inline size_t GetHeight() const { return image.GetHeight(); }
    inline PIXELFORMAT GetPixelFormat() const { return image.GetPixelFormat(); }
    inline size_t GetBytesPerPixel() const { return image.GetBytesPerPixel(); }
    inline IMAGE_TYPE GetType() const { return type; }

    void Destroy();

    bool CreateFromImage(const cImage& image);

    bool LoadFromFile(const string_t& sFilePath);
    bool SaveToBMP(const string_t& sFilePath) const;
    bool SaveToPNG(const string_t& sFilePath) const;

    const uint8_t* GetPointerToBuffer() const;
    const uint8_t* GetPointerToSurfacePixelBuffer() const;

    const cImage& GetImage() const;

    void CopyFromBufferToSurface();

    void CopyFromSurfaceToBuffer(size_t width, size_t height);
    void CopyFromSurfaceToBuffer();

    void ConvertARGBToRGBA();

  private:
    void Assign(const cSurface& rhs);

    IMAGE_TYPE type;

    cImage image;
    SDL_Surface* pSurface;
  };

  cSurface::cSurface() :
    type(IMAGE_TYPE::BITMAP),
    pSurface(nullptr)
  {
  }

  cSurface::~cSurface()
  {
    Destroy();
  }

  void cSurface::Destroy()
  {
    if (pSurface != nullptr) {
      SDL_DestroySurface(pSurface);
      pSurface = nullptr;
    }
  }

  const uint8_t* cSurface::GetPointerToBuffer() const
  {
    return image.GetPointerToBuffer();
  }

  const uint8_t* cSurface::GetPointerToSurfacePixelBuffer() const
  {
    assert(IsSurfaceValid());
    return static_cast<uint8_t*>(pSurface->pixels);
  }

  const cImage& cSurface::GetImage() const
  {
    return image;
  }

  bool cSurface::CreateFromImage(const cImage& _image)
  {
    // Only RGB and RGBA textures are supported at the moment
    assert((_image.GetPixelFormat() == PIXELFORMAT::R8G8B8) || (_image.GetPixelFormat() == PIXELFORMAT::R8G8B8A8));

    image = _image;

    return true;
  }

  bool cSurface::LoadFromFile(const string_t& sFilePath)
  {
    std::cout<<"cSurface::LoadFromFile \""<<string::ToUTF8(sFilePath)<<"\""<<std::endl;

    Destroy();

    //unsigned int mode = 0;
    pSurface = IMG_Load(string::ToUTF8(sFilePath).c_str());

    // Could not load filename
    if (pSurface == nullptr) {
      if (FileExists(sFilePath)) std::cout<<"cSurface::LoadFromFile Texture "<<string::ToUTF8(sFilePath)<<" exists"<<std::endl;
      else std::cout<<"cSurface::LoadFromFile Texture "<<string::ToUTF8(sFilePath)<<" doesn't exist"<<std::endl;

      std::cout<<"cSurface::LoadFromFile Couldn't Load Texture "<<string::ToUTF8(sFilePath)<<", returning false"<<std::endl;
      return false;
    }

    // Check the format
    const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(pSurface->format);
    if (8 == details->bits_per_pixel) {
      std::cout<<"cSurface::LoadFromFile Texture Greyscale Heightmap Image "<<string::ToUTF8(sFilePath)<<std::endl;
      type = IMAGE_TYPE::HEIGHTMAP;
      image.pixelFormat = PIXELFORMAT::H8;
    } else if (16 == details->bits_per_pixel) {
      std::cout<<"cSurface::LoadFromFile Greyscale Heightmap Image "<<string::ToUTF8(sFilePath)<<std::endl;
      type = IMAGE_TYPE::HEIGHTMAP;
      image.pixelFormat = PIXELFORMAT::H16;
    } else if (24 == details->bits_per_pixel) {
      std::cout<<"cSurface::LoadFromFile "<<string::ToUTF8(sFilePath)<<" is a 24 bit RGB image"<<std::endl;
      type = IMAGE_TYPE::BITMAP;
      image.pixelFormat = PIXELFORMAT::R8G8B8;
    } else if (32 == details->bits_per_pixel) {
      std::cout<<"cSurface::LoadFromFile "<<string::ToUTF8(sFilePath)<<" is a 32 bit RGBA image"<<std::endl;

      // Convert if BGR
      if (details->Rshift > details->Bshift) {
        SDL_Surface* pConvertedSurface = SDL_ConvertSurface(pSurface, SDL_PIXELFORMAT_RGBA8888);
        SDL_DestroySurface(pSurface);

        pSurface = pConvertedSurface;
      }

      /*int nHH = pSurface->h / 2;
      int nPitch = pSurface->pitch;

      unsigned char* pBuf = new unsigned char[nPitch];
      unsigned char* pSrc = (unsigned char*)pSurface->pixels;
      unsigned char* pDst = (unsigned char*)pSurface->pixels + nPitch * (pSurface->h - 1);

      while (nHH--) {
        std::memcpy(pBuf, pSrc, nPitch);
        std::memcpy(pSrc, pDst, nPitch);
        std::memcpy(pDst, pBuf, nPitch);

        pSrc += nPitch;
        pDst -= nPitch;
      };

      SAFE_DELETE_ARRAY(pBuf);*/

      type = IMAGE_TYPE::BITMAP;
      image.pixelFormat = PIXELFORMAT::R8G8B8A8;
    } else {
      std::ostringstream t;
      t << details->bits_per_pixel;
      std::cout<<"cSurface::LoadFromFile Error Unknown Image Format ("<<t.str()<<"bit) "<<string::ToUTF8(sFilePath)<<", returning false"<<std::endl;
      return false;
    }

    image.width = pSurface->w;
    image.height = pSurface->h;

    std::cout<<"cSurface::LoadFromFile "<<image.width<<"x"<<image.height<<std::endl;

    CopyFromSurfaceToBuffer();

    return true;
  }

  void cSurface::CopyFromSurfaceToBuffer()
  {
    assert(IsSurfaceValid());

    const size_t width = pSurface->w;
    const size_t height = pSurface->h;

    CopyFromSurfaceToBuffer(width, height);
  }

  void cSurface::CopyFromSurfaceToBuffer(size_t width, size_t height)
  {
    assert(IsSurfaceValid());

    image.CreateFromBuffer(GetPointerToSurfacePixelBuffer(), width, height, image.pixelFormat);
  }

  void cSurface::CopyFromBufferToSurface()
  {
    // Only RGB and RGBA are supported at the moment
    assert((image.GetPixelFormat() == PIXELFORMAT::R8G8B8) || (image.GetPixelFormat() == PIXELFORMAT::R8G8B8A8));

    if (!image.IsValid()) return;

    const size_t width = image.GetWidth();
    const size_t height = image.GetHeight();
    const size_t bytesPerRow = width * GetBytesPerPixel();
    const size_t n = bytesPerRow * height;

    if (!IsSurfaceValid()) {
      switch (image.GetPixelFormat()) {
        case PIXELFORMAT::R8G8B8: {
          // Load the buffer into a surface
          const int pitch = int(bytesPerRow);

          const uint8_t* pBuffer = image.GetPointerToBuffer();
          pSurface = SDL_CreateSurfaceFrom(int(width), int(height), SDL_PIXELFORMAT_RGB24, (void*)pBuffer, pitch);
          break;
        }
        case PIXELFORMAT::R8G8B8A8: {
          // Load the buffer into a surface
          // TODO: We can probably do something like RGB24 above?
          const int depth = 32;
          const int pitch = int(bytesPerRow);

          // SDL interprets each pixel as a 32-bit number, so our masks must depend on the endianness (byte order) of the machine
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
          uint32_t rmask = 0xFF000000;
          uint32_t gmask = 0x00FF0000;
          uint32_t bmask = 0x0000FF00;
          uint32_t amask = 0x000000FF;
    #else
          uint32_t rmask = 0x000000FF;
          uint32_t gmask = 0x0000FF00;
          uint32_t bmask = 0x00FF0000;
          uint32_t amask = 0xFF000000;
    #endif

          const uint8_t* pBuffer = image.GetPointerToBuffer();
          pSurface = SDL_CreateSurfaceFrom(int(width), int(height), SDL_GetPixelFormatForMasks(depth, rmask, gmask, bmask, amask), (void*)pBuffer, pitch);
        }
      }
    } else {
      const uint8_t* pBuffer = image.GetPointerToBuffer();
      std::memcpy(pSurface->pixels, pBuffer, n);
    }
  }

  /*bool cSurface::CopyToImage(cImage& image)
  {
    assert(IsSurfaceValid());

    const uint8_t* pBuffer = buffer.get();
    image.CreateFromBuffer(pBuffer, width, height, pixelFormat);
  }*/

  void cSurface::ConvertARGBToRGBA()
  {
    ASSERT(pSurface != nullptr);

    const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(pSurface->format);
    ASSERT(details->bits_per_pixel == 32);

    const size_t height = pSurface->h;
    const size_t nPitchBytes = pSurface->pitch;

    unsigned char* pBuf = static_cast<unsigned char*>(pSurface->pixels);

    // Iterate through each pixel
    for (size_t row = 0; row < height; row++) {
      for (size_t i = 0; i < nPitchBytes; i += 4) {
        // Swap the components of this pixel
        std::swap(pBuf[i], pBuf[i + 3]);
        std::swap(pBuf[i + 1], pBuf[i + 2]);
      }

      // Skip to the new row
      pBuf += nPitchBytes;
    };
  }

  bool cSurface::SaveToBMP(const string_t& sFilePath) const
  {
    assert(IsSurfaceValid());
    return SDL_SaveBMP(pSurface, string::ToUTF8(sFilePath).c_str());
  }

  bool cSurface::SaveToPNG(const string_t& sFilePath) const
  {
    assert(IsSurfaceValid());
    switch (image.GetPixelFormat()) {
      case PIXELFORMAT::R8G8B8A8: {
        // NOTE: IMG_SavePNG *always* outputs an RGBA image
        return IMG_SavePNG(pSurface, string::ToUTF8(sFilePath).c_str());
      }
      case PIXELFORMAT::R8G8B8: {
        const size_t width = pSurface->w;
        const size_t height = pSurface->h;
        const size_t nChannels = image.GetBytesPerPixel();
        const uint8_t* pBuffer = image.GetPointerToBuffer();
        stbi_write_png(sFilePath.c_str(), width, height, nChannels, pBuffer, width * nChannels);
        return true;
      }
    }

    return false;
  }


  // ** cImage

  cImage::cImage() :
    pixelFormat(PIXELFORMAT::R8G8B8A8)
  {
  }

  cImage::cImage(const cImage& rhs)
  {
    Assign(rhs);
  }

  cImage& cImage::operator=(const cImage& rhs)
  {
    Assign(rhs);
    return *this;
  }

  void cImage::Assign(const cImage& rhs)
  {
    width = rhs.width;
    height = rhs.height;

    pixelFormat = rhs.pixelFormat;

    buffer = rhs.buffer;
  }

  bool cImage::IsSameFormat(const cImage& rhs) const
  {
    return (width == rhs.width) && (height == rhs.height) && (pixelFormat == rhs.pixelFormat);
  }

  size_t cImage::GetBytesPerPixel() const
  {
    return GetBytesForPixelFormat(pixelFormat);
  }

  size_t cImage::GetBitsPerPixel() const
  {
    return GetBitsForPixelFormat(pixelFormat);
  }

  size_t cImage::GetBytesPerRow() const
  {
    return (width * GetBytesPerPixel());
  }

  const uint8_t* cImage::GetPointerToBuffer() const
  {
    assert(!buffer.empty());
    return buffer.data();
  }

  uint8_t* cImage::GetPointerToBuffer()
  {
    assert(!buffer.empty());
    return buffer.data();
  }

  bool cImage::IsLoadingSupported(const string_t& sFilePath)
  {
    const string_t sExtension = GetFileExtension(sFilePath);

    const bool bIsSupportedNative = (
      (sExtension == TEXT("hdr"))
    );

    if (bIsSupportedNative) {
      return true;
    }


    // Check the built in supported formats
    bool bIsSupportedSDL = (
      (sExtension == TEXT("bmp")) ||
      (sExtension == TEXT("cur")) ||
      (sExtension == TEXT("gif")) ||
      (sExtension == TEXT("lbm")) ||
      (sExtension == TEXT("pcx")) ||
      (sExtension == TEXT("pnm")) ||
      (sExtension == TEXT("tga")) ||
      (sExtension == TEXT("xcf")) ||
      (sExtension == TEXT("xpm")) ||
      (sExtension == TEXT("xv"))
    );

    // Check the dynamically supported formats
    if (!bIsSupportedSDL) {
      SDL_IOStream* pIOStream = SDL_IOFromFile(string::ToUTF8(sFilePath).c_str(), "rb");
      if (pIOStream != nullptr) {
        bIsSupportedSDL = (IMG_isJPG(pIOStream) || IMG_isTIF(pIOStream) || IMG_isPNG(pIOStream)); // NOTE: In later versions of SDL there is also IMG_isWEBP

        SDL_CloseIO(pIOStream);
        pIOStream = nullptr;
      }
    }

    return (bIsSupportedNative || bIsSupportedSDL);
  }

  bool cImage::LoadFromFile(const string_t& sFilePath)
  {
    const string_t sExtension = GetFileExtension(sFilePath);
    if (sExtension == TEXT("hdr")) {
      if (LoadHDR(sFilePath, *this)) {
        return true;
      }
    }

    cSurface surface;
    if (!surface.LoadFromFile(sFilePath)) {
      std::cout<<"cImage::LoadFromFile Failed to load file \""<<string::ToUTF8(sFilePath)<<"\""<<std::endl;
      return false;
    }

    *this = surface.GetImage();

    return true;
  }

  bool cImage::CreateEmptyImage(size_t _width, size_t _height, PIXELFORMAT _pixelFormat)
  {
    std::cout<<"cImage::CreateEmptyImage "<<_width<<"x"<<_height<<std::endl;

    width = _width;
    height = _height;
    pixelFormat = _pixelFormat;

    const size_t n = height * GetBytesPerRow();
    buffer.resize(n, 0);

    return true;
  }

  bool cImage::CreateFromImage(const cImage& rhs)
  {
    Assign(rhs);
    return IsValid();
  }

  bool cImage::CreateFromBuffer(const uint8_t* _pBuffer, size_t _width, size_t _height, PIXELFORMAT _pixelFormat)
  {
    std::cout<<"cImage::CreateFromBuffer "<<_width<<"x"<<_height<<std::endl;

    // TODO: Support all formats
    assert((_pixelFormat == PIXELFORMAT::H8) || (_pixelFormat == PIXELFORMAT::R8G8B8) || (_pixelFormat == PIXELFORMAT::R8G8B8A8));

    width = _width;
    height = _height;
    pixelFormat = _pixelFormat;

    const size_t n = width * height * GetBytesPerPixel();
    buffer.resize(n, 0);
    std::memcpy(buffer.data(), _pBuffer, n);

    return true;
  }

  bool cImage::SaveToBMP(const string_t& sFilename) const
  {
    cSurface surface;
    surface.CreateFromImage(*this);
    surface.CopyFromBufferToSurface();
    if (!surface.IsSurfaceValid()) {
      std::cerr<<"cImage::SaveToBMP Surface is invalid, returning false"<<std::endl;
      return false;
    }

    return surface.SaveToBMP(sFilePath);
  }

  bool cImage::SaveToPNG(const string_t& sFilePath) const
  {
    cSurface surface;
    surface.CreateFromImage(*this);
    surface.CopyFromBufferToSurface();
    if (!surface.IsSurfaceValid()) {
      std::cerr<<"cImage::SaveToPNG Surface is invalid, returning false"<<std::endl;
      return false;
    }

    return surface.SaveToPNG(sFilePath);
  }

  void cImage::CreateFromImageHalfSize(const cImage& image)
  {
    // Only RGBA is supported at the moment
    assert(image.pixelFormat == PIXELFORMAT::R8G8B8A8);

    // Only width and heights divisible by 2 are supported at the moment
    assert(spitfire::math::IsDivisibleByTwo(image.width));
    assert(spitfire::math::IsDivisibleByTwo(image.height));

    CreateEmptyImage(image.width / 2, image.height / 2, image.pixelFormat);

    const size_t widthSource = image.width;
    //const size_t heightSource = image.height;

    const size_t widthDestination = width;
    const size_t heightDestination = height;

    for (size_t y = 0; y < heightDestination; y++) {
      for (size_t x = 0; x < widthDestination; x++) {
        const size_t indexSource = (2 * ((y * widthSource) + x)) * 4;
        const size_t indexDestination = ((y * widthDestination) + x) * 4;

        buffer[indexSource] = image.buffer[indexDestination]; // Red
        buffer[indexSource + 1] = image.buffer[indexDestination + 1]; // Green
        buffer[indexSource + 2] = image.buffer[indexDestination + 2]; // Blue
        buffer[indexSource + 3] = image.buffer[indexDestination + 3]; // Alpha
      }
    }
  }

  void cImage::CreateFromImageDoubleSize(const cImage& image)
  {
    // Only RGBA is supported at the moment
    assert(image.pixelFormat == PIXELFORMAT::R8G8B8A8);

    CreateEmptyImage(image.width * 2, image.height * 2, image.pixelFormat);

    const size_t widthSource = image.width;
    //const size_t heightSource = image.height;

    const size_t widthDestination = width;
    const size_t heightDestination = height;

    for (size_t y = 0; y < heightDestination; y++) {
      for (size_t x = 0; x < widthDestination; x++) {
        const size_t indexSource = ((y * widthSource) + x) * 4;

        // For each component of RGBA
        for (size_t c = 0; c < 4; c++) {
          const uint8_t component = image.buffer[indexSource + c];

          // Fill in a 2x2 area on the destination
          buffer[(((2 * ((y * widthDestination) + x)) + 0) * 4) + c] = component;
          buffer[(((2 * ((y * widthDestination) + x)) + 1) * 4) + c] = component;
          buffer[(((widthDestination + (2 * ((y * widthDestination) + x)) + 0)) * 4) + c] = component;
          buffer[(((widthDestination + (2 * ((y * widthDestination) + x)) + 1)) * 4) + c] = component;
        }
      }
    }
  }

  void cImage::CreateFromImageAndSmooth(const cImage& image, size_t iterations)
  {
    (void)image;
    (void)iterations;

    /*const size_t n = image.width * image.height;

    imageOut.SetDimensions(n);

    std::vector<spitfire::math::cColour> temp = source;

    spitfire::math::cColour surrounding[5];

    for (size_t i = 0; i < iterations; i++) {
      for (size_t y = 0; y < depthLightmap; y++) {
        for (size_t x = 0; x < widthLightmap; x++) {

          surrounding[0] = surrounding[1] = surrounding[2] = surrounding[3] = surrounding[4] = GetLightmapPixel(temp, x, y);

          // We sample from the 4 surrounding pixels in a cross shape
          if (x != 0) surrounding[0] = GetLightmapPixel(temp, x - 1, y);
          if ((y + 1) < depthLightmap) surrounding[1] = GetLightmapPixel(temp, x, y + 1);
          if (y != 0) surrounding[3] = GetLightmapPixel(temp, x, y - 1);
          if ((x + 1) < widthLightmap) surrounding[4] = GetLightmapPixel(temp, x + 1, y);

          //const spitfire::math::cColour averageOfSurrounding = 0.25f * (surrounding[0] + surrounding[1] + surrounding[3] + surrounding[4]);
          //const spitfire::math::cColour final = 0.5f * (surrounding[2] + averageOfSurrounding);
          const spitfire::math::cColour final = 0.25f * (surrounding[0] + surrounding[1] + surrounding[3] + surrounding[4]);

          const size_t index = (y * widthLightmap) + x;

          destination[index] = final;
        }
      }


      // If we are still going then set temp to destination for the next iteration
      if ((i + 1) < iterations) temp = destination;
    }*/
  }

  void cImage::ConvertToNegative()
  {
    const size_t n = width * height * GetBytesPerPixel();
    for (size_t i = 0; i < n; i += 4) {
      buffer[i] = 255 - buffer[i]; // Red
      buffer[i + 1] = 255 - buffer[i + 1]; // Green
      buffer[i + 2] = 255 - buffer[i + 2]; // Blue
    }
  }

  void cImage::FillColour(uint8_t red, uint8_t green, uint8_t blue)
  {
    // Only RGBA is supported at the moment
    assert((pixelFormat == PIXELFORMAT::R8G8B8) || (pixelFormat == PIXELFORMAT::R8G8B8A8));

    const size_t n = width * height * GetBytesPerPixel();
    buffer.resize(n);

    if (pixelFormat == PIXELFORMAT::R8G8B8) {
      for (size_t i = 0; i < n; i += 3) {
        buffer[i] = red; // Red
        buffer[i + 1] = green; // Green
        buffer[i + 2] = blue; // Blue
      }
    } else {
      for (size_t i = 0; i < n; i += 4) {
        buffer[i] = red; // Red
        buffer[i + 1] = green; // Green
        buffer[i + 2] = blue; // Blue
        buffer[i + 3] = 255; // Apha
      }
    }
  }

  void cImage::FillBlack()
  {
    // Only RGBA is supported at the moment
    assert(pixelFormat == PIXELFORMAT::R8G8B8A8);

    const size_t n = width * height * GetBytesPerPixel();
    buffer.resize(n);

    for (size_t i = 0; i < n; i += 4) {
      buffer[i] = 0; // Red
      buffer[i + 1] = 0; // Green
      buffer[i + 2] = 0; // Blue
      buffer[i + 3] = 255; // Apha
    }
  }

  void cImage::FillWhite()
  {
    // Only RGBA is supported at the moment
    assert(pixelFormat == PIXELFORMAT::R8G8B8A8);

    const size_t n = width * height;
    buffer.resize(n);

    for (size_t i = 0; i < n; i += 4) {
      buffer[i] = 255; // Red
      buffer[i + 1] = 255; // Green
      buffer[i + 2] = 255; // Blue
      buffer[i + 3] = 255; // Apha
    }
  }

  void cImage::FillTestPattern()
  {
    // Only RGBA is supported at the moment
    assert(pixelFormat == PIXELFORMAT::R8G8B8A8);

    const size_t n = width * height;
    buffer.resize(n);

    const size_t bytesPerPixel = GetBytesPerPixel();
    const size_t bytesPerRow = GetBytesPerRow();

    uint8_t* pBuffer = buffer.data();

    // Top left, red
    for (size_t y = 0; y < height / 2; y++) {
      for (size_t x = 0; x < width / 2; x++) {
        uint8_t* pPixel = pBuffer + (y * bytesPerRow) + (x * bytesPerPixel);
        pPixel[0] = 0x00;
        pPixel[1] = 0x00;
        pPixel[2] = 0xFF;
        pPixel[3] = 0xFF;
      }
    }

    // Top right, green
    for (size_t y = 0; y < height / 2; y++) {
      for (size_t x = width / 2; x < width; x++) {
        uint8_t* pPixel = pBuffer + (y * bytesPerRow) + (x * bytesPerPixel);
        pPixel[0] = 0x00;
        pPixel[1] = 0xFF;
        pPixel[2] = 0x00;
        pPixel[3] = 0xFF;
      }
    }

    // Bottom left, blue
    for (size_t y = height / 2; y < height; y++) {
      for (size_t x = 0; x < width / 2; x++) {
        uint8_t* pPixel = pBuffer + (y * bytesPerRow) + (x * bytesPerPixel);
        pPixel[0] = 0xFF;
        pPixel[1] = 0x00;
        pPixel[2] = 0x00;
        pPixel[3] = 0xFF;
      }
    }

    // Bottom right, yellow
    for (size_t y = height / 2; y < height; y++) {
      for (size_t x = width / 2; x < width; x++) {
        uint8_t* pPixel = pBuffer + (y * bytesPerRow) + (x * bytesPerPixel);
        pPixel[0] = 0xFF;
        pPixel[1] = 0xFF;
        pPixel[2] = 0x00;
        pPixel[3] = 0xFF;
      }
    }
  }

  void cImage::FlipVertically()
  {
    if (buffer.empty()) return;

    // For each row swap it with the corresponding row on the other side of the image
    const size_t nBytesPerRow = GetBytesPerPixel() * width;
    assert(buffer.size() == nBytesPerRow * height);

    std::vector<uint8_t> tempBuffer(nBytesPerRow);
    const size_t halfHeight = height / 2;
    for (size_t y = 0; y < halfHeight; y++) {
      std::memcpy(&tempBuffer[0], &buffer[(nBytesPerRow * (height - 1)) - (y * nBytesPerRow)], nBytesPerRow);
      std::memcpy(&buffer[(nBytesPerRow * (height - 1)) - (y * nBytesPerRow)], &buffer[(y * nBytesPerRow)], nBytesPerRow);
      std::memcpy(&buffer[(y * nBytesPerRow)], &tempBuffer[0], nBytesPerRow);
    }
  }

  void cImage::FlipHorizontally()
  {
    // Only RGBA is supported at the moment
    assert(pixelFormat == PIXELFORMAT::R8G8B8A8);

    if (buffer.empty()) return;

    // For each column swap it with the corresponding column on the other side of the image
    const size_t nBytesPerRow = GetBytesPerPixel() * width;
    const size_t halfWidth = width / 2;
    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < halfWidth; x++) {
        const size_t indexA = (nBytesPerRow * y) + (4 * x);
        const size_t indexB = (nBytesPerRow * (y + 1)) - (4 * (x + 1));
        std::swap(buffer[indexA], buffer[indexB]);
        std::swap(buffer[indexA + 1], buffer[indexB + 1]);
        std::swap(buffer[indexA + 2], buffer[indexB + 2]);
        std::swap(buffer[indexA + 3], buffer[indexB + 3]);
      }
    }
  }
}
