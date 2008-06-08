#ifndef CWIDGET_H
#define CWIDGET_H

namespace breathe
{
  namespace render
  {
    class cFont;
  }

  namespace gui
  {
    typedef uint16_t id_t;
    typedef uint16_t event_t;

    id_t GenerateID();

    enum WIDGET_TYPE
    {
      WIDGET_UNKNOWN = 0,
      WIDGET_WINDOW,
      WIDGET_BUTTON,
      WIDGET_STATICTEXT,
      WIDGET_INPUT,
      WIDGET_SLIDER_HORIZONTAL,
      WIDGET_SLIDER_VERTICAL,
      WIDGET_SCROLLBAR_HORIZONTAL,
      WIDGET_SCROLLBAR_VERTICAL
    };

    enum EVENT_TYPE
    {
      EVENT_CLICK_PRIMARY = 0,
      EVENT_CLICK_SECONDARY,
      EVENT_CHANGE_VALUE
    };

    // This is our base object.
    // Windows as well as controls are derived from it.
    // Points are all relative to the parent.
    class cWidget
    {
    public:
      cWidget(id_t idControl, WIDGET_TYPE type, float x, float y, float width, float height);
      virtual ~cWidget();

      //TODO: Don't have AddChild here!  Only goes on Sizer

      bool AddChild(cWidget* pChild);
      cWidget* FindChild(id_t idControl) const;
      cWidget* FindChildAtPoint(float x, float y) const;
      void SetParent(cWidget* _pParent) { pParent = _pParent; }
      cWidget* GetParent() const { return pParent; }

      id_t GetID() const { return idControl; }
      WIDGET_TYPE GetType() const { return type; }

      float HorizontalRelativeToAbsolute(float n) const;
      float VerticalRelativeToAbsolute(float n) const;

      float GetX() const { return x; }
      float GetY() const { return y; }
      float GetWidth() const { return width; }
      float GetHeight() const { return height; }

      int GetMinimum() const { return minimum; }
      int GetMaximum() const { return maximum; }
      int GetValue() const { return value; }
      void SetMinimum(int inMinimum) { minimum = inMinimum; }
      void SetMaximum(int inMaximum) { maximum = inMaximum; }
      void SetValue(int inValue) { value = inValue; }

      const string_t& GetText() const { return text; }
      void SetText(const string_t& inText) { text = inText; }

      bool IsEnabled() const;
      bool IsVisible() const;
      bool IsResizable() const { return bResizable; }

      void SetEnabled(bool _bEnabled) { bEnabled = _bEnabled; }
      void SetVisible(bool _bVisible) { bEnabled = _bVisible; }
      void SetResizable(bool _bResizable) { bResizable = _bResizable; }

      void SetPosition(float x, float y);
      void SetSize(float width, float height);
      void SetSize(float x, float y, float width, float height) { _SetSize(x, y, width, height); }

      void OnLeftMouseDown(float x, float y) { _OnLeftMouseDown(x, y); }
      void OnLeftMouseUp(float x, float y) { _OnLeftMouseUp(x, y); }
      void OnMouseMove(float x, float y) { _OnMouseMove(x, y); }
      void OnMouseOut(float x, float y) { _OnMouseOut(x, y); }

      bool IsAControl() const { return _IsAControl(); }
      bool IsAWindow() const { return _IsAWindow(); }

      void SendCommandToParentWindow(id_t uiCommand);

    protected:
      // Setup an event handler
      void HandleEvent(event_t event, id_t idToReturn) { handlers[event] = idToReturn; }

      // If we have an event handler for this event then handle it
      void CheckAndHandleEvent(event_t event);

      std::vector<cWidget*> child;
      cWidget* pParent;

      const id_t idControl;
      WIDGET_TYPE type;

      int minimum;
      int maximum;
      int value;

      breathe::string_t text;

      float x;
      float y;

      float width;
      float height;

      bool bEnabled;
      bool bVisible;
      bool bResizable;

      // event, eventHandler for calling back
      //std::map<event_t, cEventHandler*> handler;
      std::map<event_t, id_t> handlers;

    private:
      // Find out which id is returned on which event for this control
      bool GetEventHandler(event_t event, id_t& outID) const;

      virtual bool _IsAControl() const { return true; }
      virtual bool _IsAWindow() const { return false; }

      virtual void _SetSize(float x, float y, float width, float height);

      virtual void _OnLeftMouseDown(float x, float y) { if (pParent != nullptr) pParent->OnLeftMouseDown(x, y); }
      virtual void _OnLeftMouseUp(float x, float y) { if (pParent != nullptr) pParent->OnLeftMouseUp(x, y); }
      virtual void _OnMouseMove(float x, float y) { if (pParent != nullptr) pParent->OnMouseMove(x, y); }
      virtual void _OnMouseOut(float x, float y) { if (pParent != nullptr) pParent->OnMouseOut(x, y); }

      friend class cWindowManager;
    };

    inline void cWidget::_SetSize(float _x, float _y, float _width, float _height)
    {
      x = _x;
      y = _y;
      width = _width;
      height = _height;
    }


    template <WIDGET_TYPE t>
    class cWidgetTemplate : public cWidget
    {
    public:
      cWidgetTemplate(id_t idControl, float x, float y, float width, float height) :
        cWidget(idControl, t, x, y, width, height)
      {
      }
    };


    class cWidget_Button : public cWidget
    {
    public:
      cWidget_Button(id_t idControl, float x, float y, float width, float height) :
        cWidget(idControl, WIDGET_BUTTON, x, y, width, height),
        bCurrentlyClickingOnThisControl(false)
      {
        HandleClickPrimary(idControl);
      }

      void HandleClickPrimary(id_t idToReturn)
      {
        HandleEvent(EVENT_CLICK_PRIMARY, idToReturn);
      }

    private:
      void _OnLeftMouseDown(float x, float y) { bCurrentlyClickingOnThisControl = true; }
      void _OnLeftMouseUp(float x, float y);
      void _OnMouseOut(float x, float y) { bCurrentlyClickingOnThisControl = false; }

      bool bCurrentlyClickingOnThisControl;
    };

    class cWidget_SliderHorizontal : public cWidget
    {
    public:
      cWidget_SliderHorizontal(id_t idControl, float x, float y, float width, float height) :
        cWidget(idControl, WIDGET_SLIDER_HORIZONTAL, x, y, width, height),
        bCurrentlyClickingOnThisControl(false)
      {
        HandleChangeValue(idControl);
      }

      void HandleChangeValue(id_t idToReturn)
      {
        HandleEvent(EVENT_CHANGE_VALUE, idToReturn);
      }

    private:
      void _OnLeftMouseDown(float x, float y) { bCurrentlyClickingOnThisControl = true; }
      void _OnLeftMouseUp(float x, float y);
      void _OnMouseMove(float x, float y);
      void _OnMouseOut(float x, float y) { bCurrentlyClickingOnThisControl = false; }

      bool bCurrentlyClickingOnThisControl;
      float fTempValue;
    };

    class cWidget_StaticText : public cWidget
    {
      public:
      cWidget_StaticText(id_t idControl, float x, float y, float width, float height) :
        cWidget(idControl, WIDGET_STATICTEXT, x, y, width, height),
          pFont(nullptr)
      {
      }

      render::cFont* GetFont() const
      {
        return pFont;
      }

      const math::cColour& GetColour() const
      {
        return colour;
      }

      void SetFont(render::cFont* _pFont)
      {
        pFont = _pFont;
      }

      void SetColour(const math::cColour& _colour)
      {
        colour = _colour;
      }

    private:
      render::cFont* pFont;
      math::cColour colour;
    };

    class cWidget_Input : public cWidget
    {
    public:
      cWidget_Input(id_t idControl, float x, float y, float width, float height) :
        cWidget(idControl, WIDGET_INPUT, x, y, width, height),
        pFont(nullptr)
      {
      }

      render::cFont* GetFont() const
      {
        return pFont;
      }

      const math::cColour& GetColour() const
      {
        return colour;
      }

      void SetFont(render::cFont* _pFont)
      {
        pFont = _pFont;
      }

      void SetColour(const math::cColour& _colour)
      {
        colour = _colour;
      }

    private:
      render::cFont* pFont;
      math::cColour colour;
    };

  }
}

#endif // CWIDGET_H
