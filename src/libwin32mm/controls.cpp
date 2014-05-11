// Windows headers
#include <windowsx.h>

// libwin32mm headers
#include <libwin32mm/controls.h>

// Spitfire headers
#include <spitfire/math/math.h>

namespace win32mm
{
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
