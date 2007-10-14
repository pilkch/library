#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include <breathe/breathe.h>
#include <breathe/util/cString.h>

#include <breathe/util/log.h>
#include <breathe/util/filesystem.h>
#include <breathe/util/md5.h>

#include <unrarlib/unrarlib.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace breathe
{
	namespace filesystem
	{
		void DetectByteOrderMark(const uint8_t* bytes)
		{
			/*
			UTF-8 EF BB BF
			UTF-16 (big-endian) FE FF
			UTF-16 (little-endian) FF FE
			UTF-16BE, UTF-32BE (big-endian) No BOM!
			UTF-16LE, UTF-32LE (little-endian) No BOM!
			UTF-32 (big-endian) 00 00 FE FF
			UTF-32 (little-endian) FF FE 00 00
			SCSU (compression) 0E FE FF
			*/
		}


		std::vector<std::string> vDirectory;
		std::string sExecutable = "";

		void SetThisExecutable(std::string executable)
		{
			sExecutable = GetPath(breathe::string::Replace(executable, "\\", "/"));
			assert(sExecutable.length() > 0);
		}

		std::string GetThisApplicationDirectory()
		{
			assert(sExecutable.length() > 0);
			return sExecutable;
		}

		std::string GetPath(const std::string& sFilename)
		{
			std::string p = "";
			std::string s = sFilename;

			std::string::size_type i=s.find("/");;
			while(i!=std::string::npos)
			{
				i++;
				p+=s.substr(0, i);
				s=s.substr(i);
				i=s.find("/");
			};

			return p;
		}

		std::string GetFile(const std::string& sFilename)
		{
			std::string::size_type i = sFilename.rfind("/");

			// We didn't find a folder, so just return the whole path
			if(std::string::npos == i) return sFilename;

			i++;
			return sFilename.substr(i);
		}
		std::string GetFileNoExtension(const std::string& sFilename)
		{
			std::string::size_type i = sFilename.find("/");
			std::string temp = sFilename;
			while(i != std::string::npos)
			{
				i++;
				temp = temp.substr(i);
				i = temp.find("/");
			};

			i = temp.find(".");;
			if(i != std::string::npos)
				return temp.substr(0, i);

			return "";
		}
		std::string GetExtension(const std::string& sFilename)
		{
			std::string p="";
			std::string s=sFilename;

			std::string::size_type i=s.find("/");;
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

		
		void AddDirectory(std::string sDirectory)
		{
			unsigned int i=0;
			unsigned int n=vDirectory.size();
			for(i=0;i<n;i++)
			{
				if(vDirectory[i]==sDirectory)
					return;
			}

			vDirectory.push_back(sDirectory);
			LOG.Success("FileSystem", "Added " + sDirectory);
		}

		std::string FindFile(std::string sPath, std::string sFilename)
		{
			std::string temp = FindFile(sPath + sFilename);
			
			if (filesystem::FileExists(breathe::string::ToString_t(temp))) return temp;

			//!breathe::filesystem::FileExists(breathe::string::ToString_t(pLayer->sTexture)))

			return FindFile(sFilename);
		}

		std::string FindFile(std::string sFilename)
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
	    
			LOG.Error("FileSystem", "Not Found " + sFilename);
			return sFilename;
		}

		std::string GetMD5(std::string sFilename)
		{
			cMD5 m;
			m.CheckFile(sFilename.c_str());

			return m.GetResult();
		}
		
#ifdef PLATFORM_WINDOWS
#pragma push_macro("FileExists")
#undef FileExists
#endif
		bool FileExists(breathe::string::string_t sFilename)
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
			LOG.Error("FileExists", "Not implemented on this platform");
			return false;
#endif
		}


#ifdef PLATFORM_WINDOWS
#pragma push_macro("CreateDirectory")
#undef CreateDirectory
#endif
		bool CreateDirectory(breathe::string::string_t sFoldername)
		{
#ifdef PLATFORM_WINDOWS
#pragma pop_macro("CreateDirectory")
			
#ifdef UNICODE
			return (ERROR_PATH_NOT_FOUND != ::CreateDirectoryW(sFoldername.c_str(), NULL));
#else
			return (ERROR_PATH_NOT_FOUND != ::CreateDirectoryA(sFoldername.c_str(), NULL));
#endif // !UNICODE

#else
			LOG.Error("CreateDirectory", "Not implemented on this platform");
			return false;
#endif
		}

#ifdef PLATFORM_WINDOWS
#pragma push_macro("CreateFile")
#undef CreateFile
#endif
		bool CreateFile(breathe::string::string_t sFilename)
		{
#ifdef PLATFORM_WINDOWS
#pragma pop_macro("CreateFile")
			
			// Check if this file is already created so that we don't overwrite it
			if(FileExists(sFilename))
				return true;

			// File not found, we can now create the file
#ifdef UNICODE
			HANDLE handle = ::CreateFileW(
#else
			HANDLE handle = ::CreateFileA(
#endif
				sFilename.c_str(),     // file to create
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
			LOG.Error("CreateFile", "Not implemented on this platform");
			return false;
#endif
		}


		// ************************************************* path *************************************************

		path::path(std::string file_or_directory)
		{
			sPath = file_or_directory;
		}

		bool path::IsFile() const
		{
			return GetFile().length() > 0;
		}
		
		bool path::IsDirectory() const
		{
			return GetFile().length() == 0;
		}

		std::string path::GetDirectory() const // Returns just the directory "/folder1/folder2/"
		{
			assert(IsDirectory());
			return filesystem::GetPath(sPath);
		}

		std::string path::GetFile() const // Returns just the file "file.txt"
		{
			assert(IsFile());
			return filesystem::GetFile(sPath);
		}

		std::string path::GetExtenstion() const // Returns just the extension ".txt"
		{
			assert(IsFile());
			return filesystem::GetExtension(sPath);
		}

		std::string path::str() const // Returns the full path "/folder1/folder2/file.txt"
		{
			return sPath;
		}


		// ********************************************* directory_iterator *********************************************
		
		directory_iterator::directory_iterator()
		{
			//TODO: Set ourselves to the current directory
		}

		directory_iterator::directory_iterator(const directory_iterator& rhs)
		{
		}

		directory_iterator::~directory_iterator()
		{
		}

		std::string directory_iterator::GetName() const
		{
			return "";
		}

		bool directory_iterator::HasChildren() const
		{
			return false;
		}

		directory_iterator directory_iterator::GetDirectoryIterator() const
		{
			return directory_iterator();
		}

		file_iterator directory_iterator::GetFileIterator() const
		{
			return file_iterator();
		}


		directory_iterator::operator ++(int)
		{
			return *this;
		}

		directory_iterator::operator bool() const
		{
			return false;
		}

		directory_iterator::operator =(const directory_iterator& rhs)
		{
			return *this;
		}


		// *********************************************** file_iterator ***********************************************

		file_iterator::file_iterator()
		{
		}
	}
}
