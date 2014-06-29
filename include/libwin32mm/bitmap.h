#ifndef LIBWIN32MM_BITMAP_H
#define LIBWIN32MM_BITMAP_H

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>

namespace Gdiplus
{
  class Bitmap;
}

namespace win32mm
{
  class cBitmap
  {
  public:
    cBitmap();
    ~cBitmap();

    void Release();

    bool LoadFromFile(const string_t& sFilePath);

    bool IsValid() const;

    void Set(HBITMAP bitmap);

    HBITMAP GetHBitmap() const;

  private:
    HBITMAP bitmap;
  };

  class cGDIPlusBitmap
  {
  public:
    cGDIPlusBitmap();

    bool LoadFromFile(const string_t& sFilePath);
    void Release();

    bool IsValid() const;

    void Set(Gdiplus::Bitmap* pBitmap);
    HBITMAP GetHBitmap();

    void CopyTo(cBitmap& bitmap);

    void StretchFrom(const cGDIPlusBitmap& bitmap, size_t newWidth, size_t newHeight);

  private:
    Gdiplus::Bitmap* pBitmap;
  };


  // ** Inlines

  inline cBitmap::cBitmap() :
    bitmap(NULL)
  {
  }

  inline cBitmap::~cBitmap()
  {
    Release();
  }

  inline bool cBitmap::IsValid() const
  {
    return (bitmap != NULL);
  }

  inline HBITMAP cBitmap::GetHBitmap() const
  {
    return bitmap;
  }

  inline bool cBitmap::LoadFromFile(const string_t& sFilePath)
  {
    Release();

    bitmap = HBITMAP(::LoadImage(NULL, sFilePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));

    return (bitmap != NULL);
  }

  inline void cBitmap::Release()
  {
    if (bitmap != NULL) {
      ::DeleteObject(bitmap);
      bitmap = NULL;
    }
  }

  inline void cBitmap::Set(HBITMAP _bitmap)
  {
    Release();

    bitmap = _bitmap;
  }


  // ** cGDIPlusBitmap

  inline cGDIPlusBitmap::cGDIPlusBitmap() :
    pBitmap(nullptr)
  {
  }

  inline bool cGDIPlusBitmap::IsValid() const
  {
    return (pBitmap != nullptr);
  }
}

#endif // LIBWIN32MM_BITMAP_H
