#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

namespace spitfire
{
  namespace operatingsystem
  {
    void GetOperatingSystemVersion(int& major, int& minor);

    string_t GetOperatingSystemNameString();
    string_t GetOperatingSystemVersionString();
    string_t GetOperatingSystemFullString();


    void ExecuteCommandLine(const string_t& sCommandLine);

    string_t GetUserName();

    bool GetEnvironmentVariable(const string_t& sEnvironmentVariable, string_t& sValue);

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
      DWORD dwNameSize = LSTR;
      ::GetComputerName(szComputerName, &dwNameSize);
    }

    inline void GetUserName(string_t& sUserName)
    {
      szUserName[0] = 0;
      DWORD dwNameSize = LSTR;
      ::GetUserName(szUserName, &dwNameSize);
    }
#endif

    bool IsUserRoot();

    inline uint32_t GetCurrentProcessId()
    {
      return getpid();
    }

#ifdef __APPLE__

    inline int32_t GetOSVersion();

    // Conversion from 12 to 0x12 for example
    inline uint8_t LBinaryCodedDecimal(uint8_t uValue)
    {
      return ((uValue / 10) << 4) | (uValue % 10);
    }

    // Conversion from 0x12 to 12 for example
    inline uint8_t LFromBinaryCodedDecimal(uint8_t uBCD)
    {
      return ((uBCD >> 4) * 10) + (uBCD & 0x0F);
    }

    inline void GetOSVersion(uint8_t& major, uint8_t& minor)
    {
      SInt32 osVersion = GetOSVersion;

      // Separate our major and minor versions
      major = LFromBinaryCodedDecimal(uint8_t(uint32_t(osVersion & 0x0000FF00) >> 8));
      minor = LFromBinaryCodedDecimal(osVersion & 0x000000FF);
    }

    // Conversion from 10, 4 to 0x1040
    inline uint16_t LMakeMacOSVersion(uint8_t uiMajor, uint8_t uiMinor)
    {
      uint16_t uiResult = uint16_t(LBinaryCodedDecimal(uiMajor)) << 8;

      // If uiMinor is a single digit then shift it into the form 0xi0
      if (uiMinor < 9) return uiResult | (uiMinor << 4);

      return uiResult | uint16_t(LBinaryCodedDecimal(uiMinor));
    }

    inline void GetOSVersion(string_t& sOut)
    {
      // Get our OS version
      uint8_t major = 0;
      uint8_t minor = 0;
      GetOSVersion(major, minor);

      // Now print out our version string
      if (major == 10) tsnprintf(szOut, LSTR, TEXT("Mac OS X %d.%d"), major, minor);
      else tsnprintf(szOut, LSTR, TEXT("Mac OS %d.%d"), major, minor);
    }

    inline void GetOSVersionShort(string_t& sOut)
    {
      uint8_t major = 0;
      uint8_t minor = 0;
      GetOSVersion(major, minor);

      sOut = major + "." + minor;
    }

    void OpenFolder(const string_t& sFolderPath);
#endif
  }
}

#endif // OPERATINGSYSTEM_H
