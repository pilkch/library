// Windows headers
#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <commctrl.h>
#include <shellapi.h>

#undef min
#undef max

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
#include <libwin32mm/window.h>

namespace win32mm
{
  ULONG_PTR gGDIPlusToken = 0;

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

    // Init GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gGDIPlusToken, &gdiplusStartupInput, NULL);
  }

  void Destroy()
  {
    // Destroy GDI+
    Gdiplus::GdiplusShutdown(gGDIPlusToken);
  }


  void OpenWebPage(const cWindow& parent, const string_t& sWebPage)
  {
    // Launch this web page in the default browser
    SHELLEXECUTEINFO sh;
    memset(&sh, 0, sizeof(SHELLEXECUTEINFO));

    sh.cbSize = sizeof(SHELLEXECUTEINFO);
    sh.lpVerb = TEXT("open");
    sh.lpFile = sWebPage.c_str();
    sh.hwnd = parent.GetWindowHandle();
    sh.nShow = SW_NORMAL;

    ::ShellExecuteEx(&sh);
  }
}
