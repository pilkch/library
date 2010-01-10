// Standard headers
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

#ifdef __LINUX__
#include <errno.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <grp.h>
#include <pwd.h>
#endif

#ifdef __WIN__
#include <windows.h>
#endif

// libxdgmm headers
#include <libxdgmm/libxdg.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/lang.h>
#include <spitfire/util/log.h>

#include <spitfire/platform/operatingsystem.h>

#include <spitfire/algorithm/md5.h>

namespace spitfire
{
  namespace operatingsystem
  {
#ifdef __WIN__
    size_t GetProcessorCount()
    {
      SYSTEM_INFO si;
      GetSystemInfo(&si);

      return si.dwNumberOfProcessors;
    }

    size_t GetTotalProcessorCoreCount()
    {
      ... todo can we use GetLogicalProcessorInformation()?

      // Number of Logical Cores per Physical Processor
      int nCoreCount = 1;

      _asm {
        mov   eax, 1
        cpuid
        // Test for HTT bit
        test  edx, 0x10000000

        // If HTT is not supproted, goto htt_not_supported
        jz htt_not_supported

        // Multi-core or Hyperthreading supported...
        // Read the "# of Logical Processors per Physical Processor" field:
        mov   eax, ebx
        and   eax, 0x00FF0000 // Mask the "logical core counter" byte
        shr   eax, 16 // Shift byte to be least-significant
        mov   nCoreCount, eax

        // Uniprocessor (i.e. Pentium III or any AMD CPU excluding their new dual-core A64)
        htt_not_supported:
        // nCoreCount will contain 1.
      }


      // This gives a rough estimate, although theoretically someone might put a single core processor on a motherboard with say, 16 cores and we would return either 2 or 32 cores, ugh!
      return (GetProcessorCount() * size_t(nCoreCount));
    }
#else
    // http://www.sandpile.org/ia32/cpuid.htm

    size_t GetProcessorCount()
    {
      //SYSTEM_INFO si;
      //GetSystemInfo(&si);

      //return si.dwNumberOfProcessors;

      return 1;
    }

    size_t GetTotalProcessorCoreCount()
    {
      // Number of Logical Cores per Physical Processor
      int nCoreCount = 1;

      /*__asm (
        ".intel_syntax noprefix\n"

        "mov   eax, 1\n"
        "cpuid\n"
        // Test for HTT bit
        "test  edx, 0x10000000\n"

        // If HTT is not supproted, goto htt_not_supported
        "jz htt_not_supported\n"

        // Multi-core or Hyperthreading supported...
        // Read the "# of Logical Processors per Physical Processor" field:
        "mov   eax, ebx\n"
        "and   eax, 0x00FF0000\n" // Mask the "logical core counter" byte
        "shr   eax, 16\n" // Shift byte to be least-significant
        "mov   nCoreCount, eax\n"

        // Uniprocessor (i.e. Pentium III or any AMD CPU excluding their new dual-core A64)
        "htt_not_supported:\n"

        // nCoreCount will contain 1.

        ".att_syntax \n"
      );*/


      // This gives a rough estimate, although theoretically someone might put a single core processor on a motherboard with say, 16 cores and we would return either 2 or 32 cores, ugh!
      return (GetProcessorCount() * size_t(nCoreCount));
    }
#endif

#ifdef __LINUX__
    size_t GetRAMTotalMB()
    {
      struct sysinfo info;
      if (sysinfo(&info) != 0) return 0;

      return info.totalram;
    }
#endif

#ifdef PLATFORM_LINUX_OR_UNIX
    // *** Environment variables

    bool GetEnvironmentVariable(const string_t& sVariable, string_t& sValue)
    {
      sValue.clear();

      const char* szValue = getenv(spitfire::string::ToUTF8(sVariable).c_str());
      if (szValue == nullptr) return false;

      sValue = spitfire::string::ToString(szValue);
      return true;
    }

    bool SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue)
    {
      bool bOverwrite = true;
      int iOverwrite = bOverwrite ? 1 : 0;
      int result = setenv(spitfire::string::ToUTF8(sVariable).c_str(), spitfire::string::ToUTF8(sValue).c_str(), iOverwrite);
      if (result != 0) {
        LOG<<"SetEnvironmentVariable setenv FAILED "<<result<<std::endl;
        return false;
      }

      return true;
    }

    bool RemoveEnvironmentVariable(const string_t& sVariable)
    {
      int result = unsetenv(spitfire::string::ToUTF8(sVariable).c_str());
      if (result != 0) {
        LOG<<"RemoveEnvironmentVariable unsetenv FAILED "<<result<<std::endl;
        return false;
      }

      return true;
    }



    // ** IsUserAdministrator
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

        if (spitfire::string::ToString_t(grp->gr_name) == TEXT("admin")) return true;
      }

      return false;
    }

    bool IsUserRoot()
    {
      return (getuid() == 0); // If getuid is 0 then we are root
    }
#endif

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
      spitfire::char_t user[MAX_LENGTH];
      user[0] = 0;

      DWORD nSize = (DWORD)MAX_LENGTH;
      if (GetComputerName(user, &nSize)) return spitfire::string::ToUTF8(spitfire::string_t(user));

      if (getenv("USER") != 0) return string_t(getenv("USER"));
      else if (getenv("USERNAME") != 0) return string_t(getenv("USERNAME"));

      return TEXT("");
    }
#endif


#ifdef __LINUX__
    const size_t MAX_STRING_LENGTH = 512;

    // Run a command line in the background
    // We have no way of finding out the result
    void ExecuteCommand(const string_t& sCommand)
    {
      LOG<<"ExecuteCommand "<<sCommand<<std::endl;
      int pid = fork();
      if (pid != 0) return;

      char** szCommandAndParameters = nullptr;
      //makeargv(spitfire::string::ToUTF8(sCommand).c_str(), &szCommandAndParameters);
      execv(szCommandAndParameters[0], &szCommandAndParameters[0]);

      for (size_t i = 0; szCommandAndParameters[i] != nullptr; i++) LOG<<"ExecuteCommand FAILED "<<szCommandAndParameters[i]<<std::endl;

      LOG<<"ExecuteCommand FAILED error="<<strerror(errno)<<std::endl;
    }

    // Run a command line and wait for it to finish
    // return false on failure
    bool RunCommandLineProcess(const string_t& sCommandLine)
    {
      LOG<<"RunCommandLineProcess: "<<sCommandLine<<std::endl;
      int result = system(spitfire::string::ToUTF8(sCommandLine).c_str());
      if (result == -1) LOG<<"RunCommandLineProcess system FAILED result="<<strerror(errno)<<std::endl;
      return (WIFEXITED(result) && WEXITSTATUS(result) == 0);
    }


    void GetComputerName(string_t& sComputerName)
    {
      char szComputerName[MAX_STRING_LENGTH];
      gethostname(szComputerName, MAX_STRING_LENGTH);
      sComputerName = spitfire::string::ToString_t(szComputerName);
    }

    void GetUserName(string_t& sUserName)
    {
      // Try the environment variables first
      if (GetEnvironmentVariable(TEXT("LOGNAME"), sUserName)) return;
      if (GetEnvironmentVariable(TEXT("USER"), sUserName)) return;

      struct passwd *pw = getpwuid(getuid());
      ASSERT(pw != nullptr);
      sUserName.assign(spitfire::string::ToString_t(pw->pw_name)); // login name ie. "chris"
      sUserName.assign(spitfire::string::ToString_t(pw->pw_gecos)); // Real name is. "Chris Pilkington"

      LOG<<"GetUserName GetEnvironmentVariable FAILED Finding LOGNAME or USER"<<std::endl;
    }





    string_t GetUserDataDirectory()
    {
      std::string sFullPath;
      xdg::GetDataHomeDirectory(sFullPath);

      return string::ToString_t(sFullPath) + LANG("L_Application");
    }

    // *** Open functions

    //void OpenFile(const string_t& sFullPath)
    //{
    //    string_t sCommand = (IsRunningInKDE()) ? TEXT("kfmclient exec ") : TEXT("gnome-open ");
    //    sCommand += sFullPath;
    //    system(string::ToUTF8(sCommand).c_str());
    //}

    void OpenURL(const string_t& sURL)
    {
      int result = xdg::OpenURL(string::ToUTF8(sURL).c_str());
      if (result != 0) LOG<<"xdg::OpenFile returned "<<xdg::GetOpenErrorString(result)<<std::endl;
    }

    void OpenFile(const string_t& sFullPath)
    {
      int result = xdg::OpenFile(string::ToUTF8(sFullPath).c_str());
      if (result != 0) LOG<<"OpenFile xdg::OpenFile returned "<<xdg::GetOpenErrorString(result)<<std::endl;
    }

    void OpenFolder(const string_t& sFullPath)
    {
      int result = xdg::OpenFolder(string::ToUTF8(sFullPath).c_str());
      if (result != 0) LOG<<"OpenFolder xdg::OpenFile returned "<<xdg::GetOpenErrorString(result)<<std::endl;
    }
#endif

#ifdef __APPLE__
    //void GetUserHome(string_t& sUserHome)
    //{
    //  if (GetEnvironmentVariable(TEXT("HOME"), sUserHome)) return;
    //
    //  LOG<<"GetUserHome GetEnvironmentVariable FAILED Finding HOME"<<std::endl;
    //}




    void RevealInFinder(const string_t& sFilePath)
    {
      // TODO: Is this a constant in a header somewhere?
      const OSType finderSig = 'MACS';

      FSRef fsr;
      FSPathMakeRef((UInt8*)szFilePath, &fsr, NULL);

      AliasHandle alias = nil;
      OSErr err = FSNewAlias(nil, &fsr, &alias);
      require_noerr(err, Bail);

      HLock((Handle)alias);

      AEAddressDesc targetAddrDesc = { typeNull, nil };
      // Address target by signature
      err = AECreateDesc(typeApplSignature, &finderSig, sizeof(OSType), &targetAddrDesc);
      require_noerr(err, Bail);

      // Make the event
      AppleEvent theAppleEvent = { typeNull, nil };
      err = AECreateAppleEvent(kAEMiscStandards, kAEMakeObjectsVisible, &targetAddrDesc, kAutoGenerateReturnID, kAnyTransactionID, &theAppleEvent);
      require_noerr(err, Bail);

      err = AEPutParamPtr(&theAppleEvent, keyDirectObject, typeAlias, *alias, GetHandleSize((Handle) alias));
      require_noerr(err, Bail);

      // Send the event
      AppleEvent replyAppleEvent = { typeNull, nil };
      err = AESend(&theAppleEvent, &replyAppleEvent, kAENoReply, kAENormalPriority, kAEDefaultTimeout, NULL, NULL);

      {
        // Bring the finder to the front
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

    void OpenWebPage(const string_t& sWebPageURL)
    {
      ICInstance icInstance;
      OSType psiSignature = uint32_t('Psi ');
      OSStatus error = ICStart( &icInstance, psiSignature );
      if (error == noErr) {
        ConstStr255Param hint = 0x0;
        const char* data = spitfire::string::ToUTF8(sWebPageURL).get();
        long length = url.length();
        long start = 0;
        long end = length;
        ICLaunchURL(icInstance, hint, data, length, &start, &end);
        ICStop(icInstance);
      }
    }

    void GetComputerName(const string_t& sComputerName)
    {
      cCFString cfsComputerName(CSCopyMachineName());
      ASSERT(cfsComputerName.IsValid());
      cfsComputerName.GetCString(sComputerName);
    }


    bool GetBooleanValue(const string_t& section, const string_t& name, bool defaultValue)
    {
      string_t fullName(section + TEXT(".") + name);
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
      string_t fullName(section + TEXT(".") + name);
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
      string_t fullName(section + TEXT(".") + name);
      CFStringRef keyName = CFStringCreateWithCString(NULL, fullName.c_str(), kCFStringEncodingASCII);
      CFPreferencesSetAppValue(keyName, value ? kCFBooleanTrue : kCFBooleanFalse, kCFPreferencesCurrentApplication);
      CFRelease(keyName);
      CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
    }

    void SetIntegerValue(const string_t& section, const string_t& name, int value)
    {
      string_t fullName(section + TEXT(".") + name);
      CFStringRef keyName = CFStringCreateWithCString(NULL, fullName.c_str(), kCFStringEncodingASCII);
      CFNumberRef tempValue = CFNumberCreate(NULL, kCFNumberIntType, &value);
      CFPreferencesSetAppValue(keyName, tempValue, kCFPreferencesCurrentApplication);
      CFRelease(tempValue);
      CFRelease(keyName);
      CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
    }

    void SetStringValue(const string_t& section, const string_t& name, const string_t& value)
    {
      string_t fullName(section + TEXT(".") + name);
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
