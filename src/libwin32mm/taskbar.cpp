// Standard headers
#include <string.h>

// Windows headers
#include <windows.h>
#include <commctrl.h>
#include <comdef.h>

// libwin32mm headers
#include <libwin32mm/taskbar.h>

namespace win32mm
{
  // ** cTaskBar

  cTaskBar::cTaskBar() :
    hwndWindow(NULL)
  {
  }

  void cTaskBar::Init(cWindow& owner)
  {
    std::cout<<"LTaskbarButton::Init"<<std::endl;

    ASSERT(hwndWindow == NULL);
    hwndWindow = owner.GetWindowHandle();

    ASSERT(!taskBarList.IsValid());
    taskBarList.CreateObject((REFCLSID)CLSID_TaskbarList, IID_ITaskbarList3);
    ASSERT(taskBarList.IsValid());
  }

  bool cTaskBar::IsValid() const
  {
    return ((hwndWindow != NULL) && taskBarList.IsValid());
  }

  void cTaskBar::SetOverlayIcon(HICON hIcon, const string_t& sDescription)
  {
    std::cout<<"cTaskBar::SetOverlayIcon"<<std::endl;
    ASSERT(IsValid());
    taskBarList->SetOverlayIcon(hwndWindow, hIcon, LPCWSTR(sDescription.c_str()));
  }

  void cTaskBar::RemoveOverlayIcon()
  {
    std::cout<<"cTaskBar::RemoveOverlayIcon"<<std::endl;
    ASSERT(IsValid());
    taskBarList->SetOverlayIcon(hwndWindow, NULL, nullptr);
  }

  void cTaskBar::_SetProgressState(TBPFLAG progressState)
  {
     std::cout<<"cTaskBar::_SetProgressState"<<std::endl;
     ASSERT(taskBarList.IsValid());
     taskBarList->SetProgressState(hwndWindow, (TBPFLAG)progressState);
  }

  void cTaskBar::SetProgressOff()
  {
     _SetProgressState(TBPF_NOPROGRESS);
  }

  void cTaskBar::SetProgress0To100(float fProgress0To100)
  {
    ASSERT(IsValid());
    taskBarList->SetProgressValue(hwndWindow, int(fProgress0To100), 100LL);
  }

  void cTaskBar::SetProgressIndeterminateOn()
  {
     _SetProgressState(TBPF_INDETERMINATE);
  }

  void cTaskBar::SetProgressIndeterminateOff()
  {
     _SetProgressState(TBPF_NORMAL);
  }

  void cTaskBar::SetProgressPause()
  {
     _SetProgressState(TBPF_PAUSED);
  }

  void cTaskBar::SetProgressError()
  {
     _SetProgressState(TBPF_ERROR);
  }
}
