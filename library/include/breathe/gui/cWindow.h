#ifndef CWINDOW_H
#define CWINDOW_H

namespace breathe
{
  namespace gui
  {
    class cWindow : public cWidget
    {
    public:
      void OnEvent(id_t idEvent) { _OnEvent(idEvent); }
      void OnMouseEvent(int button, int state, float x, float y);
      void Update(sampletime_t currentTime);

      bool HasCaption() const { return !text.empty(); }
      const string_t& GetCaption() const { return text; }
      void SetCaption(const string_t& inText) { text = inText; }
      int GetZDepth() const { return z; }

      float GetTitleBarHeightAbsolute() const;

      bool AddChildToContainer(cWidget* pChild);

    protected:
      cWindow(id_t id, float x, float y, float width, float height, const string_t& caption, cWindow* pParent, bool bModeless, bool bResizable);

      bool bModeless;

    private:
      virtual void _OnEvent(id_t idEvent) { printf("cWindow::_OnEvent %d\n", idEvent); }
      virtual void _OnMouseEvent(int button, int state, float x, float y) { printf("cWindow::_OnMouseEvent %d\n", button); }

      bool _IsAControl() const { return false; }
      bool _IsAWindow() const { return true; }

      int z;
      cWidget_InvisibleContainer* pChildContainer;
    };

    // *** A normal modeless window, resizable, can call AddModalDialog, can call AddModelessWindow
    class cModelessWindow : public cWindow
    {
    public:
      cModelessWindow(id_t id, float x, float y, float width, float height, const string_t& caption, cWindow* pParent);
    };

    // *** A modal dialog, modal, not resizable, can call AddModalDialog, cannot call AddModelessWindow
    class cModalDialog : public cWindow
    {
    public:
      cModalDialog(id_t id, float x, float y, float width, float height, const string_t& caption, cWindow* pParent);
    };

    // *** A dialog where if you click anywhere behind the dialog it cancels
    typedef cModalDialog cModalDialogClickAnywhereElseToCancel;

/*

class cWidgetContainer
{
public:
  void SetText(wid widget, std::string& value);
  void SetValue(wid widget, int value);

  void GetValue(wid widget, std::string& value) const;
  void GetValue(wid widget, int& value) const;

  void SetRadioButton(wid widget);
  bool GetRadioButtonValue(wid widget) const;

  void SetCheckValue(wid widget, bool bChecked);
  bool GetCheckValue(wid widget) const;
};

class cWindow : public cControlContainer
{
public:
};

class cDialog : public cWindow
{
public:
};

class cWidget : public cWidgetContainer
{
public:

};

void cWidgetContainer::SetText(wid widget, std::string& value)
{
  int n = widgets.size();
  for (int i = 0; i < n; i++) widgets[i]._SetText(value);
}


class cWindowManager
{
public:


private:
  std::map<gui_id, cWidget*> mWidgets;
};


<include path="gui_console.xml"/>
...

<window id="cSUDOKU_SETTINGS" caption="window">
  <button text="button" id="cSUDOKU_SETTINGS_BUTTON"/>
  <text text="text" id="cSUDOKU_SETTINGS_TEXT"/>
  <text radio="radio" id="cSUDOKU_SETTINGS_RADIO"/>
  <text check="check" id="cSUDOKU_SETTINGS_CHECK"/>
</window>

<dialog>
</dialog>

A window is modeless.
A dialog is a window that has a parent (Modal), no close or minimise buttons and is dismissed by either
pressing escape, clicking Cancel
OR
pressing enter, clicking OK

window& window = window_manager->AddWindow();
window& window = window_manager->GetElement(i);
window.AddButton("Upper");
window.AddButton("Middle");
window.AddButton("Lower");


Window manager will work in 0..1 coordinates.
Everything in widescreen will be stretched automatically.
Resize is handled automatically.

class cWindow : public cWidget
{
public:
  virtual OnInit() {}
  virtual OnDestroy() {}

  virtual OnOk() {}
  virtual OnCancel() {}

  virtual OnChildDestroy(unsigned int message) {}
};

window_manager->AddDialog(new cWindowProxySettings());

cWindow::AddDialog(cDialog* pDialog)
{
  Disable();
  ... add the dialog now;
}
*/
  }
}

#endif // CWINDOW_H
