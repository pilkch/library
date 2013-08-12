#ifndef gtkmmicontheme_h
#define gtkmmicontheme_h

// Standard headers
#include <string>

// gtkmm headers
#include <gtkmm.h>

namespace gtkmm
{
  #define sICON_ADD "gtk-add"
  #define sICON_DIRECTORY "gtk-directory"
  #define sICON_GO_UP "gtk-go-up"
  #define sICON_CONVERT "gtk-convert"
  #define sICON_DELETE "gtk-delete"
  #define sICON_FIND "gtk-find"
  #define sICON_MEDIA_PREVIOUS "gtk-media-previous-ltr"
  #define sICON_MEDIA_PLAY "gtk-media-play-ltr"
  #define sICON_MEDIA_NEXT "gtk-media-next-ltr"
  #define sICON_REPEAT_TOGGLE "gtk-goto-top"
  #define sICON_STOP "gtk-stop"
  #define sICON_ZOOM_100 "gtk-zoom-100"
  #define sICON_ZOOM_FIT "gtk-zoom-fit"
  #define sICON_ZOOM_IN "gtk-zoom-in"
  #define sICON_ZOOM_OUT "gtk-zoom-out"

  class cIconTheme
  {
  public:
    cIconTheme();

    template <class T>
    void RegisterThemeChangedListener(T& listener);

    void LoadStockIconWithSizePixels(const std::string& sStockIconName, size_t sizePixels, Gtk::Image& image);
    void LoadStockIcon(const std::string& sStockIconName, Gtk::Image& image);
    void LoadStockIconRotatedClockwise(const std::string& sStockIconName, Gtk::Image& image);

  private:
    Glib::RefPtr<Gtk::IconTheme> icon_theme;
  };

  template <class T>
  void cIconTheme::RegisterThemeChangedListener(T& listener)
  {
    // Connect changed signal of IconTheme
    icon_theme->signal_changed().connect(sigc::mem_fun(listener, &T::OnThemeChanged));
  }
}

#endif // gtkmmicontheme_h
