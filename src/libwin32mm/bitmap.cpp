// Standard headers
#include <iostream>

// Windows headers
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#undef min
#undef max

// libwin32mm headers
#include <libwin32mm/bitmap.h>

#pragma comment(lib, "gdiplus.lib")

namespace win32mm
{
  bool cGDIPlusBitmap::LoadFromFile(const string_t& sFilePath)
  {
    Release();

    pBitmap = Gdiplus::Bitmap::FromFile(sFilePath.c_str());

    return (IsValid() && (pBitmap->GetLastStatus() == Gdiplus::Ok));
  }

  void cGDIPlusBitmap::Release()
  {
    delete pBitmap;
    pBitmap = nullptr;
  }

  HBITMAP cGDIPlusBitmap::GetHBitmap()
  {
    ASSERT(IsValid());

    HBITMAP hBitmap = NULL;
    pBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &hBitmap);
    return hBitmap;
  }

  void cGDIPlusBitmap::CopyTo(cBitmap& rhs)
  {
    rhs.Set(GetHBitmap());
  }

  void cGDIPlusBitmap::StretchFrom(const cGDIPlusBitmap& bitmap, size_t newWidth, size_t newHeight)
  {
    ASSERT(bitmap.IsValid());

    Release();

    // Create our stretched version
    Gdiplus::Bitmap* pNewBitmap = (Gdiplus::Bitmap*)bitmap.pBitmap->GetThumbnailImage(UINT(newWidth), UINT(newHeight));
    if (pNewBitmap != nullptr) {
      // Take ownership of the new bitmap
      pBitmap = pNewBitmap;
    }
  }
}
