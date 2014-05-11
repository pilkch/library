#ifndef LIBWIN32MM_CONTROLS_H
#define LIBWIN32MM_CONTROLS_H

// libwin32mm headers
#include <libwin32mm/icon.h>
#include <libwin32mm/window.h>

#ifndef CreateWindow
#define CreateWindow  CreateWindowW
#endif

namespace win32mm
{
  const int ID_STATUS_BAR = 10101;

  const int COMBOBOX_INVALID_ITEM = CB_ERR;

  class cWindow;
  class cInputUpDown;
  class cMenu;
  class cPopupMenu;
  class cStatusBar;

  class cStatusBar
  {
  public:
    cStatusBar();

    HWND GetHandle() const;

    void Create(cWindow& parent);

    void SetWidths(const int* widths, size_t nWidths);
    void SetText(size_t segment, const string_t& sText);

    void Resize();

  private:
    HWND control;
  };

  class cGroupBox
  {
  public:
    cGroupBox();

    HWND GetHandle() const;

    void Create(cWindow& parent, const string_t& sText);

  private:
    HWND control;
  };

  class cButton
  {
  public:
    cButton();

    HWND GetHandle() const;

    void Create(cWindow& parent, int idControl, const string_t& sText);
    void Create(cWindow& parent, int idControl, cIcon& icon);
    void CreateOk(cWindow& parent, const string_t& sText);
    void CreateCancel(cWindow& parent);
    void CreateHelp(cWindow& parent);

  private:
    HWND control;
  };

  class cInputUpDown
  {
  public:
    cInputUpDown();

    HWND GetHandleInput() const;
    HWND GetHandleUpDown() const;

    void Create(cWindow& parent, int idControlInput, int iMin, int iMax, int iSmallestStep);

    int GetValue() const;
    void SetValue(int iValue);

  private:
    HWND controlInput;
    HWND controlUpDown;
  };

  class cStatic
  {
  public:
    cStatic();

    HWND GetHandle() const;

    void Create(cWindow& parent, const string_t& sText);

  private:
    HWND control;
  };

  class cComboBox
  {
  public:
    cComboBox();

    void CreateComboBox(cWindow& parent, int idControl);
    void CreatePullDownList(cWindow& parent, int idControl);
    void Destroy();

    HWND GetHandle() const;

    string_t GetText() const;
    void SetText(const string_t& sText);
    int AddString(const string_t& sText);

  private:
    void _Create(cWindow& parent, int idControl, bool bComboBox);
    
    LRESULT APIENTRY WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT APIENTRY _WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND control;
    HWND controlEdit;
    HWND controlComboBox;
  };

  class cHorizontalLine
  {
  public:
    cHorizontalLine();

    HWND GetHandle() const;

    void Create(cWindow& parent);

  private:
    HWND control;
  };

  class cScrollBar : cWindowProcHandler
  {
  public:
    cScrollBar();

    HWND GetHandle() const;

    void CreateVertical(cWindow& window, int idControl);
    void Destroy();

    void GetRange(int& iMin, int& iMax) const;
    void SetRange(int iMin, int iMax);
    int GetPosition() const;
    void SetPosition(int iPosition);
    int GetPageSize() const;
    void SetPageSize(int iPageSize);

  private:
    virtual LRESULT OnWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    HWND control;
    cWindow* pWindow;
  };


  // ** cStatusBar

  inline cStatusBar::cStatusBar() :
    control(NULL)
  {
  }

  inline HWND cStatusBar::GetHandle() const
  {
    return control;
  }

  inline void cStatusBar::Create(cWindow& parent)
  {
    control = ::CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, parent.GetWindowHandle(), (HMENU)ID_STATUS_BAR, GetModuleHandle(NULL), NULL);

    // Set the default font
    parent.SetControlDefaultFont(control);
  }

  inline void cStatusBar::SetWidths(const int* widths, size_t nWidths)
  {
    ::SendMessage(control, SB_SETPARTS, WPARAM(nWidths), LPARAM(widths));
  }

  inline void cStatusBar::SetText(size_t segment, const string_t& sText)
  {
    ::SendMessage(control, SB_SETTEXT, WPARAM(INT(segment)), LPARAM((LPSTR)sText.c_str()));
  }

  inline void cStatusBar::Resize()
  {
    // Send a dummy resize event to update the statusbar position
    ::SendMessage(control, WM_SIZE, 0, 0);
  }


  // ** cGroupBox

  inline cGroupBox::cGroupBox() :
    control(NULL)
  {
  }

  inline HWND cGroupBox::GetHandle() const
  {
    return control;
  }

  inline void cGroupBox::Create(cWindow& parent, const string_t& sText)
  {
    // Create the groupbox

    // Set the default font
    //parent.SetControlDefaultFont(control);
  }

  // ** cButton

  inline cButton::cButton() :
    control(NULL)
  {
  }

  inline HWND cButton::GetHandle() const
  {
    return control;
  }

  inline void cButton::Create(cWindow& parent, int idControl, const string_t& sText)
  {
    // Create the button
    control = ::CreateWindowEx(NULL, TEXT("BUTTON"), sText.c_str(),
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS,
      50, 220, 100, 24,
      parent.GetWindowHandle(),
      (HMENU)idControl,
      ::GetModuleHandle(NULL), NULL
    );

    // Set the default font
    parent.SetControlDefaultFont(control);
  }

  inline void cButton::Create(cWindow& parent, int idControl, cIcon& icon)
  {
    // Create the button
    control = ::CreateWindowEx(NULL, TEXT("BUTTON"), nullptr,
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | BS_ICON | BS_CENTER,
      50, 220, 100, 24,
      parent.GetWindowHandle(),
      (HMENU)idControl,
      ::GetModuleHandle(NULL), NULL
    );

    // Set the default font
    parent.SetControlDefaultFont(control);

    // Set the icon
    ::SendMessage(control, BM_SETIMAGE, WPARAM(IMAGE_ICON), LPARAM(icon.GetIcon()));
  }

  inline void cButton::CreateOk(cWindow& parent, const string_t& sText)
  {
    // Create the button
    Create(parent, IDOK, sText);

    // Make it the default button
    ::SendMessage(control, BM_SETSTYLE, WPARAM(BS_DEFPUSHBUTTON), TRUE);
  }

  inline void cButton::CreateCancel(cWindow& parent)
  {
    // Create the button
    Create(parent, IDCANCEL, TEXT("Cancel"));
  }


  // ** cInputUpDown

  inline cInputUpDown::cInputUpDown() :
    controlInput(NULL),
    controlUpDown(NULL)
  {
  }

  inline HWND cInputUpDown::GetHandleInput() const
  {
    return controlInput;
  }

  inline HWND cInputUpDown::GetHandleUpDown() const
  {
    return controlUpDown;
  }

  inline void cInputUpDown::Create(cWindow& parent, int idControlUpDown, int iMin, int iMax, int iSmallestStep)
  {
    // Create the edit control
    controlInput = ::CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, TEXT(""), ES_LEFT | ES_AUTOHSCROLL | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP, 0, 0, 0, 0, parent.GetWindowHandle(), (HMENU)idControlUpDown + 1, NULL, (LPVOID)0);
    ASSERT(controlInput != NULL);

    // Create the up down control
    controlUpDown = ::CreateWindowEx(0, UPDOWN_CLASS, TEXT(""), WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP | UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_HOTTRACK | UDS_NOTHOUSANDS | UDS_ALIGNRIGHT, 0, 0, 0, 0, parent.GetWindowHandle(), (HMENU)idControlUpDown, NULL, (LPVOID)0);
    ASSERT(controlUpDown != NULL);


    // Set the buddy window now that the control has been created.  We can't set this when creating the control because it will trigger the WM_COMMAND messages for EN_CHANGE and EN_UPDATE which will calls EnableControls twice
    if ((::GetWindowLong(controlUpDown, GWL_STYLE) & UDS_AUTOBUDDY) == 0) ::SendMessage(controlUpDown, UDM_SETBUDDY, WPARAM(controlInput), 0);

    // Set control range
    ::SendMessage(controlUpDown, UDM_SETRANGE32, WPARAM(iMin), LPARAM(iMax));

    // set the control acceleration
    UDACCEL uda[3];
    uda[0].nSec = 0;
    uda[0].nInc = iSmallestStep;
    uda[1].nSec = 1;
    uda[1].nInc = iSmallestStep * 10;
    uda[2].nSec = 5;
    uda[2].nInc = iSmallestStep * 100;
    ::SendMessage(controlUpDown, UDM_SETACCEL, countof(uda), LPARAM(uda));

    if ((iMin < 0) || (iMax < 0)) {
      // Turn off ES_NUMBER so we can edit negative numbers
      ::SetWindowLong(controlInput, GWL_STYLE, ::GetWindowLong(controlInput, GWL_STYLE) & ~ES_NUMBER);
    }

    // Set the initial value
    SetValue(iMin);

    // Set the default font
    parent.SetControlDefaultFont(controlInput);
  }

  inline int cInputUpDown::GetValue() const
  {
   return int(::SendMessage(controlUpDown, UDM_GETPOS32, 0, 0));
  }

  inline void cInputUpDown::SetValue(int iValue)
  {
    ::SendMessage(controlUpDown, UDM_SETPOS32 , 0, LPARAM(iValue));
  }


  // ** cStatic

  inline cStatic::cStatic() :
    control(NULL)
  {
  }

  inline HWND cStatic::GetHandle() const
  {
    return control;
  }

  inline void cStatic::Create(cWindow& parent, const string_t& sText)
  {
    // Create the static text
    control = ::CreateWindowEx(NULL, TEXT("STATIC"), sText.c_str(),
      WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | SS_LEFT,
      50, 220, 100, 24,
      parent.GetWindowHandle(),
      (HMENU)NULL,
      ::GetModuleHandle(NULL), NULL
    );

    // Set the default font
    parent.SetControlDefaultFont(control);
  }


  // ** cComboBox

  inline cComboBox::cComboBox() :
    control(NULL),
    controlEdit(NULL),
    controlComboBox(NULL)
  {
  }

  inline void cComboBox::CreateComboBox(cWindow& parent, int idControl)
  {
    _Create(parent, idControl, true);
  }

  inline void cComboBox::CreatePullDownList(cWindow& parent, int idControl)
  {
    _Create(parent, idControl, false);
  }

  inline void cComboBox::_Create(cWindow& parent, int idControl, bool bComboBox)
  {
    if (bComboBox) {
      control = CreateWindowEx(0, WC_COMBOBOXEX, NULL,
                    WS_BORDER | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,
                    // No size yet--resize after setting image list.
                    0,      // Vertical position of Combobox
                    0,      // Horizontal position of Combobox
                    0,      // Sets the width of Combobox
                    100,    // Sets the height of Combobox
                    parent.GetWindowHandle(),
                    (HMENU)idControl,
                    GetHInstance(),
                    NULL);
    } else {
      control = ::CreateWindow(WC_COMBOBOXEX, TEXT(""), CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_CHILD | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP, 0, 0, 0, 150, parent.GetWindowHandle(), (HMENU)idControl, GetHInstance(), (LPVOID)0);
    }

    // Set the font for this control
    ::SendMessage(control, WM_SETFONT, WPARAM(HFONT(::SendMessage(parent.GetWindowHandle(), WM_GETFONT, 0, 0))), TRUE);

    // Set our user data for this control
    ::SetProp(control, TEXT("cComboBoxThis"), (HANDLE)this);

    controlEdit = (HWND)::SendMessage(control, CBEM_GETEDITCONTROL, 0, 0);
    controlComboBox = (HWND)::SendMessage(control, CBEM_GETCOMBOCONTROL, 0, 0);
  }

  inline void cComboBox::Destroy()
  {
    // Clear the user data for this control
    ::SetProp(control, TEXT("cComboBoxThis"), (HANDLE)nullptr);
  }

  inline HWND cComboBox::GetHandle() const
  {
    return control;
  }

  inline string_t cComboBox::GetText() const
  {
    string_t sText;
    cWindow::GetControlText(control, sText);
    return sText;
  }

  inline void cComboBox::SetText(const string_t& sText)
  {
    cWindow::SetControlText(control, sText);
  }

  inline int cComboBox::AddString(const string_t& sText)
  {
    COMBOBOXEXITEM item;
    item.mask = CBEIF_TEXT;
    item.iItem = -1;
    item.pszText = const_cast<char_t*>(sText.c_str());
    item.cchTextMax = int(sText.length());
    return ::SendMessage(control, CBEM_INSERTITEM, 0, LPARAM(&item));
  }


  // ** cHorizontalLine

  inline cHorizontalLine::cHorizontalLine() :
    control(NULL)
  {
  }

  inline HWND cHorizontalLine::GetHandle() const
  {
    return control;
  }

  inline void cHorizontalLine::Create(cWindow& parent)
  {
    // Create the horizontal line
  }


  // ** cScrollBar

  inline cScrollBar::cScrollBar() :
    control(NULL),
    pWindow(nullptr)
  {
  }

  inline HWND cScrollBar::GetHandle() const
  {
    return control;
  }

  inline void cScrollBar::CreateVertical(cWindow& window, int idControl)
  {
    // Create the scroll bar
    control = ::CreateWindowEx(0, WC_SCROLLBAR, TEXT(""), SBS_VERT | WS_CHILD | WS_VISIBLE, 0, 0, window.GetScrollBarWidth(), 100, window.GetWindowHandle(), (HMENU)idControl, GetHInstance(), (LPVOID)0);
 
    // Set the default font
    window.SetControlDefaultFont(control);

    // Set the default range and position
    SetRange(0, 100);
    SetPageSize(10);
    SetPosition(0);

    pWindow = &window;
    
    // Set our user data for this control
    ::SetProp(control, TEXT("cScrollBarThis"), (HANDLE)this);

    window.AddHandler(control, *this);
  }

  inline void cScrollBar::Destroy()
  {
    // Clear the user data for this control
    ::SetProp(control, TEXT("cScrollBarThis"), (HANDLE)nullptr);
  }

  inline void cScrollBar::GetRange(int& iMin, int& iMax) const
  {
    SCROLLINFO info;
    info.cbSize = sizeof(info);
    info.fMask  = SIF_RANGE;
    ::GetScrollInfo(control, SB_CTL, &info);
    iMin = info.nMin;
    iMax = info.nMax;
  }

  inline void cScrollBar::SetRange(int iMin, int iMax)
  {
    SCROLLINFO info;
    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask  = SIF_RANGE | SIF_DISABLENOSCROLL;
    info.nMin = iMin;
    info.nMax = iMax;
    ::SetScrollInfo(control, SB_CTL, &info, TRUE);

    // Make sure that worked
    int iNewMin = 0;
    int iNewMax = 0;
    GetRange(iNewMin, iNewMax);
    ASSERT(iNewMin == iMin);
    ASSERT(iNewMax == iMax);
  }

  inline int cScrollBar::GetPageSize() const
  {
    SCROLLINFO info;
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE;
    ::GetScrollInfo(control, SB_CTL, &info);
    return info.nPage - 1;
  }

  inline void cScrollBar::SetPageSize(int iPageSize)
  {
    SCROLLINFO info;
    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask  = SIF_PAGE | SIF_DISABLENOSCROLL;
    info.nPage = UINT(iPageSize + 1);
    ::SetScrollInfo(control, SB_CTL, &info, TRUE);

    // Make sure that worked
    ASSERT(GetPageSize() == iPageSize);
  }

  inline int cScrollBar::GetPosition() const
  {
    SCROLLINFO info;
    info.cbSize = sizeof(info);
    info.fMask  = SIF_POS;
    ::GetScrollInfo(control, SB_CTL, &info);
    return info.nPos;
  }

  inline void cScrollBar::SetPosition(int iPosition)
  {
    SCROLLINFO info;
    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask  = SIF_POS | SIF_DISABLENOSCROLL;
    info.nPos = UINT(iPosition);
    ::SetScrollInfo(control, SB_CTL, &info, TRUE);

    // Make sure that worked
    const int iNewPosition = GetPosition();
    ASSERT(iNewPosition == iPosition);
  }
}

#endif // LIBWIN32MM_CONTROLS_H
