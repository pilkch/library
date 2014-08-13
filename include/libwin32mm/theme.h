#ifndef LIBWIN32MM_THEME_H
#define LIBWIN32MM_THEME_H

// libwin32mm headers
#include <libwin32mm/colorref.h>

namespace win32mm
{
  class cTheme
  {
  public:
    cColorRef GetHighlightTextColour() const;
    cColorRef GetHighlightBackgroundColour() const;
  };

  inline cColorRef cTheme::GetHighlightTextColour() const
  {
    return cColorRef(GetSysColor(COLOR_HIGHLIGHTTEXT));
  }

  inline cColorRef cTheme::GetHighlightBackgroundColour() const
  {
    return cColorRef(GetSysColor(COLOR_HIGHLIGHT));
  }
}

#endif // LIBWIN32MM_THEME_H
