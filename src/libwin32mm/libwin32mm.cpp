#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

// Turn on Visual Styles
// http://msdn.microsoft.com/en-us/library/windows/desktop/bb773175%28v=vs.85%29.aspx

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <libwin32mm/libwin32mm.h>
#include <libwin32mm/dynamiclibrary.h>

namespace win32mm
{
  void Init()
  {
    cDLL dll(TEXT("user32.dll"));
    if (dll.IsValid()) {
      BOOL (WINAPI *SetProcessDPIAware)() = nullptr;
      if (dll.LoadFunction("SetProcessDPIAware", &SetProcessDPIAware)) {
        std::wcout<<TEXT("WinMain Calling SetProcessDPIAware()")<<std::endl;
        SetProcessDPIAware();
      }
    }

    // Init common controls for modern looking theming
    INITCOMMONCONTROLSEX icce;
    icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icce.dwICC =
      ICC_USEREX_CLASSES | // Combobox
      ICC_STANDARD_CLASSES // All other controls
    ;
    ::InitCommonControlsEx(&icce);
  }
}
