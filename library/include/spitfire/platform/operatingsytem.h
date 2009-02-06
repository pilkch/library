#ifndef PLATFORM_OPERATINGSYSTEM_H
#define PLATFORM_OPERATINGSYSTEM_H

namespace spitfire
{
#ifdef __WIN__
  void GetEnvironmentVariable(const string_t& sVariable, string_t& sValue)
  {
    tchar szValue[LSTR];
      ::GetEnvironmentVariable(spitfire::string::ToCString(sVariable), szValue, LSTR);
      sValue = szValue;
  }

  void SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue)
  {
      ::SetEnvironmentVariable(spitfire::string::ToCString(sVariable), spitfire::string::ToCString(sValue));
  }

  void RemoveEnvironmentVariable(const string_t& sVariable)
  {
      ::SetEnvironmentVariable(spitfire::string::ToCString(sVariable), nullptr);
  }
#elif defined(PLATFORM_LINUX_OR_UNIX)
  void GetEnvironmentVariable(const string_t& sVariable, string_t& sValue)
  {
    sValue.clear();

    char* szValue = getenv(spitfire::string::ToCString(sVariable));
    if (szValue != nullptr) sValue = szValue;
  }

  void SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue)
  {
    putenv(spitfire::string::ToCString(sVariable), spitfire::string::ToCString(sValue));
  }

  void RemoveEnvironmentVariable(const string_t& sVariable)
  {
    unsetenv(spitfire::string::ToCString(sVariable));
  }
#endif
}

#endif // PLATFORM_OPERATINGSYSTEM_H
