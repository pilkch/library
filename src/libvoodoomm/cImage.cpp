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

// SDL headers
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

// libvoodoomm headers
#include <libvoodoomm/cImage.h>

#ifdef PLATFORM_LINUX_OR_UNIX
#include <sys/stat.h>
#endif

namespace voodoo
{
#ifdef PLATFORM_LINUX_OR_UNIX
  bool FileExists(const voodoo::string_t& strFilename)
  {
    struct stat statInfo;

    int iResult = stat(voodoo::string::ToUTF8(strFilename).c_str(), &statInfo);

    return (iResult == 0);
  }
#endif




  class cSurface
  {
  public:
    cSurface();
    ~cSurface();

    //cSurface(const cSurface& rhs);
    //cSurface& operator=(const cSurface& rhs);

    bool IsSurfaceValid() const { return (pSurface != nullptr); }

    size_t GetWidth() const { return image.GetWidth(); }
    size_t GetHeight() const { return image.GetHeight(); }
    PIXELFORMAT GetPixelFormat() const { return image.GetPixelFormat(); }
    size_t GetBytesPerPixel() const { return image.GetBytesPerPixel(); }
    IMAGE_TYPE GetType() const { return type; }

    void Destroy();

    bool CreateFromImage(const cImage& image);
    bool LoadFromFile(const string_t& sFilename);

    const uint8_t* GetPointerToBuffer() const;
    const uint8_t* GetPointerToSurfacePixelBuffer() const;

    const cImage& GetImage() const;

    void CopyFromBufferToSurface();

    void CopyFromSurfaceToBuffer(size_t width, size_t height);
    void CopyFromSurfaceToBuffer();

    bool SaveToBMP(const string_t& sFilename) const;

  private:
    void Assign(const cSurface& rhs);

    bool IsSameFormat(const cSurface& rhs) const;

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
      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }
  }

  /*cSurface& cSurface::operator=(const cSurface& rhs)
  {
    type = rhs.type;

    image = rhs.image;
    SDL_Surface* pSurface;
  }*/

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
    // Only RGBA textures are supported at the moment
    assert(_image.GetPixelFormat() == PIXELFORMAT::R8G8B8A8);

    // Only width and heights divisible by 2 are supported at the moment
    assert(spitfire::math::IsDivisibleByTwo(_image.GetWidth()));
    assert(spitfire::math::IsDivisibleByTwo(_image.GetHeight()));

    // Only square textures are supported at the moment
    assert(_image.GetWidth() == _image.GetHeight());

    image = _image;

    return true;
  }

  bool cSurface::LoadFromFile(const string_t& sFilename)
  {
    std::cout<<"cSurface::LoadFromFile \""<<string::ToUTF8(sFilename)<<"\""<<std::endl;

    Destroy();

    //unsigned int mode = 0;
    pSurface = IMG_Load(string::ToUTF8(sFilename).c_str());

    // Could not load filename
    if (pSurface == nullptr) {
      if (FileExists(sFilename)) std::cout<<"cSurface::LoadFromFile Texture "<<string::ToUTF8(sFilename)<<" exists"<<std::endl;
      else std::cout<<"cSurface::LoadFromFile Texture "<<string::ToUTF8(sFilename)<<" doesn't exist"<<std::endl;

      std::cout<<"cSurface::LoadFromFile Couldn't Load Texture "<<string::ToUTF8(sFilename)<<", returning false"<<std::endl;
      return false;
    }

    // Check the format
    if (8 == pSurface->format->BitsPerPixel) {
      std::cout<<"cSurface::LoadFromFile Texture Greyscale Heightmap Image "<<string::ToUTF8(sFilename)<<std::endl;
      type = IMAGE_TYPE::HEIGHTMAP;
    } else if (16 == pSurface->format->BitsPerPixel) {
      std::cout<<"cSurface::LoadFromFile Greyscale Heightmap Image "<<string::ToUTF8(sFilename)<<std::endl;
      type = IMAGE_TYPE::HEIGHTMAP;
    } else if (24 == pSurface->format->BitsPerPixel) {
      std::cout<<"cSurface::LoadFromFile "<<string::ToUTF8(sFilename)<<" is a 24 bit RGB image"<<std::endl;
      // Add alpha channel
      SDL_PixelFormat format = {
        NULL, 32, 4, 0, 0, 0, 0,
        0, 8, 16, 24,
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
        0, 255
      };
      SDL_Surface* pConvertedSurface = SDL_ConvertSurface(pSurface, &format, SDL_SWSURFACE);
      SDL_FreeSurface(pSurface);
      pSurface = pConvertedSurface;

      // The image has now been converted to RGBA
      type = IMAGE_TYPE::BITMAP;
      image.pixelFormat = PIXELFORMAT::R8G8B8A8;
    } else if (32 == pSurface->format->BitsPerPixel) {
      std::cout<<"cSurface::LoadFromFile "<<string::ToUTF8(sFilename)<<" is a 32 bit RGBA image"<<std::endl;

      // Convert if BGR
      if (pSurface->format->Rshift > pSurface->format->Bshift) {
        SDL_PixelFormat format = {
          NULL, 32, 4, 0, 0, 0, 0,
          0, 8, 16, 24,
          0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
          0, 255};
        SDL_Surface* pConvertedSurface = SDL_ConvertSurface(pSurface, &format, SDL_SWSURFACE);
        SDL_FreeSurface(pSurface);
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
      t << pSurface->format->BitsPerPixel;
      std::cout<<"cSurface::LoadFromFile Error Unknown Image Format ("<<t.str()<<"bit) "<<string::ToUTF8(sFilename)<<", returning false"<<std::endl;
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

    image.CreateFromBuffer(GetPointerToSurfacePixelBuffer(), width, height, PIXELFORMAT::R8G8B8A8);
  }

  void cSurface::CopyFromBufferToSurface()
  {
    // Only RGBA is supported at the moment
    assert(image.GetPixelFormat() == PIXELFORMAT::R8G8B8A8);

    if (!image.IsValid()) return;

    const size_t width = image.GetWidth();
    const size_t height = image.GetHeight();
    const size_t bytesPerRow = width * GetBytesPerPixel();
    const size_t n = bytesPerRow * height;

    if (!IsSurfaceValid()) {
      // Load the buffer into a surface
      const size_t depth = 32;
      const size_t pitch = bytesPerRow;

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
      pSurface = SDL_CreateRGBSurfaceFrom((void*)pBuffer, width, height, depth, pitch, rmask, gmask, bmask, amask);
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

  bool cSurface::SaveToBMP(const string_t& sFilename) const
  {
    assert(IsSurfaceValid());
    SDL_SaveBMP(pSurface, string::ToUTF8(sFilename).c_str());
    return true;
  }



  // ** cImage

  cImage::cImage() :
    pixelFormat(PIXELFORMAT::R8G8B8A8)
  {
  }

  cImage::~cImage()
  {
    buffer.clear();
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
    //return (type == IMAGE_TYPE::HEIGHTMAP ? 1 : 4);
    assert(pixelFormat == PIXELFORMAT::R8G8B8A8);
    return 4;
  }

  const uint8_t* cImage::GetPointerToBuffer() const
  {
    assert(!buffer.empty());
    return buffer.data();
  }

  bool cImage::LoadFromFile(const string_t& sFilename)
  {
    cSurface surface;
    if (!surface.LoadFromFile(sFilename)) {
      std::cout<<"cImage::LoadFromFile Failed to load file \""<<string::ToUTF8(sFilename)<<"\""<<std::endl;
      return false;
    }

    *this = surface.GetImage();

    return true;
  }

  bool cImage::CreateEmptyImage(size_t _width, size_t _height, PIXELFORMAT _pixelFormat)
  {
    std::cout<<"cImage::CreateEmptyImage "<<_width<<"x"<<_height<<std::endl;

    // Only RGBA is supported at the moment
    assert(_pixelFormat == PIXELFORMAT::R8G8B8A8);

    width = _width;
    height = _height;
    pixelFormat = _pixelFormat;

    const size_t n = _width * _height;
    buffer.resize(n, 0);
    FillBlack();

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

    // Only RGBA is supported at the moment
    assert(_pixelFormat == PIXELFORMAT::R8G8B8A8);

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
    return surface.SaveToBMP(sFilename);
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

  void cImage::FlipVertically()
  {
    // Only RGBA is supported at the moment
    assert(pixelFormat == PIXELFORMAT::R8G8B8A8);

    if (buffer.empty()) return;

    // For each row swap it with the corresponding row on the other side of the image
    const size_t nBytesPerRow = GetBytesPerPixel() * width;
    assert(buffer.size() == nBytesPerRow * height);

    uint8_t tempBuffer[nBytesPerRow];
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

    // This has not been implemented yet
    assert(false);

    /*// For each column swap it with the corresponding column on the other side of the image
    const size_t nBytesPerRow = GetBytesPerPixel() * width;
    const size_t halfWidth = width / 2;
    for (size_t y = 0; y < height; y++) {
      std::memcpy(&buffer[(y * nBytesPerRow)], &buffer[(nBytesPerRow * height) - (y * nBytesPerRow)], nBytesPerRow);
    }*/
  }
}
