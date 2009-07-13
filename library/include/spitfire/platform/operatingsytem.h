#ifndef PLATFORM_OPERATINGSYSTEM_H
#define PLATFORM_OPERATINGSYSTEM_H

namespace spitfire
{
#ifdef __WIN__
  // *** Environment variables

  inline void GetEnvironmentVariable(const string_t& sVariable, string_t& sValue)
  {
    tchar szValue[1024];
    GetEnvironmentVariable(spitfire::string::ToCString(sVariable), szValue, 1024);
    sValue = szValue;
  }

  inline void SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue)
  {
    SetEnvironmentVariable(spitfire::string::ToCString(sVariable), spitfire::string::ToCString(sValue));
  }

  inline void RemoveEnvironmentVariable(const string_t& sVariable)
  {
    SetEnvironmentVariable(spitfire::string::ToCString(sVariable), nullptr);
  }
#elif defined(PLATFORM_LINUX_OR_UNIX)
  // *** Environment variables

  inline void GetEnvironmentVariable(const string_t& sVariable, string_t& sValue)
  {
    sValue.clear();

    char* szValue = getenv(spitfire::string::ToCString(sVariable));
    if (szValue != nullptr) sValue = szValue;
  }

  inline void SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue)
  {
    bool bOverwrite = true;
    int iOverwrite = bOverwrite ? 1 : 0;
    int result = setenv(spitfire::string::ToCString(sVariable), spitfire::string::ToCString(sValue), iOverwrite);
    if (result != 0) LOG<<"SetEnvironmentVariable setenv FAILED "<<result<<std::endl;
    else {
      // Also export this variable
      //putenv(spitfire::string::ToCString(sVariable));
      putenv(spitfire::string::ToCString(sVariable), spitfire::string::ToCString(sValue));
    }
  }

  inline void RemoveEnvironmentVariable(const string_t& sVariable)
  {
    int result = unsetenv(ToCString(sVariable));
    if (result != 0) LOG<<"RemoveEnvironmentVariable unsetenv FAILED "<<result<<std::endl;
  }


   // *** Linux specific version functions

   inline bool IsRunningInGnome()
   {
      return (getenv("GNOME_DESKTOP_SESSION_ID") != nullptr);
   }

   inline bool IsRunningInKDE()
   {
      return (getenv("KDE_FULL_SESSION") != nullptr);
   }


   // *** Open functions

   inline void OpenFile(const string_t& sFullPath)
   {
      string_t sCommand = (IsRunningInKDE()) ? TEXT("kfmclient exec ") : TEXT("gnome-open ");
      sCommand += sFullPath;
      system(ToUTF8(sCommand).c_str());
   }

   inline void OpenTextFile(const string_t& sFullPath)
   {
      OpenFile(sFullPath);
   }

   inline void OpenFolder(const string_t& sFullPath)
   {
      OpenFile(sFullPath);
   }

   inline void OpenWebPage(const string_t& sURL)
   {
      OpenFile(sURL);
   }
#endif
}

#endif // PLATFORM_OPERATINGSYSTEM_H
