#ifndef LIBWIN32MM_ICON_H
#define LIBWIN32MM_ICON_H

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>

#ifdef LoadIcon
#undef LoadIcon
#endif

namespace win32mm
{
  class cIcon
  {
  public:
    cIcon();
    ~cIcon();

    void Release();

    bool LoadFromFile(const string_t& sFilePath, size_t widthOrHeight);

    bool IsValid() const;

    HICON GetIcon() const;

  private:
    HICON icon;
  };

  // ** Inlines

  inline cIcon::cIcon() :
    icon(NULL)
  {
  }

  inline cIcon::~cIcon()
  {
    Release();
  }

  inline bool cIcon::IsValid() const
  {
    return (icon != NULL);
  }

  inline HICON cIcon::GetIcon() const
  {
    return icon;
  }

  inline bool cIcon::LoadFromFile(const string_t& sFilePath, size_t widthOrHeight)
  {
    icon = HICON(::LoadImage(NULL, sFilePath.c_str(), IMAGE_ICON, int(widthOrHeight), int(widthOrHeight), LR_LOADFROMFILE));

    return (icon != NULL);
  }

  inline void cIcon::Release()
  {
    if (icon != NULL) {
      ::DestroyIcon(icon);
      icon = NULL;
    }
  }
}

#endif // LIBWIN32MM_ICON_H
