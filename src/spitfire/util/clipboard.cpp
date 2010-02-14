// Spitfire headers
#include <spitfire/util/clipboard.h>

namespace spitfire
{
  namespace util
  {
#ifdef __GTK__
    bool cClipboardHandler::IsTextOnClipboard() const
    {
      return (gtk_clipboard_wait_is_text_available(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD)) == TRUE);
    }

    bool cClipboardHandler::PasteTextFromClipboard(string_t& sText) const
    {
      const char* szText = gtk_clipboard_wait_for_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
      if (szText == nullptr) return false;
      sText = string::ToString(szText);
      g_free(szText);
      return true;
    }

    void cClipboardHandler::CopyTextToClipboard(const string_t& sText)
    {
      const std::string sTextUTF8 = ToUTF8(sText);
      gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), sTextUTF8.c_str(), -1);
    }
#else
    bool cClipboardHandler::IsTextOnClipboard() const
    {
      return !sText.empty();
    }

    bool cClipboardHandler::PasteTextFromClipboard(string_t& _sText) const
    {
      sText = _sText;
    }

    void cClipboardHandler::CopyTextToClipboard(const string_t& _sText)
    {
      _sText = sText;
    }
#endif
  }
}
