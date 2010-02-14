#ifndef CLIPBOARD_H
#define CLIPBOARD_H

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>

namespace spitfire
{
  namespace util
  {
    class cClipboardHandler
    {
    public:
      bool IsTextOnClipboard() const;

      bool PasteTextFromClipboard(string_t& sText) const;
      void CopyTextToClipboard(const string_t& sText);

    private:
#ifndef __GTK__
      string_t sText; // Ghetto internal copy only version
#endif
    };
  }
}

#endif // CLIPBOARD_H
