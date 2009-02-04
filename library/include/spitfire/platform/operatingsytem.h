#ifndef PLATFORM_OPERATINGSYSTEM_H
#define PLATFORM_OPERATINGSYSTEM_H

namespace breathe
{
#ifdef __WIN__
  void GetEnvironmentVariable(const string_t& sVariable, string_t& sValue)
  {
    tchar szValue[LSTR];
      ::GetEnvironmentVariable(breathe::string::ToCString(sVariable), szValue, LSTR);
      sValue = szValue;
  }

  void SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue)
  {
      ::SetEnvironmentVariable(breathe::string::ToCString(sVariable), breathe::string::ToCString(sValue));
  }

  void RemoveEnvironmentVariable(const string_t& sVariable)
  {
      ::SetEnvironmentVariable(breathe::string::ToCString(sVariable), nullptr);
  }
#elif defined(PLATFORM_LINUX_OR_UNIX)
  void GetEnvironmentVariable(const string_t& sVariable, string_t& sValue)
  {
    sValue.clear();

    char* szValue = getenv(breathe::string::ToCString(sVariable));
    if (szValue != nullptr) sValue = szValue;
  }

  void SetEnvironmentVariable(const string_t& sVariable, const string_t& sValue)
  {
    putenv(breathe::string::ToCString(sVariable), breathe::string::ToCString(sValue));
  }

  void RemoveEnvironmentVariable(const string_t& sVariable)
  {
    unsetenv(breathe::string::ToCString(sVariable));
  }
#endif
}

#endif // PLATFORM_OPERATINGSYSTEM_H
