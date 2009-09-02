#ifndef CWINDOW_MANAGER_H
#define CWINDOW_MANAGER_H

// TODO: include shadows for gui with an alpha blend niceynicey.
// TODO: Be original, don''t use the mac os x controls,
// TODO: use original solid black white and grey rectangles with rounded edges
// TODO: alphablending
// TODO: menu is black, white and grey, in game hud is team colour ie. blue vs red

// For first version just have:
// rectangle.png // stretchable

// void DrawWindow(float_t x, float_t y, float_t width, float_t height)
// {
//    // Rectangle in the middle
//   RenderRectangle(rectangle.png, x, y, width, height);
// }

namespace breathe
{
  namespace gui
  {
    class cWindowManager
    {
    public:
      typedef std::list<cWindow*> child_list;
      typedef child_list::iterator child_iterator;
      typedef child_list::const_iterator child_const_iterator;

      cWindowManager();
      ~cWindowManager();

      void LoadTheme();
      void LoadGuiFromXML(const string_t& sFilename);

      // Automatically adds a string with an automatically generated id if not found
      id_t GetIDFromStringIdentifier(const string_t& stringIdentifier);

      void Render() const;

      bool OnMouseEvent(int button, int state, float x, float y);
      void Update(sampletime_t currentTime);

      bool AddChild(cWindow* pChild);
      bool RemoveChild(cWindow* pChild);

      // For overriding the gui with team colours for example
      void SetColourDefault() { colour.SetRGBA(0.5f, 0.5f, 0.5f, 0.5f); }
      void SetColourRGB(const math::cColour& _colour) { colour.SetRGBA(_colour.r, _colour.g, _colour.b, 0.5f); }

    private:
      cWindow* _FindWindowUnderPoint(float x, float y) const;

      void _RenderChildren(const cWidget& widget) const;
      void _RenderWidget(const cWidget& widget) const;

      // This is just for the first version to get something rendered, don't worry about texturing, just draw
      // a plain solid colour filled rectangle with a border
      void _RenderRectangle(float x, float y, float width, float height) const;

      void _RenderWindow(const cWindow& widget) const;
      void _RenderButton(const cWidget_Button& widget) const;
      void _RenderStaticText(const cWidget_StaticText& widget) const;
      void _RenderInput(const cWidget_Input& widget) const;

      child_list child;
      cWidget* pEventWidgetMouseLeftButtonDown;
      std::map<string_t, id_t> stringIdentifierToID;

      math::cColour colour;
    };
  }
}

#endif // CWINDOW_MANAGER_H
