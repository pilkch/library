#ifndef CMANAGER_H
#define CMANAGER_H

// TODO: include shadows for gui with an alpha blend niceynicey.
// TODO: Be original, don't use the mac os x controls,
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

// TODO: New gui renderer class
//
// Each widget has properties
// alpha
// position x,y
// scale x,y
// rotation x,y
//
// Render each widget to a layer
//  - Each layer has a fbo which we render the widget to
//  - Each layer takes on the properties of the widget
//  - Do not have to rebuild the layer if these properties change

namespace breathe
{
  namespace gui
  {
    typedef uint32_t id_t;

    enum class WIDGET_TYPE {
      WINDOW,
      STATICTEXT,
      BUTTON,
      CHECKBOX,
      INPUT,
      SLIDER,
      RETRO_BUTTON,
      RETRO_INPUT,
      INVISIBLE_LAYER,
    };

    enum class EVENT_RESULT {
      NOT_HANDLED_PERCOLATE,
      HANDLED
    };

    class cWidget;

    // ** cLayoutAbsolute
    //
    // The children of this layout can be positioned at absolute coordinates within the layout.
    // This is useful for creating the HUD for example.
    // The radar might be positioned at the top left (0, 0), the health bar might be at
    // the bottom left (0.1, 0.9) and the compass might be centered at the bottom (0.45, 0.9).
    //

    class cLayoutAbsolute
    {
    public:
      void AddChild(cWidget* pChild);
      void RemoveChildAndDestroy(cWidget* pChild);
      void RemoveChild(cWidget* pChild);

      void SetChildPositionAndSize(cWidget* pChild, const spitfire::math::cVec2&
    position0To1, float fWidth0To1, float fHeight0To1);
      void SetChildPositionAndSize(size_t child, const spitfire::math::cVec2&
    position0To1, float fWidth0To1, float fHeight0To1);

    private:
      std::vector<cWidget*> children;
      std::vector<spitfire::math::cVec2> positions;
      std::vector<float> widths;
      std::vector<float> heights;
    };

    class cManager;

    class cWidget
    {
    public:
      friend class cManager;

      cWidget();
      virtual ~cWidget();

      const cWidget* GetParent() const { return pParent; }
      cWidget* GetParent() { return pParent; }

      id_t GetId() const { return id; }
      void SetId(id_t id);

      WIDGET_TYPE GetType() const { return type; }

      const spitfire::math::cVec2& GetRelativePosition() const { return position; }
      void SetRelativePosition(const spitfire::math::cVec2& position);

      spitfire::math::cVec2 GetAbsolutePosition() const;

      float GetX() const { return position.x; }
      float GetY() const { return position.y; }
      float GetWidth() const { return width; }
      float GetHeight() const { return height; }

      bool IsDirtyRendering() const { return bIsDirtyRendering; }
      void ClearDirtyRendering() { bIsDirtyRendering = false; }

      void AddChild(cWidget* pChild);
      void RemoveChildAndDestroy(cWidget* pChild);
      void RemoveChild(cWidget* pChild);

      size_t GetNumberOfChildren() const { return children.size(); }
      const cWidget* GetChild(size_t i) const { return children[i]; }
      cWidget* GetChild(size_t i) { return children[i]; }

      const cWidget* GetChildById(id_t id) const;
      cWidget* GetChildById(id_t id);

      const cWidget* FindWidgetUnderPoint(const spitfire::math::cVec2& point) const;
      cWidget* FindWidgetUnderPoint(const spitfire::math::cVec2& point);

      void BringToFront(); // Brings this widget to the front (Does not change the visibility)

      void SetVisible(bool bVisible) { bIsVisible = bVisible; }
      bool IsVisible() const { return bIsVisible; }

      void SetEnabled(bool bEnabled) { bIsEnabled = bEnabled; }
      bool IsEnabled() const { return bIsEnabled; }

      void SetFocused();
      bool IsFocused() const { return (bIsEnabled && bIsFocusable && bIsFocused); }

      void SetNextFocused();
      void SetFocusToNextChild();

      const spitfire::math::cColour& GetColour() const { return colourBackground; }
      void SetColour(const spitfire::math::cColour& colour) { colourBackground = colour; }
      const spitfire::math::cColour& GetTextColour() const { return colourText; }
      void SetTextColour(const spitfire::math::cColour& colour) { colourText = colour; }

    protected:
      explicit cWidget(WIDGET_TYPE type);

      EVENT_RESULT OnEventKeyboardDown(int keyCode) { return _OnEventKeyboardDown(keyCode); }
      EVENT_RESULT OnEventKeyboardUp(int keyCode) { return _OnEventKeyboardUp(keyCode); }
      EVENT_RESULT OnEventMouseDown(int button, float x, float y) { return _OnEventMouseDown(button, x, y); }
      EVENT_RESULT OnEventMouseUp(int button, float x, float y) { return _OnEventMouseUp(button, x, y); }
      EVENT_RESULT OnEventMouseMove(int button, float x, float y) { return _OnEventMouseMove(button, x, y); }
      EVENT_RESULT OnEventMouseClick(int button, float x, float y) { return _OnEventMouseClick(button, x, y); }

      void SetFocusable(bool bFocusable) { bIsFocusable = bIsFocusable; }
      bool IsFocusable() const { return (bIsEnabled && bIsFocusable); }

    public:
      const cWidget* _GetRoot() const;
      cWidget* _GetRoot();
      const cWidget* _GetFocusedChild() const;
      cWidget* _GetFocusedChild();
      size_t _GetFocusableChildCount() const;
      void _BringChildToFront(cWidget& widget);

      virtual EVENT_RESULT _OnEventKeyboardDown(int keyCode) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
      virtual EVENT_RESULT _OnEventKeyboardUp(int keyCode) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
      virtual EVENT_RESULT _OnEventMouseDown(int button, float x, float y) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
      virtual EVENT_RESULT _OnEventMouseUp(int button, float x, float y) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
      virtual EVENT_RESULT _OnEventMouseMove(int button, float x, float y) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
      virtual EVENT_RESULT _OnEventMouseClick(int button, float x, float y) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }

      id_t id;
      WIDGET_TYPE type;
      spitfire::math::cVec2 position;
      float width;
      float height;

      spitfire::string_t sCaption;

      bool bIsDirtyRendering;

      cWidget* pParent;
      std::vector<cWidget*> children; // Children in back to front order (0 is the child lowest in the z order, n -1 is the child at the front)

      bool bIsVisible;
      bool bIsEnabled;
      bool bIsFocusable;
      bool bIsFocused;

      spitfire::math::cColour colourBackground;
      spitfire::math::cColour colourText;
    };

    class cLayer : public cWidget
    {
    public:
      cLayer();
    };

    class cWindow : public cWidget
    {
    public:
      cWindow();
    };

    class cStaticText : public cWidget
    {
    public:
      cStaticText();
    };

    class cButton : public cWidget
    {
    public:
      cButton();

    private:
      virtual EVENT_RESULT _OnEventMouseDown(int button, float x, float y);
      virtual EVENT_RESULT _OnEventMouseUp(int button, float x, float y);
      virtual EVENT_RESULT _OnEventMouseMove(int button, float x, float y);
      virtual EVENT_RESULT _OnEventMouseClick(int button, float x, float y);
    };

    class cCheckbox : public cWidget
    {
    public:
      cCheckbox();

      bool IsSelected() const;
      void SetSelected(bool bSelected);

    private:
      virtual EVENT_RESULT _OnEventMouseDown(int button, float x, float y);
      virtual EVENT_RESULT _OnEventMouseUp(int button, float x, float y);
      virtual EVENT_RESULT _OnEventMouseMove(int button, float x, float y);
      virtual EVENT_RESULT _OnEventMouseClick(int button, float x, float y);

      bool bIsSelected;
    };

    class cInput : public cWidget
    {
    public:
      cInput();
    };

    class cSlider : public cWidget
    {
    public:
      cSlider();
    };

    // ** Retro versions of the normal widgets

    class cRetroButton : public cButton
    {
    public:
      cRetroButton();
    };

    class cRetroInput : public cInput
    {
    public:
      cRetroInput();
    };

    //cImage
    //cHUDText
    //cHUDImage
    //cHUDGraph
    //cHUDRadar
    //cHUDVehicleGuageGear
    //cHUDVehicleGuageSpeedo
    //cHUDVehicleGuageRPM
    //cHUDGuageInputHorizontal (Steering)
    //cHUDGuageInputVertical (Accelerator, brake, hand brake and clutch)

    //class cImage
    //{
    //public:
    //  const cImage& GetImage() const { return image; }
    //
    //private:
    //  cImage image;
    //};

    //class cGraph
    //{
    //public:
    //  const std::array<float>& GetValues() const { return values; }
    //
    //private:
    //  std::array<float> values;
    //};

    class cManager
    {
    public:
      cManager();

      const cWidget* GetRoot() const { return pRoot; }
      cWidget* GetRoot() { return pRoot; }
      void SetRoot(cWidget* pWidget) { pRoot = pWidget; }

      void SetHUDOffset(const spitfire::math::cVec2& offset) { HUDOffset = offset; }
      const spitfire::math::cVec2& GetHUDOffset() const { return HUDOffset; }

      float GetTextHeight() const;
      float GetTextWidth(const spitfire::string_t& sText) const;

      float GetStaticTextHeight() const;
      float GetButtonHeight() const;
      float GetCheckboxHeight() const;
      float GetInputHeight() const;

      const spitfire::math::cColour& GetColourWindow() const { return colourWindow; }
      const spitfire::math::cColour& GetColourWidget() const { return colourWidget; }
      const spitfire::math::cColour& GetColourText() const { return colourText; }

      cWindow* CreateWindow();
      cStaticText* CreateStaticText();
      cButton* CreateButton();
      cCheckbox* CreateCheckbox();
      cInput* CreateInput();
      cSlider* CreateSlider();

      // Retro versions of the normal widgets
      cRetroInput* CreateRetroInput();
      cRetroButton* CreateRetroButton();

      // Inject events into the window manager
      // NOTE: These return true if they have handled the event
      bool InjectEventKeyboardDown(int keyCode);
      bool InjectEventKeyboardUp(int keyCode);
      bool InjectEventMouseDown(int button, float x, float y);
      bool InjectEventMouseUp(int button, float x, float y);
      bool InjectEventMouseMove(int button, float x, float y);

    private:
      const cWidget* GetFocusedWidget() const;
      cWidget* GetFocusedWidget();
      const cWidget* FindWidgetUnderPoint(const spitfire::math::cVec2& point) const;
      cWidget* FindWidgetUnderPoint(const spitfire::math::cVec2& point);

      cLayoutAbsolute HUD;

      spitfire::math::cVec2 HUDOffset; // For shaking the HUD

      cWidget* pRoot;
      id_t idMouseLeftButtonDown;

      spitfire::math::cColour colourWindow;
      spitfire::math::cColour colourWidget;
      spitfire::math::cColour colourText;
    };
  }
}

#endif // CMANAGER_H
