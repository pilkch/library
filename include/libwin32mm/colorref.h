#ifndef LIBWIN32MM_COLORREF_H
#define LIBWIN32MM_COLORREF_H

// libwin32mm headers
#include <libwin32mm/window.h>

namespace win32mm
{
  class cColorRef
  {
  public:
    cColorRef();
    explicit cColorRef(COLORREF colour);
    cColorRef(uint8_t red, uint8_t green, uint8_t blue);

    COLORREF GetColorRef() const;

    uint8_t GetRed() const;
    uint8_t GetGreen() const;
    uint8_t GetBlue() const;

    bool OpenColorPickerDialogAtCursor(cWindow& parent, std::vector<cColorRef>& colourPalette);

  private:
    COLORREF colour;
  };

  inline cColorRef::cColorRef() :
    colour(0) // Default to black
  {
  }

  inline cColorRef::cColorRef(COLORREF _colour) :
    colour(_colour)
  {
  }

  inline cColorRef::cColorRef(uint8_t red, uint8_t green, uint8_t blue)
  : colour(RGB(red, green, blue))
  {
  }

  inline COLORREF cColorRef::GetColorRef() const
  {
    return colour;
  }

  inline uint8_t cColorRef::GetRed() const
  {
     return GetRValue(colour);
  }

  inline uint8_t cColorRef::GetGreen() const
  {
     return GetGValue(colour);
  }

  inline uint8_t cColorRef::GetBlue() const
  {
     return GetBValue(colour);
  }

  /*bool cColorRef::OpenColorPickerDialogAtCursor(cWindow& parent, std::vector<cColorRef>& colourPalette)
  {
    COLORREF palette[16] = { 0 };

    // Use our last known colours if present
    const size_t n = colourPalette.size();
    for (size_t i = 0; (i < n) && (i < 16); i++) palette[i] = colourPalette[i].colour;

    CHOOSECOLOR settings;
    ZeroMemory(&settings, sizeof(settings));
    settings.lStructSize = sizeof(settings);
    settings.hwndOwner = parent.hwndDlg;
    settings.rgbResult = colour;
    settings.lpCustColors = palette;
    settings.Flags = CC_RGBINIT | CC_SOLIDCOLOR;

    if (ChooseColor(&settings) == 0) return false;

    // Return our colour
    colour = settings.rgbResult;

    colourPalette.clear();

    // Return our last known colours
    for (size_t i = 0; i < 16; i++) colourPalette.push_back(cColorRef(palette[i]));

    return true;
  }*/
}

#endif // LIBWIN32MM_COLORREF_H
