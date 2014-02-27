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

#include <thread>

#ifdef __LINUX__
#include <errno.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
//#include <sys/resource.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#endif

#ifdef __WIN__
#include <windows.h>
#endif

#ifdef __LINUX__
// libxdgmm headers
#include <libxdgmm/libxdgmm.h>
#endif

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/platform/dynamiclibrary.h>
#include <spitfire/util/string.h>
#include <spitfire/util/lang.h>
#include <spitfire/util/log.h>

#include <spitfire/platform/operatingsystem.h>

#include <spitfire/algorithm/md5.h>

namespace spitfire
{
  namespace operatingsystem
  {
    size_t GetProcessorCount()
    {
      #ifdef __WIN__
      SYSTEM_INFO si;
      GetSystemInfo(&si);
      return si.dwNumberOfProcessors;
      #elif defined(_SC_NPROCESSORS_CONF)
      return sysconf(_SC_NPROCESSORS_CONF);
      #elif defined(__APPLE__)
      int iaMib[2] = { CTL_HW, HW_NCPU };
      int nProcessorCount;
      size_t len = sizeof(nProcessorCount);
      if (sysctl(iaMib, 2, &nProcessorCount, &len, NULL, 0) == 0) return nProcessorCount;
      return 1;
      #else
      #error "GetProcessorCount has not been implemented on this platform"
      #endif
    }

    size_t GetProcessorCoreCount()
    {
      const size_t nConcurrency = static_cast<size_t>(boost::thread::hardware_concurrency());

      return max(nConcurrency, 1);
    }

    #ifdef __WIN__

    size_t GetMemoryTotalMB()
    {
      // http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
      MEMORYSTATUSEX memoryStatus;
      memoryStatus.dwLength = sizeof(memoryStatus);
      GlobalMemoryStatusEx(&memoryStatus);

      return static_cast<size_t>(memoryStatus.ullTotalPhys / 1024 / 1024);
    }
    
    uint64_t GetMemoryUsedByApplicationMB()
    {
      // Get the virtual memory size from the process memory info
      // http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
      PROCESS_MEMORY_COUNTERS_EX memoryCounters;
      GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memoryCounters), sizeof(memoryCounters));

      return memoryCounters.PrivateUsage / 1024 / 1024;
    }

    #elif defined(__LINUX__)

    uint64_t GetSystemTotalPhysicalMemorySizeMb()
    {
      // http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process

      struct sysinfo sysInfo;
      sysinfo(&sysInfo);

      return static_cast<uint64_t>(sysInfo.totalram) / 1024 / 1024;
    }

    size_t GetMemoryUsedByApplicationMB()
    {
      using std::ios_base;
      using std::ifstream;

      // 'file' stat seems to give the most reliable results
      //
      ifstream stat_stream("/proc/self/stat", ios_base::in);

      // dummy vars for leading entries in stat that we don't care about
      //
      std::string pid, comm, state, ppid, pgrp, session, tty_nr;
      std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
      std::string utime, stime, cutime, cstime, priority, nice;
      std::string dummy, itrealvalue, starttime;

      // the two fields we want
      //
      unsigned long vsize = 0;
      long rss = 0;

      stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                  >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                  >> utime >> stime >> cutime >> cstime >> priority >> nice
                  >> dummy >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

      //long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
      double vm_usage_mb = vsize / 1024.0 / 1024.0;
      //double resident_set_mb = rss * page_size_kb / 1024.0;

      return size_t(vm_usage_mb);

      // getrusage is broken on Linux still
      //int who = RUSAGE_SELF;
      //struct rusage usage;
      //if (getrusage(who, &usage) != 0) return 0;

      //return usage.;
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
      std::vector<gid_t> group_list(ngroups_max, 0);

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

    bool IsOS64Bit()
    {
      #ifdef BUILD_PLATFORM_64
      return true;
      #elif defined(__WIN__)
      typedef BOOL (WINAPI* PFNISWOW64PROCESS)(HANDLE, PBOOL);
      cDynamicLibrary dll;
      dll.LoadFromAnywhere(TEXT("kernel32.dll"));
      PFNISWOW64PROCESS pIsWow64Process = nullptr;
      if (!dll.LoadFunction("IsWow64Process", &pIsWow64Process)) return false;
      BOOL bIsWow64 = FALSE;
      pIsWow64Process(GetCurrentProcess(), &bIsWow64);
      IsWow64Process(GetCurrentProcess(), &bIsWow64);
      return (bIsWow64 != FALSE);
      #else
      #error "IsOS64Bit hasn't been implemented"
      #endif
    }

    #ifdef __WIN__
    bool IsWindowsRunningUnderWine()
    {
      // We know that we are running under Wine if the function wine_get_unix_file_name exists
      typedef BOOL (WINAPI* PFNwine_get_unix_file_name)(LPCWSTR, LPSTR, DWORD);
      cDynamicLibrary dll;
      dll.LoadFromAnywhere(TEXT("kernel32.dll"));
      PFNwine_get_unix_file_name pwine_get_unix_file_name = nullptr;
      return dll.LoadFunction("wine_get_unix_file_name", &pwine_get_unix_file_name);
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

    /*void GetOSVersion(uint8_t& major);
    void GetOSVersion(uint8_t& major, uint8_t& minor);
    string_t GetOSVersionString();
    string_t GetOSVersionStringShort();

    ubuntu

    lsb_release -a – get Ubuntu version
    uname -r – get kernel version
    uname -a - get kernal information*/

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
      int major = 0;
      int minor = 0;
      GetOperatingSystemVersion(major, minor);

      if (4 == major) {
        if (0 == minor) s = TEXT("95");
        else if (1 == minor) s = TEXT("98");
        else if (9 >= minor) s = TEXT("ME");
      } else if (5 == major) {
        if (0 == minor) s = TEXT("2000");
        else if (1 == minor) s = TEXT("XP");
        else if (2 >= minor) s = TEXT("Server 2003");
      } else if (6 == major) {
        if (0 == minor) s = TEXT("Vista");
        else if (minor == 1) s = TEXT("Windows 7");
        else if (minor >= 2) s = TEXT("Windows 8");
      }

      // Check if we are running in Wine
      if (IsWindowsRunningUnderWine()) s += TEXT(" under Wine");
      #else
      #error "GetOperatingSystemFullString"
#endif

      return GetOperatingSystemNameString() + TEXT(" ") + s;
    }

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
      xdg::cXdg xdg;
      const std::string sFullPath = xdg.GetHomeDataDirectory();
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
      xdg::cXdg xdg;
      int result = xdg.OpenURL(string::ToUTF8(sURL).c_str());
      if (result != 0) LOG<<"xdg::OpenFile returned "<<xdg.GetOpenErrorString(result)<<std::endl;
    }

    void OpenFile(const string_t& sFullPath)
    {
      xdg::cXdg xdg;
      int result = xdg.OpenFile(string::ToUTF8(sFullPath).c_str());
      if (result != 0) LOG<<"OpenFile xdg::OpenFile returned "<<xdg.GetOpenErrorString(result)<<std::endl;
    }

    void ShowFile(const string_t& sFilePath)
    {
      xdg::cXdg xdg;
      xdg.OpenFolderHighlightFile(spitfire::string::ToUTF8(sFilePath));
    }

    void ShowFolder(const string_t& sFolderPath)
    {
      xdg::cXdg xdg;
      int result = xdg.OpenFolder(string::ToUTF8(sFolderPath).c_str());
      if (result != 0) LOG<<"OpenFolder xdg::OpenFile returned "<<xdg.GetOpenErrorString(result)<<std::endl;
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

    void OpenURL(const string_t& sURL)
    {
      ICInstance icInstance;
      OSType psiSignature = uint32_t('Psi ');
      OSStatus error = ICStart( &icInstance, psiSignature );
      if (error == noErr) {
        ConstStr255Param hint = 0x0;
        const char* data = spitfire::string::ToUTF8(sURL).get();
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

    bool IsBundled()
    {
      // http://developer.apple.com/carbon/tipsandtricks.html#AmIBundled

      FSRef processRef;
      ProcessSerialNumber psn = { 0, kCurrentProcess };
      GetProcessBundleLocation(&psn, &processRef);

      FSCatalogInfo processInfo;
      FSGetCatalogInfo(&processRef, kFSCatInfoNodeFlags, &processInfo, NULL, NULL, NULL);

      bool bIsBundled = ((processInfo.nodeFlags & kFSNodeIsDirectoryMask) != 0);

      return bIsBundled;
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
