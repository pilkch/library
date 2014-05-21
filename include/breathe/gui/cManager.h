#ifndef CMANAGER_H
#define CMANAGER_H

// libopenglmm headers
#include <libopenglmm/cWindow.h>

// Breathe headers
#include <breathe/breathe.h>
#include <breathe/util/joystick.h>

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

#ifdef __WIN__
#pragma push_macro("CreateWindow")
#undef CreateWindow
#endif

namespace breathe
{
  namespace gui
  {
    typedef size_t id_t;

    using opengl::KEY;
    using opengl::cWindowEvent;
    using opengl::cKeyboardEvent;
    using opengl::cMouseEvent;

    enum class WIDGET_TYPE {
      WINDOW,
      STATICTEXT,
      BUTTON,
      CHECKBOX,
      INPUT,
      COMBOBOX,
      SLIDER,
      TOOLBAR,
      SCROLLBAR,
      RETRO_BUTTON,
      RETRO_INPUT,
      RETRO_INPUT_UPDOWN,
      RETRO_COLOUR_PICKER,
      INVISIBLE_LAYER,
    };

    enum class EVENT_RESULT {
      NOT_HANDLED_PERCOLATE,
      HANDLED
    };

    class cWidget;

    class cWidgetEvent
    {
    public:
      cWidgetEvent();

      const cWidget* GetWidget() const { return pWidget; }
      cWidget* GetWidget() { return pWidget; }

      bool IsChanged() const { return (type == TYPE::CHANGED); }
      bool IsPressed() const { return (type == TYPE::PRESSED); }

      enum class TYPE {
        CHANGED,
        PRESSED,
        UNKNOWN,
      };

      void SetWidget(cWidget* _pWidget) { pWidget = _pWidget; }
      void SetType(TYPE _type) { type = _type; }

    private:
      cWidget* pWidget;
      TYPE type;
    };


    class cWidgetEventListener
    {
    public:
      virtual ~cWidgetEventListener() {}

      virtual EVENT_RESULT _OnWidgetEvent(const cWidgetEvent& event) = 0;
    };


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
    class cRenderer;

    class cWidget
    {
    public:
      friend class cManager;
      friend class cRenderer;

      cWidget();
      virtual ~cWidget();

      void SetEventListener(cWidgetEventListener& listener);

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
      void SetWidth(float _width) { width = _width; }
      float GetHeight() const { return height; }
      void SetHeight(float _height) { height = _height; }

      string_t GetCaption() const { return sCaption; }
      void SetCaption(const string_t& _sCaption) { sCaption = _sCaption; }

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

      void SetVisible(bool bVisible);
      bool IsVisible() const { return bIsVisible; }

      void SetEnabled(bool bEnabled) { bIsEnabled = bEnabled; }
      bool IsEnabled() const { return bIsEnabled; }

      void SetFocused();
      bool IsFocused() const { return (bIsEnabled && bIsFocusable && bIsFocused); }

      void SetNextFocused();
      void SetPreviousFocused();
      void SetFocusToFirstChild();
      void SetFocusToNextChild();
      void SetFocusToPreviousChild();

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

    protected:
      const cWidget* _GetRoot() const;
      cWidget* _GetRoot();

      // Direct children of this widget
      const cWidget* _GetFocusedChild() const;
      cWidget* _GetFocusedChild();
      size_t _GetFocusableChildCount() const;
      void _BringChildToFront(cWidget& widget);

      // All widgets under the tree of this widget
      const cWidget* _GetFocusedDescendant() const;
      cWidget* _GetFocusedDescendant();

      void RemoveFocus();
      bool SetFocusToFirstChildRecursive();

      virtual EVENT_RESULT _OnEventKeyboardDown(int keyCode) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
      virtual EVENT_RESULT _OnEventKeyboardUp(int keyCode) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
      virtual EVENT_RESULT _OnEventMouseDown(int button, float x, float y) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
      virtual EVENT_RESULT _OnEventMouseUp(int button, float x, float y) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
      virtual EVENT_RESULT _OnEventMouseMove(int button, float x, float y) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }
      virtual EVENT_RESULT _OnEventMouseClick(int button, float x, float y) { return EVENT_RESULT::NOT_HANDLED_PERCOLATE; }

      cWidgetEventListener* pListener;

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
      virtual EVENT_RESULT _OnEventMouseDown(int button, float x, float y) override;
      virtual EVENT_RESULT _OnEventMouseUp(int button, float x, float y) override;
      virtual EVENT_RESULT _OnEventMouseMove(int button, float x, float y) override;
      virtual EVENT_RESULT _OnEventMouseClick(int button, float x, float y) override;
    };

    class cCheckbox : public cWidget
    {
    public:
      cCheckbox();

      bool IsSelected() const;
      void SetSelected(bool bSelected);

    private:
      virtual EVENT_RESULT _OnEventMouseDown(int button, float x, float y) override;
      virtual EVENT_RESULT _OnEventMouseUp(int button, float x, float y) override;
      virtual EVENT_RESULT _OnEventMouseMove(int button, float x, float y) override;
      virtual EVENT_RESULT _OnEventMouseClick(int button, float x, float y) override;

      bool bIsSelected;
    };

    class cInput : public cWidget
    {
    public:
      cInput();
    };

    class cComboBox : public cWidget
    {
    public:
      cComboBox();
    };

    class cSlider : public cWidget
    {
    public:
      cSlider();
    };

    class cToolbar : public cWidget
    {
    public:
      cToolbar();
    };

    class cScrollbar : public cWidget
    {
    public:
      cScrollbar();
    };

    // ** Retro versions of the normal widgets

    class cRetroButton : public cButton
    {
    public:
      cRetroButton();

    private:
      virtual EVENT_RESULT _OnEventKeyboardDown(int keyCode) override;
    };

    class cRetroInput : public cInput
    {
    public:
      cRetroInput();

    private:
      virtual EVENT_RESULT _OnEventKeyboardDown(int keyCode) override;
    };

    class cRetroInputUpDown : public cRetroInput
    {
    public:
      cRetroInputUpDown();

      int GetMin() const { return min; }
      int GetMax() const { return max; }
      void SetRange(int min, int max);
      int GetValue() const { return value; }
      void SetValue(int value, bool bNotifyListener);

    private:
      virtual EVENT_RESULT _OnEventKeyboardDown(int keyCode) override;

      int min;
      int max;
      int value;
    };

    class cRetroColourPicker : public cRetroInput
    {
    public:
      cRetroColourPicker();

      size_t GetNumberOfColours() const;
      string_t GetColourName(size_t index) const;
      spitfire::math::cColour GetColour(size_t index) const;
      void AddColour(const string_t& sName, const spitfire::math::cColour& colour);

      size_t GetSelectedColour() const { return selected; }
      void SetSelectedColour(size_t index, bool bNotifyListener);

    private:
      virtual EVENT_RESULT _OnEventKeyboardDown(int keyCode) override;

      std::vector<string_t> colourNames;
      std::vector<spitfire::math::cColour> colours;

      size_t selected;
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
      ~cManager();

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
      float GetComboBoxHeight() const;
      float GetToolbarWidthOrHeight() const;
      float GetScrollBarWidthOrHeight() const;
      float GetSpacerWidth() const;
      float GetSpacerHeight() const;

      const spitfire::math::cColour& GetColourWindow() const { return colourWindow; }
      const spitfire::math::cColour& GetColourWidget() const { return colourWidget; }
      const spitfire::math::cColour& GetColourText() const { return colourText; }

      cWindow* CreateWindow();
      cStaticText* CreateStaticText();
      cButton* CreateButton();
      cCheckbox* CreateCheckbox();
      cInput* CreateInput();
      cComboBox* CreateComboBox();
      cSlider* CreateSlider();
      cToolbar* CreateToolbar();
      cScrollbar* CreateScrollbar();

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
      bool InjectEventJoystickEvent(const breathe::util::cJoystickEvent& event);

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
