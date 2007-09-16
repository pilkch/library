#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cOperatingSystem.h>
#include <BREATHE/UTIL/cMD5.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace BREATHE
{
	namespace OPERATINGSYSTEM
	{
		std::string GetOperatingSystemNameString()
		{
#ifdef PLATFORM_WINDOWS
			return "Windows";
#elif defined PLATFORM_LINUX
			return "Linux";
#elif defined PLATFORM_MAC
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

#ifdef PLATFORM_WIN
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
				if (0 = minor) s = "2000";
				else if (1 = minor) s = "XP";
				else if (2 = minor) s = "Server 2003";
			}
			else if (6 == major)
			{
				if (0 = minor) s = "Vista";
			}

			// Check if we are running in Wine
			HKEY hKey;
			if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Wine"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
			{
				RegCloseKey(hKey);
				s += " Under Wine";
			}
			else
			{
				unicode_char szPath[MAX_PATH];
				unicode_char* szPtr;
				return (SearchPath(NULL, TEXT("explorer.exe"), NULL, MAX_PATH, szPath, &szPtr) == 0);
			}
#endif

			return GetOperatingSystemNameString() + " " + s;
		}

		int GetOperatingSystemVersionMajor()
		{
#ifdef PLATFORM_WINDOWS
			OSVERSIONINFO vi;
			vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&vi);
			return vi.dwMajorVersion;
#else
			!!! Unknown operating system
			return 0;
#endif
		}

		int GetOperatingSystemVersionMinor()
		{
#ifdef PLATFORM_WINDOWS
			OSVERSIONINFO vi;
			vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&vi);
			return vi.dwMinorVersion;
#else
			!!! Unknown operating system
			return 0;
#endif
		}
	}
}
