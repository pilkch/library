#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

// Spitfire headers
#include <spitfire/algorithm/algorithm.h>
#include <spitfire/util/log.h>

namespace spitfire
{
  namespace operatingsystem
  {
    size_t GetProcessorCount();
    size_t GetTotalProcessorCoreCount();

    size_t GetMemoryTotalMB();
    size_t GetMemoryUsedByApplicationMB();

#ifdef __WIN__
    //void AddLaunchIconToDesktop(); // We intentionally do not try to add a application icon to a Windows desktop because that looks tacky
    void AddLaunchIconToPanel();
    void AddLaunchIconToMenu();
#elif defined(__GTK__)
    //void AddLaunchIconToDesktop(); // We intentionally do not try to add a application icon to a gtk desktop because that looks tacky
    void AddLaunchIconToPanel();
    void AddLaunchIconToMenu();
#elif defined(__APPLE__)
    //void AddLaunchIconToDesktop(); // We intentionally do not try to add a application icon to a mac desktop because that is never done by anyone
    void AddLaunchIconToPanel();
    //void AddLaunchIconToMenu(); // We intentionally do not try to add a application to /Applications
#endif



#ifdef __WIN__
    const size_t MAX_STRING_LENGTH = 1024;

    // *** Environment variables

    inline bool GetEnvironmentVariable(const string_t& sVariable, string_t& sValue)
    {
      char_t szValue[MAX_STRING_LENGTH];
      ::GetEnvironmentVariable(sVariable.c_str(), szValue, MAX_STRING_LENGTH);
      sValue = szValue;
    }

    inline bool SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue)
    {
      ::SetEnvironmentVariable(sVariable.c_str(), sValue.c_str());
    }

    inline bool RemoveEnvironmentVariable(const string_t& sVariable)
    {
      ::SetEnvironmentVariable(sVariable.c_str(), nullptr);
    }
#elif defined(PLATFORM_LINUX_OR_UNIX)
    // *** Environment variables

    bool GetEnvironmentVariable(const string_t& sVariable, string_t& sValue);
    bool SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue);
    bool RemoveEnvironmentVariable(const string_t& sVariable);
#endif

    #ifdef __WIN__
    inline bool IsRunningUnderWine()
    {
      // Wine has a registry key we can test
      HKEY hKey;
      if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Wine"), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) return false;
      RegCloseKey(hKey);
      return true;
    }
    #endif

#ifdef __LINUX__
    // *** Linux specific version functions

    inline bool IsRunningInGnome()
    {
        return (getenv("GNOME_DESKTOP_SESSION_ID") != nullptr);
    }

    inline bool IsRunningInKDE()
    {
        return (getenv("KDE_FULL_SESSION") != nullptr);
    }
#endif

    // *** Open functions

    void OpenFile(const string_t& sFullPath);

    inline void OpenTextFile(const string_t& sFullPath) { OpenFile(sFullPath); }

    void OpenURL(const string_t& sURL);

    void ShowFile(const string_t& sFilePath);     // Opens the file navigator to show where this file is
    void ShowFolder(const string_t& sFolderPath); // Opens the file navigator to show where this folder is

    void GetOperatingSystemVersion(int& major, int& minor);

    string_t GetOperatingSystemNameString();
    string_t GetOperatingSystemVersionString();
    string_t GetOperatingSystemFullString();


    // Run a command line in the background
    // We have no way of finding out the result
    void ExecuteCommand(const string_t& sCommand);

    // Run a command line and wait for it to finish
    // return false on failure
    bool RunCommandLineProcess(const string_t& sCommandLine);


    string_t GetUserName();

#ifdef __WIN__
    inline void OpenURL(const string_t& sURL)
    {
      ShellExecute(NULL, NULL, sURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

    inline void OpenFolder(const string_t& sFolderPath)
    {
      if (uint32_t(ShellExecute(NULL, TEXT("explore"), sFolderPath.c_str(), NULL, NULL, SW_SHOWNORMAL)) >= 32) return;

      string_t sCommand(TEXT("explorer.exe \"") + sFolderPath + TEXT("\""));
      STARTUPINFO siStartInfo;
      ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
      siStartInfo.cb = sizeof(STARTUPINFO);
      siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
      siStartInfo.wShowWindow = SW_NORMAL;

      PROCESS_INFORMATION piProcInfo;
      ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

      BOOL bResult = CreateProcess(NULL, &sCommand[0], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, sFolderPath.c_str(), &siStartInfo, &piProcInfo);
      if (bResult != TRUE) LOG<<"OpenFolder unable to find explorer.exe"<<std::endl;
      CloseHandle(piProcInfo.hThread);
      CloseHandle(piProcInfo.hProcess);
    }

    inline void GetComputerName(string_t& sComputerName)
    {
      sComputerName[0] = 0;

      char_t szComputerName[MAX_STRING_LENGTH];
      DWORD dwNameSize = MAX_STRING_LENGTH;
      ::GetComputerName(szComputerName, &dwNameSize);
      sComputerName = szComputerName;
    }

    inline void GetUserName(string_t& sUserName)
    {
      sUserName[0] = 0;

      char_t szUserName[MAX_STRING_LENGTH];
      DWORD dwNameSize = MAX_STRING_LENGTH;
      ::GetUserName(szUserName, &dwNameSize);
      sUserName = szUserName;
    }
#endif

    bool IsUserAdministrator();
    bool IsUserRoot();

    inline uint32_t GetCurrentProcessId()
    {
      #ifdef __WIN__
      return ::GetCurrentProcessId();
      #else
      return getpid();
      #endif
    }

    bool IsOSVersion(uint8_t major);
    bool IsOSVersion(uint8_t major, uint8_t minor);
    bool IsOSVersionOrLater(uint8_t major);
    bool IsOSVersionOrLater(uint8_t major, uint8_t minor);

    void GetOSVersion(uint8_t& major);
    void GetOSVersion(uint8_t& major, uint8_t& minor);
    string_t GetOSVersionString();
    string_t GetOSVersionStringShort();

    #ifdef __WIN__
    bool IsWindows();
    bool IsWindowsXP();
    bool IsWindowsVista();
    inline bool IsWindows7() { return IsOSVersion(7); }
    bool IsWindowsXPOrLater();
    bool IsWindowsVistaOrLater();
    inline bool IsWindows7OrLater() { return IsOSVersionOrLater(7); }
    #elif defined(__LINUX__)
    bool IsUbuntu();
    bool IsUbuntu10_4();
    bool IsUbuntu10_10();
    bool IsUbuntu11_4();
    bool IsUbuntu11_10();
    bool IsUbuntu10_4Orlater();
    bool IsUbuntu10_10Orlater();
    bool IsUbuntu11_4Orlater();
    bool IsUbuntu11_10Orlater();
    bool IsFedora();
    bool IsFedora12();
    bool IsFedora13();
    bool IsFedora14();
    bool IsFedora15();
    bool IsFedora16();
    bool IsFedora12OrLater();
    bool IsFedora13OrLater();
    bool IsFedora14OrLater();
    bool IsFedora15OrLater();
    bool IsFedora16OrLater();
    #elif defined(__APPLE__)
    inline bool IsMacOS10_4Tiger() { return IsOSVersion(10, 4); }
    inline bool IsMacOS10_5Leopard() { return IsOSVersion(10, 5); }
    inline bool IsMacOS10_6SnowLeopard() { return IsOSVersion(10, 6); }
    inline bool IsMacOS10_7Lion() { return IsOSVersion(10, 7); }
    inline bool IsMacOS10_4TigerOrLater() { return IsOSVersionOrLater(10, 4); }
    inline bool IsMacOS10_5LeopardOrLater() { return IsOSVersionOrLater(10, 5); }
    inline bool IsMacOS10_6SnowLeopardOrLater() { return IsOSVersionOrLater(10, 6); }
    inline bool IsMacOS10_7LionOrLater() { return IsOSVersionOrLater(10, 7); }
    #endif

#ifdef __APPLE__
    inline int32_t GetOSVersion();

    inline void GetOSVersion(uint8_t& major, uint8_t& minor)
    {
      SInt32 osVersion = GetOSVersion;

      // Separate our major and minor versions
      major = FromBinaryCodedDecimal(uint8_t(uint32_t(osVersion & 0x0000FF00) >> 8));
      minor = FromBinaryCodedDecimal(osVersion & 0x000000FF);
    }

    // Conversion from 10, 4 to 0x1040
    inline uint16_t MakeMacOSVersion(uint8_t uiMajor, uint8_t uiMinor)
    {
      uint16_t uiResult = uint16_t(algorithm::ToBinaryCodedDecimal(uiMajor)) << 8;

      // If uiMinor is a single digit then shift it into the form 0xi0
      if (uiMinor < 9) return uiResult | (uiMinor << 4);

      return uiResult | uint16_t(algorithm::ToBinaryCodedDecimal(uiMinor));
    }

    inline void GetOSVersion(string_t& sOut)
    {
      // Get our OS version
      uint8_t major = 0;
      uint8_t minor = 0;
      GetOSVersion(major, minor);

      // Now print out our version string
      ostringstream_t o;
      o<<TEXT("Mac OS ");
      if (major == 10) o<<TEXT("X ");
      o<<major;
      o<<minor;

      sOut = o.str();
    }

    inline void GetOSVersionShort(string_t& sOut)
    {
      uint8_t major = 0;
      uint8_t minor = 0;
      GetOSVersion(major, minor);

      sOut = major + TEXT(".") + minor;
    }

    void OpenFolder(const string_t& sFolderPath);

    bool IsBundled();
#endif
  }
}

#endif // OPERATINGSYSTEM_H
