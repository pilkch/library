// Windows headers
#include <windowsx.h>

// libwin32mm headers
#include <libwin32mm/bitmap.h>
#include <libwin32mm/colorref.h>
#include <libwin32mm/controls.h>

// Spitfire headers
#include <spitfire/math/math.h>

namespace win32mm
{
  // ** cCheckBox

  bool cCheckBox::IsChecked() const
  {
    return (Button_GetCheck(control) == BST_CHECKED);
  }

  void cCheckBox::SetChecked(bool bChecked)
  {
    Button_SetCheck(control, (bChecked ? BST_CHECKED : BST_UNCHECKED));
  }


  // ** cComboBox

  LRESULT APIENTRY cComboBox::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    switch (uMsg) {
      case 1: {
        break;
      }
      /*case ...: {
        break;
      }*/
      default:
        break;
    }

    return FALSE;
  }

  LRESULT APIENTRY cComboBox::_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    cComboBox* pThis = (cComboBox*)::GetProp(HWND(hwnd), TEXT("cComboBoxThis"));
    ASSERT(pThis != nullptr);
    return pThis->WindowProc(uMsg, wParam, lParam);
  }


  // ** cLinkControl

  void cLinkControl::Create(cWindow& parent, cLinkControlListener& listener, int idControl, const string_t& sText)
  {
    // Create the SysLink control
    control = ::CreateWindowEx(NULL, WC_LINK, sText.c_str(),
      WS_VISIBLE | WS_CHILD | WS_TABSTOP,
      50, 220, 100, 24,
      parent.GetWindowHandle(),
      (HMENU)NULL,
      ::GetModuleHandle(NULL), NULL
    );

    // Set the default font
    parent.SetControlDefaultFont(control);

    parent.AddHandler(control, *this);

    pListener = &listener;
  }

  LRESULT APIENTRY cLinkControl::OnWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    switch (uMsg) {
      case WM_NOTIFY: {
        switch (((const LPNMHDR)lParam)->code) {
          case NM_CLICK:    // Fall through to the NM_RETURN case
          case NM_RETURN: {
            if (pListener != nullptr) {
              const PNMLINK pNMLink = (const PNMLINK)lParam;
              const LITEM item = pNMLink->item;
              pListener->OnLinkClicked(*this, size_t(item.iLink));
            }

            break;
          }
        }

        break;
      }
      default:
        break;
    }

    return FALSE;
  }


  // ** cImageControl

  void cImageControl::Create(cWindow& parent, const cBitmap& bitmap)
  {
    // Create the control
    control = ::CreateWindowEx(0, WC_STATIC, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | SS_BITMAP, 0, 0, 0, 0, parent.GetWindowHandle(), 0, GetHInstance(), NULL);

    // Set the image
    SetImage(bitmap);

    parent.AddHandler(control, *this);
  }

  void cImageControl::SetImage(const cBitmap& bitmap)
  {
    // Set the image
    int iResult = ::SendMessage(control, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bitmap.GetHBitmap());
    printf("cImageControl::Create iResult=%d\n", iResult);
  }

  LRESULT APIENTRY cImageControl::OnWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    switch (uMsg) {
      case WM_CTLCOLOR: {
        ::SetBkMode((HDC)wParam, TRANSPARENT);
        return LRESULT(::GetStockObject(NULL_BRUSH));
      }
      case WM_ERASEBKGND: {
        break;
      }
      case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(control, &ps);
        const cColorRef colour(255, 0, 0);
        SetBkColor(hdc, colour.GetColorRef());
        TextOut(hdc, 50, 50, TEXT("Hello"), 5);
        EndPaint(control, &ps);
        break;
      }
      default:
        break;
    }

    return FALSE;
  }


  // ** cScrollBar

  LRESULT cScrollBar::OnWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if ((uMsg != WM_HSCROLL) && (uMsg != WM_VSCROLL)) {
      // Ignore non-scrolling related messages
      return FALSE;
    }

    // Make sure that we can treat horizontal and vertical scrollbars the same way
    STATIC_ASSERT(SB_LINEUP == SB_LINELEFT, "SB_LINUP differs from SB_LINELEFT");
    STATIC_ASSERT(SB_LINEDOWN == SB_LINERIGHT, "SB_LINEDOWN differs from SB_LINERIGHT");
    STATIC_ASSERT(SB_PAGEUP == SB_PAGELEFT, "SB_PAGEUP differs from SB_PAGELEFT");
    STATIC_ASSERT(SB_PAGEDOWN == SB_PAGERIGHT, "SB_PAGEDOWN differs from SB_PAGERIGHT");

    // Get the scroll bar info
    SCROLLINFO info;
    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask = SIF_ALL;
    BOOL bResult = ::GetScrollInfo(control, SB_CTL, &info);
    ASSERT(bResult != FALSE);

    int iNewPosition = info.nPos;

    WORD wNotifyCode = LOWORD(wParam);

    switch (wNotifyCode) {
      case SB_LEFT:
          iNewPosition = info.nMin;
          break;

      case SB_RIGHT:
          iNewPosition = info.nMax;
          break;

      case SB_LINEUP:
          iNewPosition -= max(GetPageSize() >> 2, 1);
          break;

      case SB_LINEDOWN:
          iNewPosition += max(GetPageSize() >> 2, 1);
          break;

      case SB_PAGEUP:
          iNewPosition -= max(GetPageSize(), GetSystemMetrics(SM_CXHTHUMB));
          break;

      case SB_PAGEDOWN:
          iNewPosition += max(GetPageSize(), GetSystemMetrics(SM_CXHTHUMB));
          break;

      case SB_THUMBPOSITION:
          iNewPosition = info.nPos;
          break;

      case SB_THUMBTRACK:
          iNewPosition = info.nTrackPos;
          break;

      default:
        // We haven't handled this message
        return FALSE;
    }

    // Set the scrollbar's new position, enforcing its range
    SetPosition(spitfire::math::clamp(iNewPosition, info.nMin, info.nMax));

    // Notify the parent window
    assert(pWindow != nullptr);
    ::SendMessage(pWindow->GetWindowHandle(), WM_COMMAND, MAKEWPARAM(LOWORD((WPARAM)::GetDlgCtrlID(control)), wNotifyCode), (LPARAM)control);

    // We have handled this message
    return TRUE;
  }
}
