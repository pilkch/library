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


  // ** cImage

  cImage::cImage() :
    pixelFormat(PIXELFORMAT::R8G8B8A8),
    type(IMAGE_TYPE::BITMAP),

    pSurface(nullptr)
  {
  }

  cImage::~cImage()
  {
    if (pSurface != nullptr) {
      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }

    data.clear();
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
    // Delete our old surface
    if (pSurface != nullptr) {
      SDL_FreeSurface(pSurface);
      pSurface = nullptr;
    }


    width = rhs.width;
    height = rhs.height;

    pixelFormat = rhs.pixelFormat;

    type = rhs.type;

    if (rhs.pSurface != nullptr) {
      // Make a copy of the surface so that we do not modify the original
      pSurface = SDL_ConvertSurface(rhs.pSurface, rhs.pSurface->format, rhs.pSurface->flags);
    }

    data = rhs.data;
  }

  size_t cImage::GetBytesPerPixel() const
  {
    return (type == IMAGE_TYPE::HEIGHTMAP ? 1 : 4);
  }

  const uint8_t* cImage::GetPointerToData() const
  {
    assert(!data.empty());
    return data.data();
  }

  const uint8_t* cImage::GetPointerToSurfacePixelBuffer() const
  {
    assert(pSurface != nullptr);

    return static_cast<const uint8_t*>(pSurface->pixels);
  }

  bool cImage::LoadFromFile(const string_t& sFilename)
  {
    std::cout<<"cImage::LoadFromFile \""<<string::ToUTF8(sFilename)<<"\""<<std::endl;

    //unsigned int mode = 0;
    pSurface = IMG_Load(string::ToUTF8(sFilename).c_str());

    // Could not load filename
    if (pSurface == nullptr) {
      if (FileExists(sFilename)) std::cout<<"cImage::LoadFromFile Texture "<<string::ToUTF8(sFilename)<<" exists"<<std::endl;
      else std::cout<<"cImage::LoadFromFile Texture "<<string::ToUTF8(sFilename)<<" doesn't exist"<<std::endl;

      std::cout<<"cImage::LoadFromFile Couldn't Load Texture "<<string::ToUTF8(sFilename)<<", returning false"<<std::endl;
      return false;
    }



    // Check the format
    if (8 == pSurface->format->BitsPerPixel) {
      std::cout<<"cImage::LoadFromFile Texture Greyscale Heightmap Image "<<string::ToUTF8(sFilename)<<std::endl;
      type = IMAGE_TYPE::HEIGHTMAP;
    } else if (16 == pSurface->format->BitsPerPixel) {
      std::cout<<"cImage::LoadFromFile Greyscale Heightmap Image "<<string::ToUTF8(sFilename)<<std::endl;
      type = IMAGE_TYPE::HEIGHTMAP;
    } else if (24 == pSurface->format->BitsPerPixel) {
      std::cout<<"cImage::LoadFromFile "<<string::ToUTF8(sFilename)<<" is a 24 bit RGB image"<<std::endl;
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
      pixelFormat = PIXELFORMAT::R8G8B8A8;
    } else if (32 == pSurface->format->BitsPerPixel) {
      std::cout<<"cImage::LoadFromFile "<<string::ToUTF8(sFilename)<<" is a 32 bit RGBA image"<<std::endl;

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
      pixelFormat = PIXELFORMAT::R8G8B8A8;
    } else {
      std::ostringstream t;
      t << pSurface->format->BitsPerPixel;
      std::cout<<"cImage::LoadFromFile Error Unknown Image Format ("<<t.str()<<"bit) "<<string::ToUTF8(sFilename)<<", returning false"<<std::endl;
      return false;
    }

    width = pSurface->w;
    height = pSurface->h;

    std::cout<<"cImage::LoadFromFile "<<width<<"x"<<height<<std::endl;

    CopyFromSurfaceToData(pSurface->w, pSurface->h);

    return true;
  }

  bool cImage::CreateFromBuffer(const uint8_t* pBuffer, size_t _width, size_t _height, PIXELFORMAT pixelFormat)
  {
    std::cout<<"cImage::CreateFromBuffer "<<_width<<"x"<<_height<<std::endl;

    // Only RGBA is supported at the moment
    assert(pixelFormat == PIXELFORMAT::R8G8B8A8);

    // Load the buffer into a surface
    const size_t depth = 32;
    const size_t pitch = _width * GetBytesForPixelFormat(pixelFormat);

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

    pSurface = SDL_CreateRGBSurfaceFrom((void*)pBuffer, _width, _height, depth, pitch, rmask, gmask, bmask, amask);

    /*// Were we able to load the bitmap?
    if (pTemp == nullptr) {
      std::cout<<"cImage::CreateFromBuffer Unable to load bitmap: "<<SDL_GetError()<<std::endl;
      return false;
    }

    // Convert the image to optimal display format
    pSurface = SDL_DisplayFormat(pTemp);

    // Free the temporary surface
    SDL_FreeSurface(pTemp);
    pTemp = nullptr;*/

    width = pSurface->w;
    height = pSurface->h;

    //std::cout<<"cImage::CreateFromBuffer "<<width<<"x"<<height<<std::endl;

    CopyFromSurfaceToData(pSurface->w, pSurface->h);

    return true;
  }

  void cImage::CopyFromSurfaceToData(size_t _width, size_t _height)
  {
    // Fill out the pData structure array, we use this for when we have to reload this data
    // on a task switch or fullscreen mode change

    width = _width;
    height = _height;

    CopyFromSurfaceToData();
  }

  void cImage::CopyFromSurfaceToData()
  {
    assert(pSurface != nullptr);

    const size_t n = width * height * GetBytesPerPixel();

    // Fill out the pData structure array, we use this for when we have to reload this data
    // on a task switch or fullscreen mode change
    if (data.empty()) data.resize(n, 0);

    std::memcpy(&data[0], pSurface->pixels, n);
  }

  void cImage::CopyFromDataToSurface()
  {
    assert(pSurface != nullptr);

    if (data.empty()) return;

    const size_t n = width * height * GetBytesPerPixel();

    std::memcpy(pSurface->pixels, &data[0], n);
  }

  void cImage::FlipDataVertically()
  {
    if (data.empty()) return;

    // For each row swap it with the corresponding row on the other side of the image
    const size_t nBytesPerRow = GetBytesPerPixel() * width;
    uint8_t buffer[nBytesPerRow];
    const size_t halfHeight = height / 2;
    for (size_t y = 0; y < halfHeight; y++) {
      std::memcpy(&buffer[0], &data[(nBytesPerRow * (height - 1)) - (y * nBytesPerRow)], nBytesPerRow);
      std::memcpy(&data[(nBytesPerRow * (height - 1)) - (y * nBytesPerRow)], &data[(y * nBytesPerRow)], nBytesPerRow);
      std::memcpy(&data[(y * nBytesPerRow)], &buffer[0], nBytesPerRow);
    }
  }

  void cImage::FlipDataHorizontally()
  {
    /*if (data.empty()) return;

    // For each column swap it with the corresponding column on the other side of the image
    const size_t nBytesPerRow = GetBytesPerPixel() * width;
    const size_t halfWidth = width / 2;
    for (size_t y = 0; y < height; y++) {
      std::memcpy(&data[(y * nBytesPerRow)], &data[(nBytesPerRow * height) - (y * nBytesPerRow)], nBytesPerRow);
    }*/
  }

  bool cImage::SaveToBMP(const string_t& inFilename) const
  {
    assert(pSurface != nullptr);
    SDL_SaveBMP(pSurface, string::ToUTF8(inFilename).c_str());
    return true;
  }
}
