#include <cstdio>
#include <cstdlib>
#include <cmath>

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

#ifdef __WIN__
#include <windows.h>
#endif

namespace breathe
{
	namespace operatingsystem
	{
		std::string GetOperatingSystemNameString()
		{
#if defined(__WIN__)
			return "Windows";
#elif defined(__LINUX__)
			return "Linux";
#elif defined(__APPLE__)
	#if defined(__MACOSX__)
			return "MacOSX";
	#else
			return "MacOS Unknown";
	#endif
#else
			return "Unknown";
#endif
		}

		std::string GetOperatingSystemVersionString()
		{
			std::ostringstream o;
			o<<GetOperatingSystemVersionMajor()<<"."<<GetOperatingSystemVersionMinor();
			return o.str();
		}

		std::string GetOperatingSystemFullString()
		{
			std::string s = "Unknown " + GetOperatingSystemVersionString();

#ifdef __WIN__
			int major = GetOperatingSystemVersionMajor();
			int minor = GetOperatingSystemVersionMinor();
			
			if (4 == major)
			{
				if (0 == minor) s = "95";
				else if (1 == minor) s = "98";
				else if (9 == minor) s = "ME";
			}
			else if (5 == major)
			{
				if (0 == minor) s = "2000";
				else if (1 == minor) s = "XP";
				else if (2 == minor) s = "Server 2003";
			}
			else if (6 == major)
			{
				if (0 == minor) s = "Vista";
			}

			// Check if we are running in Wine
			HKEY hKey;
			if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Wine"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
			{
				RegCloseKey(hKey);
				s += " under Wine";
			}
#endif

			return GetOperatingSystemNameString() + " " + s;
		}

		int GetOperatingSystemVersionMajor()
		{
#ifdef __WIN__
			OSVERSIONINFO vi;
			vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&vi);
			return vi.dwMajorVersion;
#else
#error "Unknown operating system"
			return 0;
#endif
		}

		int GetOperatingSystemVersionMinor()
		{
#ifdef __WIN__
			OSVERSIONINFO vi;
			vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&vi);
			return vi.dwMinorVersion;
#else
#error "Unknown operating system"
			return 0;
#endif
		}

#ifdef __WIN__
		std::string GetUserName()
		{
			breathe::char_t user[260];
			user[0] = 0;

			DWORD nSize=(DWORD)260;
			if (GetComputerName(user, &nSize)) return breathe::string::ToUTF8(breathe::string_t(user));

			if (getenv("USER") != 0) return std::string(getenv("USER"));
			else if (getenv("USERNAME") != 0) return std::string(getenv("USERNAME"));
			
			return std::string("<UNKNOWN>");
		}
#endif
	}
}
