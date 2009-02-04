#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/operatingsystem.h>

#include <breathe/algorithm/md5.h>

#ifdef __LINUX__
#include <errno.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#endif

#ifdef __WIN__
#include <windows.h>
#endif

namespace breathe
{
  namespace operatingsystem
  {
    string_t GetOperatingSystemNameString()
    {
#if defined(__WIN__)
      return TEXT("Windows");
#elif defined(__LINUX__)
      return TEXT("Linux");
#elif defined(__APPLE__)
  #if defined(__MACOSX__)
      return TEXT("MacOS X");
  #else
      return TEXT("MacOS Unknown");
  #endif
#else
      return TEXT("Unknown");
#endif
    }

#ifdef __APPLE__
    int32_t GetOSVersion()
    {
      SInt32 osVersion;
      Gestalt(gestaltSystemVersion, &osVersion);
      return osVersion;
    }
#endif

    void GetOperatingSystemVersion(int& major, int& minor)
    {
#ifdef __WIN__
      OSVERSIONINFO vi;
      vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      GetVersionEx(&vi);
      major = vi.dwMajorVersion;
      minor = vi.dwMinorVersion;
#else
      major = 0;
      minor = 0;
#endif
    }

    string_t GetOperatingSystemVersionString()
    {
      int major = 0;
      int minor = 0;
      GetOperatingSystemVersion(major, minor);

      ostringstream_t o;
      o<<major<<TEXT(".")<<minor;
      return o.str();
    }

    string_t GetOperatingSystemFullString()
    {
      string_t s = TEXT("Unknown ") + GetOperatingSystemVersionString();

#ifdef __WIN__
      int major = GetOperatingSystemVersionMajor();
      int minor = GetOperatingSystemVersionMinor();

      if (4 == major) {
        if (0 == minor) s = "95";
        else if (1 == minor) s = "98";
        else if (9 == minor) s = "ME";
      }
      else if (5 == major) {
        if (0 == minor) s = "2000";
        else if (1 == minor) s = "XP";
        else if (2 == minor) s = "Server 2003";
      }
      else if (6 == major) {
        if (0 == minor) s = "Vista";
      }

      // Check if we are running in Wine
      HKEY hKey;
      if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Wine"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        s += " under Wine";
      }
#endif

      return GetOperatingSystemNameString() + TEXT(" ") + s;
    }

#ifdef __WIN__
    string_t GetUserName()
    {
      const int MAX_LENGTH = 260;
      breathe::char_t user[MAX_LENGTH];
      user[0] = 0;

      DWORD nSize = (DWORD)MAX_LENGTH;
      if (GetComputerName(user, &nSize)) return breathe::string::ToUTF8(breathe::string_t(user));

      if (getenv("USER") != 0) return string_t(getenv("USER"));
      else if (getenv("USERNAME") != 0) return string_t(getenv("USERNAME"));

      return TEXT("<UNKNOWN>");
    }
#endif


#ifdef __LINUX__
    const size_t MAX_STRING_LENGTH = 512;

    void ExecuteCommand(const string_t& sCommand)
    {
      LOG<<"ExecuteCommand "<<sCommand<<std::endl;
      int pid = fork();
      if (pid != 0) return;

      char** szCommandAndParameters = nullptr;
      //makeargv(breathe::string::ToUTF8(sCommand).c_str(), &szCommandAndParameters);
      execv(szCommandAndParameters[0], &szCommandAndParameters[0]);

      for (size_t i = 0; szCommandAndParameters[i] != nullptr; i++) LOG<<"ExecuteCommand FAILED "<<szCommandAndParameters[i]<<std::endl;

      LOG<<"ExecuteCommand FAILED error="<<strerror(errno)<<std::endl;
    }

    // Run a command line and wait for it to finish
    // return false on failure
    bool RunCommandLineProcess(const string_t& sCommandLine)
    {
      LOG<<"RunCommandLineProcess: "<<sCommandLine<<std::endl;
      int result = system(breathe::string::ToUTF8(sCommandLine).c_str());
      if (result == -1) LOG<<"RunCommandLineProcess system FAILED result="<<strerror(errno)<<std::endl;
      return (WIFEXITED(result) && WEXITSTATUS(result) == 0);
    }

    bool IsOSGnome()
    {
      return getenv("GNOME_DESKTOP_SESSION_ID") != nullptr;
    }

    bool IsOSKDE()
    {
      return getenv("KDE_FULL_SESSION") != nullptr;
    }

    void OpenWebPage(const string_t& sWebPageURL)
    {
      string_t sCommand;
      if (IsOSKDE()) sCommand = TEXT("kfmclient exec \"") + sWebPageURL + TEXT("\"");
      else sCommand = TEXT("gnome-open \"") + sWebPageURL + TEXT("\"");

      system(breathe::string::ToUTF8(sCommand).c_str());
    }

    void GetComputerName(string_t& sComputerName)
    {
      char szComputerName[MAX_STRING_LENGTH];
      gethostname(szComputerName, MAX_STRING_LENGTH);
      sComputerName = breathe::string::ToString_t(szComputerName);
    }

    bool GetEnvironmentVariable(const string_t& sEnvironmentVariable, string_t& sValue)
    {
      sValue.clear();

      const char* szResult = getenv(breathe::string::ToUTF8(sEnvironmentVariable).c_str());
      if (szResult == nullptr) return false;

      sValue = breathe::string::ToString_t(szResult);
      return true;
    }

    void GetUserName(string_t& sUserName)
    {
      // Try the environment variables first
      if (GetEnvironmentVariable(TEXT("LOGNAME"), sUserName)) return;
      if (GetEnvironmentVariable(TEXT("USER"), sUserName)) return;

      struct passwd *pw = getpwuid(getuid());
      ASSERT(pw != nullptr);
      sUserName.assign(breathe::string::ToString_t(pw->pw_name)); // login name ie. "chris"
      sUserName.assign(breathe::string::ToString_t(pw->pw_gecos)); // Real name is. "Chris Pilkington"

      LOG<<"GetUserName GetEnvironmentVariable FAILED Finding LOGNAME or USER"<<std::endl;
    }

    void GetUserHome(string_t& sUserHome)
    {
      if (GetEnvironmentVariable(TEXT("HOME"), sUserHome)) return;

      LOG<<"GetUserHome GetEnvironmentVariable FAILED Finding HOME"<<std::endl;
    }

    bool IsUserRoot()
    {
      string_t sUser;
      GetEnvironmentVariable(TEXT("USER"), sUser);
      return ((getuid() == 0) || (sUser == TEXT("root")) == 0); // Check if we are root
    }


    // ** LIsUserAdministrator
    // Returns true if the user is a member of group admin

    bool IsUserAdministrator()
    {
      const long ngroups_max = sysconf(_SC_NGROUPS_MAX) + 1;
      if (ngroups_max == 0) return false;

      // Create the space to hold the groups and fill them out
      std::vector<gid_t> group_list(ngroups_max, nullptr);

      const size_t n = getgroups(ngroups_max, &group_list[0]);

      // Go through the list of groups
      LOG<<"IsUserAdministrator Groups"<<std::endl;
      for (size_t i = 0; i < n; i++) {
        struct group* grp = getgrgid(group_list[i]);
        LOG<<"IsUserAdministrator "<<(int)group_list[i]<<":"<<grp->gr_name<<std::endl;

        if (breathe::string::ToString_t(grp->gr_name) == TEXT("admin")) return true;
      }

      return false;
    }
#endif

#ifdef __APPLE__
    void RevealInFinder(const string_t& sFilePath)
    {
      const OSType finderSig = 'MACS';

      FSRef fsr;
      FSPathMakeRef((UInt8*)szFilePath, &fsr, NULL);

      AliasHandle alias = nil;
      OSErr err = FSNewAlias(nil, &fsr, &alias);
      require_noerr(err, Bail);

      HLock((Handle)alias);

      AEAddressDesc targetAddrDesc = { typeNull, nil };
      // address target by signature
      err = AECreateDesc(typeApplSignature, &finderSig, sizeof(OSType), &targetAddrDesc);
      require_noerr(err, Bail);

      // make the event
      AppleEvent theAppleEvent = { typeNull, nil };
      err = AECreateAppleEvent(kAEMiscStandards, kAEMakeObjectsVisible, &targetAddrDesc, kAutoGenerateReturnID, kAnyTransactionID, &theAppleEvent);
      require_noerr(err, Bail);

      err = AEPutParamPtr(&theAppleEvent, keyDirectObject, typeAlias, *alias, GetHandleSize((Handle) alias));
      require_noerr(err, Bail);

      // send it
      AppleEvent replyAppleEvent = { typeNull, nil };
      err = AESend(&theAppleEvent, &replyAppleEvent, kAENoReply, kAENormalPriority, kAEDefaultTimeout, NULL, NULL);

      {
        // bring the finder to the front
        const OSType finderType = 'FNDR';
        ProcessSerialNumber finderPSN;
        if (GetPSNFromSignature(finderSig, finderType, &finderPSN)) SetFrontProcess(&finderPSN);
      }

Bail:
      if (alias != NULL) DisposeHandle((Handle)alias);
      if (targetAddrDesc.dataHandle != NULL) AEDisposeDesc(&targetAddrDesc);
      if (theAppleEvent.dataHandle != NULL) AEDisposeDesc(&theAppleEvent);
    }


    void OpenFolder(const string_t& sFilePath)
    {
      FSRef fsr;
      FSPathMakeRef((UInt8*)szFilePath, &fsr, NULL);
      LSOpenFSRef(&fsr, NULL);
    }

    void GetComputerName(const string_t& sComputerName)
    {
      cCFString cfsComputerName(CSCopyMachineName());
      ASSERT(cfsComputerName.IsValid());
      cfsComputerName.GetCString(sComputerName);
    }


    bool GetBooleanValue(const string_t& section, const string_t& name, bool defaultValue)
    {
      string_t fullName(section + "." + name);
      CFStringRef keyName = CFStringCreateWithCString(NULL, fullName.c_str(), kCFStringEncodingASCII);
      bool value = defaultValue;

      CFBooleanRef tempValue = (CFBooleanRef)CFPreferencesCopyAppValue(keyName, kCFPreferencesCurrentApplication);
      if (tempValue != NULL) {
        value = CFBooleanGetValue(tempValue);
        CFRelease(tempValue);
      }
      CFRelease(keyName);

      return value;
    }

    int GetIntegerValue(const string_t& section, const string_t& name, int defaultValue)
    {
      string_t fullName(section + "." + name);
      CFStringRef keyName = CFStringCreateWithCString(NULL, fullName.c_str(), kCFStringEncodingASCII);
      int value = defaultValue;

      CFNumberRef tempValue = (CFNumberRef)CFPreferencesCopyAppValue(keyName, kCFPreferencesCurrentApplication);
      if ( tempValue ) {
        CFNumberGetValue(tempValue, kCFNumberIntType, &value);
        CFRelease(tempValue);
      }
      CFRelease(keyName);

      return value;
    }

    string_t GetStringValue(const string_t& section, const string_t& name, const string_t& defaultValue)
    {
      string_t fullName(section + "." + name);
      CFStringRef keyName = CFStringCreateWithCString(NULL, fullName.c_str(), kCFStringEncodingASCII);

      string_t value = defaultValue;

      CFStringRef tempValue = (CFStringRef)CFPreferencesCopyAppValue(keyName, kCFPreferencesCurrentApplication);
      if (tempValue) {
        unsigned int valueLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(tempValue), kCFStringEncodingASCII) + 1;
        char *valueString = new char[valueLength];
        CFStringGetCString(tempValue, valueString, valueLength, kCFStringEncodingASCII);
        value = string_t(valueString);
        SAFE_DELETE_ARRAY(valueString);
        CFRelease(tempValue);
      }
      CFRelease(keyName);

      return value;
    }

    void SetBooleanValue(const string_t& section, const string_t& name, bool value)
    {
      string_t fullName(section + "." + name);
      CFStringRef keyName = CFStringCreateWithCString(NULL, fullName.c_str(), kCFStringEncodingASCII);
      CFPreferencesSetAppValue(keyName, value ? kCFBooleanTrue : kCFBooleanFalse, kCFPreferencesCurrentApplication);
      CFRelease(keyName);
      CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
    }

    void SetIntegerValue(const string_t& section, const string_t& name, int value)
    {
      string_t fullName(section + "." + name);
      CFStringRef keyName = CFStringCreateWithCString(NULL, fullName.c_str(), kCFStringEncodingASCII);
      CFNumberRef tempValue = CFNumberCreate(NULL, kCFNumberIntType, &value);
      CFPreferencesSetAppValue(keyName, tempValue, kCFPreferencesCurrentApplication);
      CFRelease(tempValue);
      CFRelease(keyName);
      CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
    }

    void SetStringValue(const string_t& section, const string_t& name, const string_t& value)
    {
      string_t fullName(section + "." + name);
      CFStringRef keyName = CFStringCreateWithCString(NULL, fullName.c_str(), kCFStringEncodingASCII);
      CFStringRef tempValue = CFStringCreateWithCString(NULL, value.c_str(), kCFStringEncodingASCII);
      CFPreferencesSetAppValue(keyName, tempValue, kCFPreferencesCurrentApplication);
      CFRelease(tempValue);
      CFRelease(keyName);
      CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
    }
#endif
  }
}