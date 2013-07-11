#ifndef LIBGTKMM_SLIDER_H
#define LIBGTKMM_SLIDER_H

// gtkmm headers
#include <gtkmm.h>

namespace gtkmm
{
  // http://git.gnome.org/browse/rhythmbox/tree/widgets/rb-header.c
  // HACK: We want the behaviour you get with the middle button, so we
  // mangle the event.
  // Clicking with other buttons moves the slider in step increments
  // Clicking with the middle button moves the slider to the location of the click.
  //#define BUILD_GTKMM_SLIDER_DONT_JUMP_HACK

  // HACK: In gtkmm 2.4 the area before the slider was highlighted,
  // in gtkmm 3.0 and later this highlighting is turned off.
  // We can abuse the fill value which does provide highlighting and set
  // the value of the fill value to the value of the slider whenever our slider moves.
  #define BUILD_GTKMM_SLIDER_HIGHLIGHT

  class cGtkmmSlider;

  class cGtkmmSliderHandler
  {
  public:
    virtual ~cGtkmmSliderHandler() {}

    virtual void OnActionSliderValueChanged(const cGtkmmSlider& slider, uint64_t uiValue) = 0;
  };

  class cGtkmmSlider : public Gtk::Scale {
  public:
    cGtkmmSlider(cGtkmmSliderHandler& handler, bool bVertical);
    virtual ~cGtkmmSlider() {}

    void SetValue(uint64_t uiValue);
    uint64_t GetValue() const;
    void SetRange(uint64_t uiMin, uint64_t uiMax);

  private:
    #ifdef BUILD_GTKMM_SLIDER_DONT_JUMP_HACK
    bool OnButtonPressReleaseEvent(GdkEventButton* event);
    #endif
    bool OnValueChanged(Gtk::ScrollType scrollType, double value);

    cGtkmmSliderHandler& handler;

    #ifdef BUILD_GTKMM_SLIDER_DONT_JUMP_HACK
    bool bLeftClickedPressEventChanged;
    #endif

    uint64_t uiMin;
    uint64_t uiMax;
  };
}

#endif // LIBGTKMM_SLIDER_H
