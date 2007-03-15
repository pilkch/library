#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>
#include <BREATHE/UTIL/cMD5.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

BREATHE::cFileSystem *pFileSystem;

namespace BREATHE
{
	std::string cFileSystem::GetPath(std::string sFilename)
	{
		std::string p="";
		std::string s=sFilename;

		int i=s.find("/");;
		while(i!=std::string::npos)
		{
			i++;
			p+=s.substr(0, i);
			s=s.substr(i);
			i=s.find("/");
		};

		return p;
	}

	std::string cFileSystem::GetFile(std::string sFilename)
	{
		std::string s=sFilename;

		int i=s.find("/");;
		while(i!=std::string::npos)
		{
			i++;
			s=s.substr(i);
			i=s.find("/");
		};

		return s;
	}
	std::string cFileSystem::GetFileNoExtension(std::string sFilename)
	{
		std::string s=sFilename;

		int i=s.find("/");
		while(i!=std::string::npos)
		{
			i++;
			s=s.substr(i);
			i=s.find("/");
		};

		i=s.find(".");;
		if(i!=std::string::npos)
			return s.substr(0, i);

		return "";
	}
	std::string cFileSystem::GetExtension(std::string sFilename)
	{
		std::string p="";
		std::string s=sFilename;

		int i=s.find("/");;
		while(i!=std::string::npos)
		{
			i++;
			s=s.substr(i);
			i=s.find("/");
		};

		if(i!=std::string::npos)
			s=s.substr(i);

		i=s.find(".");;
		while(i!=std::string::npos)
		{
			i++;
			s=s.substr(i);
			i=s.find(".");
		};

		if(i!=std::string::npos)
			p=s.substr(i);
		else
      p=s.substr(0);

		return p;
	}

	
	void cFileSystem::AddDirectory(std::string sDirectory)
	{
		unsigned int i=0;
		unsigned int n=vDirectory.size();
		for(i=0;i<n;i++)
		{
			if(vDirectory[i]==sDirectory)
				return;
		}

		vDirectory.push_back(sDirectory);
    pLog->Success("FileSystem", "Added " + sDirectory);
	}

	std::string cFileSystem::FindFile(std::string sFilename)
	{
		if(""==sFilename)
			return "";

		//Check sFilename that was passed in
		std::ifstream f(sFilename.c_str());

		if(f.is_open())
		{
			f.close();
			return sFilename;
		}

		//Check for each directory+sFilename
		std::vector<std::string>::iterator iter=vDirectory.begin();
    while(iter!=vDirectory.end())
		{
			f.open(((*iter) + sFilename).c_str());
			
			if(f.is_open())
			{
				f.close();
				return (*iter) + sFilename;
			}

			iter++;
		};
		

		//Check for each directory+sFilename-path
		iter=vDirectory.begin();
		std::string sFile=GetFile(sFilename);
    while(iter!=vDirectory.end())
		{
			f.open(((*iter) + sFile).c_str());
			
			if(f.is_open())
			{
				f.close();
				return (*iter) + sFile;
			}

			iter++;
		};
    
		pLog->Error("FileSystem", "Not Found " + sFilename);
		return sFilename;
	}

	std::string cFileSystem::GetMD5(std::string sFilename)
	{
		cMD5 m;
		m.CheckFile(sFilename.c_str());

		return m.sResult;
	}
	
	namespace FILESYSTEM
	{
#ifdef PLATFORM_WINDOWS
#pragma push_macro("FileExists")
#undef FileExists
#endif
		bool FileExists(std::string sFilename)
		{
#ifdef PLATFORM_WINDOWS
#pragma pop_macro("FileExists")
			
			WIN32_FIND_DATA FindFileData;
			HANDLE hFind = FindFirstFile(sFilename.c_str(), &FindFileData);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				FindClose(hFind);
				return true;
			}

			return false;
#else
			pLog->Error("cFileSystem::CreateFile", "Not implemented on this platform");
			return false;
#endif
		}


#ifdef PLATFORM_WINDOWS
#pragma push_macro("CreateDirectory")
#undef CreateDirectory
#endif
		bool CreateDirectory(std::string sFoldername)
		{
#ifdef PLATFORM_WINDOWS
#pragma pop_macro("CreateDirectory")

			return !(ERROR_PATH_NOT_FOUND==CreateDirectory(sFoldername.c_str(), NULL));
#else
			pLog->Error("cFileSystem::CreateFolder", "Not implemented on this platform");
			return false;
#endif
		}

#ifdef PLATFORM_WINDOWS
#pragma push_macro("CreateFile")
#undef CreateFile
#endif
		bool CreateFile(std::string sFilename)
		{
#ifdef PLATFORM_WINDOWS
#pragma pop_macro("CreateFile")
			
			// Check if this file is already created so that we don't overwrite it
			if(FileExists(sFilename))
				return true;

			// File not found, we can now create the file
			HANDLE handle = CreateFile(sFilename.c_str(),     // file to create
                   GENERIC_WRITE,          // open for writing
                   0,                      // do not share
                   NULL,                   // default security
                   CREATE_ALWAYS,          // overwrite existing
                   FILE_ATTRIBUTE_NORMAL | // normal file
                   FILE_FLAG_OVERLAPPED,   // asynchronous I/O
                   NULL);                  // no attr. template
			if(INVALID_HANDLE_VALUE != handle)
			{
				// This file is created
				CloseHandle(handle);
				return true;
			}
			
			return false;
#else
			pLog->Error("cFileSystem::CreateFile", "Not implemented on this platform");
			return false;
#endif
		}
	}
}
