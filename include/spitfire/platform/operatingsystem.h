#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

namespace spitfire
{
  namespace operatingsystem
  {
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
    // *** Environment variables

    inline bool GetEnvironmentVariable(const string_t& sVariable, string_t& sValue)
    {
      char_t szValue[MAX_STRING_LENGTH];
      ::GetEnvironmentVariable(spitfire::string::ToUTF8(sVariable).c_str(), szValue, MAX_STRING_LENGTH);
      sValue = szValue;
    }

    inline bool SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue)
    {
      ::SetEnvironmentVariable(spitfire::string::ToUTF8(sVariable).c_str(), spitfire::string::ToUTF8(sValue).c_str());
    }

    inline bool RemoveEnvironmentVariable(const string_t& sVariable)
    {
      ::SetEnvironmentVariable(spitfire::string::ToUTF8(sVariable).c_str(), nullptr);
    }
#elif defined(PLATFORM_LINUX_OR_UNIX)
    // *** Environment variables

    bool GetEnvironmentVariable(const string_t& sVariable, string_t& sValue);
    bool SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue);
    bool RemoveEnvironmentVariable(const string_t& sVariable);
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

    inline void OpenTextFile(const string_t& sFullPath)
    {
        OpenFile(sFullPath);
    }

    void OpenFolder(const string_t& sFullPath);
    void OpenWebPage(const string_t& sWebPageURL);

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
    inline void OpenWebPage(const string_t& sWebPageURL)
    {
      ShellExecute(NULL, NULL, sWebPageURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

    inline void OpenFolder(const string_t& sFolderPath)
    {
      if (uint32_t(ShellExecute(NULL, TEXT("explore"), sFolderPath.c_str(), NULL, NULL, SW_SHOWNORMAL)) >= 32) return;

      if (!IsOSWine()) {
        LDEBUG("OpenFolder ShellExecute FAILED");
        return;
      }

      const string_t sCommand(TEXT("explorer.exe \"") + sFolderPath + TEXT("\""));
      STARTUPINFO siStartInfo;
      ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
      siStartInfo.cb = sizeof(STARTUPINFO);
      siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
      siStartInfo.wShowWindow = SW_NORMAL;

      PROCESS_INFORMATION piProcInfo;
      ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

      BOOL bResult = CreateProcess(NULL, sCommand.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, szFolder, &siStartInfo, &piProcInfo);
      if (bResult != TRUE) LOG<<"OpenFolder unable to find explorer.exe"<<std::endl;
      CloseHandle(piProcInfo.hThread);
      CloseHandle(piProcInfo.hProcess);
    }

    inline void GetComputerName(string_t& sComputerName)
    {
      szComputerName[0] = 0;
      DWORD dwNameSize = MAX_STRING_LENGTH;
      ::GetComputerName(szComputerName, &dwNameSize);
    }

    inline void GetUserName(string_t& sUserName)
    {
      szUserName[0] = 0;
      DWORD dwNameSize = MAX_STRING_LENGTH;
      ::GetUserName(szUserName, &dwNameSize);
    }
#endif

    bool IsUserAdministrator();
    bool IsUserRoot();

    inline uint32_t GetCurrentProcessId()
    {
      return getpid();
    }

#ifdef __APPLE__

    inline int32_t GetOSVersion();

    // Conversion from 12 to 0x12 for example
    inline uint8_t ToBinaryCodedDecimal(uint8_t uValue)
    {
      return ((uValue / 10) << 4) | (uValue % 10);
    }

    // Conversion from 0x12 to 12 for example
    inline uint8_t FromBinaryCodedDecimal(uint8_t uBCD)
    {
      return ((uBCD >> 4) * 10) + (uBCD & 0x0F);
    }

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
      uint16_t uiResult = uint16_t(ToBinaryCodedDecimal(uiMajor)) << 8;

      // If uiMinor is a single digit then shift it into the form 0xi0
      if (uiMinor < 9) return uiResult | (uiMinor << 4);

      return uiResult | uint16_t(ToBinaryCodedDecimal(uiMinor));
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
#endif
  }
}

#endif // OPERATINGSYSTEM_H
