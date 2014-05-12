#ifndef LIBWIN32MM_TASKBAR_H
#define LIBWIN32MM_TASKBAR_H

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>
#include <libwin32mm/com.h>
#include <libwin32mm/window.h>

namespace win32mm
{
  // ** cTaskBar

  class cTaskBar {
  public:
    cTaskBar();

    void Init(cWindow& owner);
    
    bool IsValid() const;

    // Overlay the task bar icon with a custom icon
    void SetOverlayIcon(HICON hIcon, const string_t& sDescription);
    void RemoveOverlayIcon();

    // Progress functions
    void SetProgressOff();                         // Don't show progress any more
    void SetProgress0To100(float fProgress0To100); // Set the progress percentage from 0 to 100
    void SetProgressIndeterminateOn();
    void SetProgressIndeterminateOff();
    void SetProgressPause();
    void SetProgressError();

  private:
    void _SetProgressState(TBPFLAG progressState);

    cComScope cs;

    HWND hwndWindow;

    cComObject<ITaskbarList3> taskBarList;
  };
}

#endif // LIBWIN32MM_TASKBAR_H
