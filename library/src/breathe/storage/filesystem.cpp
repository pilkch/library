#include <sys/stat.h>

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

#include <breathe/storage/filesystem.h>

#ifndef FIRESTARTER
#include <breathe/util/log.h>
#include <breathe/algorithm/md5.h>
#endif


#ifdef __WIN__
#include <windows.h>
#endif

namespace breathe
{
  namespace filesystem
  {
    std::vector<string_t> vDirectory;
    string_t sApplicationDirectory = TEXT("");

    string_t ExpandPath(const string_t& path)
    {
      printf("ExpandPath path=\"%s\"\n", path.c_str());

      // ""
      if (path.empty() || (TEXT("./") == path)) {
        printf("ExpandPath 0 returning \"%s\"\n", GetThisApplicationDirectory().c_str());
        return GetThisApplicationDirectory();
      }

      if (TEXT(".") == path) {
        printf("ExpandPath 1 returning \"%s\"\n", breathe::string::StripAfterLastInclusive(GetThisApplicationDirectory(), TEXT("/")).c_str());
        return breathe::string::StripAfterLastInclusive(GetThisApplicationDirectory(), TEXT("/"));
      }

			// "."
			// ".********"
      if ((path == TEXT(".")) || ((path.length() > 2) && path[0] == TEXT('.')) && (path[1] != TEXT('.'))) {
        string_t expanded(path.substr(2));
        printf("ExpandPath 2 returning \"%s\"\n", expanded.c_str());
        return expanded;
      }

			string_t expanded = path;
			string_t prefix = GetThisApplicationDirectory();
			while (breathe::string::BeginsWith(expanded, TEXT("../"))) {
				expanded.erase(0, 3);
        printf("ExpandPath prefix=\"%s\"\n", prefix.c_str());
				prefix = StripLastDirectory(prefix);
			};

      printf("ExpandPath final prefix=\"%s\" expanded=\"%s\"\n", prefix.c_str(), expanded.c_str());

			return prefix + expanded;
		}

		string_t StripLastDirectory(const string_t& path)
		{
			// if "folder1/folder2/folder3" return "folder1/folder2/"
			// else ("folder1/folder2/" so ... ) return "folder1/"

			string_t result(path);
			if (breathe::string::EndsWith(path, TEXT("/"))) result = breathe::string::StripAfterLastInclusive(result, TEXT("/"));

			return breathe::string::StripAfterLast(result, TEXT("/"));
		}

    void SetThisExecutable(const string_t& executable)
    {
      CONSOLE<<"SetThisExecutable executable="<<executable<<std::endl;

#ifdef __WIN__
      sApplicationDirectory = GetPath(breathe::string::Replace(executable, TEXT("\\"), TEXT("/")));
#else
      const size_t len = 1024;
      char pwd[len];
      getcwd(pwd, len);
      CONSOLE<<"SetThisExecutable pwd="<<pwd<<std::endl;
      sApplicationDirectory = string_t(pwd);
#endif

      assert(sApplicationDirectory.length() > 0);

      if (!breathe::string::EndsWith(sApplicationDirectory, TEXT("/"))) sApplicationDirectory += TEXT("/");

      CONSOLE<<"SetThisExecutable returning "<<sApplicationDirectory<<std::endl;
    }

    string_t GetThisApplicationDirectory()
    {
      assert(sApplicationDirectory.length() > 0);
      return sApplicationDirectory;
    }

		string_t GetPath(const string_t& sFilename)
		{
			string_t p = TEXT("");
			string_t s = sFilename;

			string_t::size_type i = s.find(TEXT("/"));
			while(i != string_t::npos)
			{
				i++;
				p += s.substr(0, i);
				s = s.substr(i);
				i = s.find(TEXT("/"));
			};

			return p;
		}

		string_t GetFile(const string_t& sFilename)
		{
			string_t::size_type i = sFilename.rfind(TEXT("/"));

			// We didn't find a folder, so just return the whole path
			if (string_t::npos == i) return sFilename;

			i++;
			return sFilename.substr(i);
		}

		string_t GetFileNoExtension(const string_t& sFilename)
		{
			string_t::size_type i = sFilename.find(TEXT("/"));
			string_t temp = sFilename;
			while(i != string_t::npos)
			{
				i++;
				temp = temp.substr(i);
				i = temp.find(TEXT("/"));
			};

			i = temp.find(TEXT("."));
			if (i != string_t::npos) return temp.substr(0, i);

			return TEXT("");
		}

		string_t GetExtension(const string_t& sFilename)
		{
			string_t s=sFilename;

			string_t::size_type i=s.find(TEXT("/"));;
			while(i != string_t::npos)
			{
				i++;
				s=s.substr(i);
				i=s.find(TEXT("/"));
			};

			if (i != string_t::npos) s = s.substr(i);

			i = s.find(TEXT("."));
			while(i!=string_t::npos)
			{
				i++;
				s = s.substr(i);
				i = s.find(TEXT("."));
			};

			if (i != string_t::npos) return s.substr(i);

			return s.substr(0);
		}


		void AddDirectory(const string_t& sDirectory)
		{
			string_t expanded = ExpandPath(sDirectory);

			size_t i = 0;
			size_t n = vDirectory.size();
			for (i=0;i<n;i++)
			{
				if (vDirectory[i] == expanded)
					return;
			}

			vDirectory.push_back(expanded);
#ifndef FIRESTARTER
			LOG.Success("FileSystem", breathe::string::ToUTF8(TEXT("Added ") + expanded));
#endif
		}

		string_t FindFile(const string_t& sPath, const string_t& sFilename)
		{
			string_t temp = FindFile(sPath + sFilename);

			if (FileExists(temp)) return temp;

			return FindFile(sFilename);
		}

		string_t FindFile(const string_t& sFilename)
		{
#ifdef __LINUX__
      CONSOLE<<"FindFile "<<sFilename<<std::endl;
#endif
      if (TEXT("") == sFilename) return sFilename;

      // Check for each directory+sFilename
      std::vector<string_t>::iterator iter=vDirectory.begin();
      while(iter!=vDirectory.end()) {
        string_t filename = breathe::string::ToString_t((*iter) + sFilename);
        CONSOLE<<"Attempting to open "<<filename<<std::endl;
        if (FileExists(filename)) {
          CONSOLE<<"Found "<<filename<<std::endl;
          return filename;
        }

        iter++;
      };


      // Check for each directory+sFilename-path
      iter = vDirectory.begin();
      string_t sFile = GetFile(sFilename);
      while(iter != vDirectory.end()) {
        string_t filename = breathe::string::ToString_t(breathe::string::ToString_t((*iter) + sFilename));
        CONSOLE<<"Attempting to open "<<filename<<std::endl;
        if (FileExists(filename)) {
          CONSOLE<<"Found "<<filename<<std::endl;
          return filename;
        }

        iter++;
      };

      // Check sFilename that was passed in
      CONSOLE<<"Attempting to open "<<sFilename<<std::endl;
      if (FileExists(sFilename))
      {
        CONSOLE<<"Found "<<sFilename<<std::endl;
        return sFilename;
      }

      return sFilename;
    }

    uint64_t GetFileSize(const string_t& sFilename)
    {
      struct stat results;

      if (stat(sFilename.c_str(), &results) == 0) return results.st_size;

      return 0;
    }


#ifndef FIRESTARTER
		string_t GetMD5(const string_t& sFilename)
		{
			cMD5 m;
			m.CheckFile(breathe::string::ToUTF8(sFilename));

			return breathe::string::ToString_t(m.GetResult());
		}
#endif

#ifdef __WIN__
#pragma push_macro("FileExists")
#undef FileExists
#endif
    bool FileExists(const breathe::string_t& sFilename)
    {
#ifdef __WIN__
#pragma pop_macro("FileExists")

      WIN32_FIND_DATA FindFileData;
      HANDLE hFind = FindFirstFile(sFilename.c_str(), &FindFileData);
      if (hFind != INVALID_HANDLE_VALUE)
      {
        FindClose(hFind);
        return true;
      }

      return false;
#elif defined(__LINUX__)
      return (0 == access(sFilename.c_str(), F_OK));
#else
      #error "FileExists not implemented on this platform"
#endif
    }


#ifdef __WIN__
#pragma push_macro("CreateDirectory")
#undef CreateDirectory
#endif
    bool CreateDirectory(const breathe::string_t& sFoldername)
    {
#ifdef __WIN__
#pragma pop_macro("CreateDirectory")

#ifdef UNICODE
      return (ERROR_PATH_NOT_FOUND != ::CreateDirectoryW(sFoldername.c_str(), NULL));
#else
      return (ERROR_PATH_NOT_FOUND != ::CreateDirectoryA(sFoldername.c_str(), NULL));
#endif // !UNICODE

#elif defined(__LINUX__)
      return (0 == mkdir(sFoldername.c_str(), S_IRWXU | S_IRWXG | S_IRWXO));
#else
      #error "CreateDirectory not implemented on this platform"
      return false;
#endif
    }

#ifdef __WIN__
#pragma push_macro("CreateFile")
#undef CreateFile
#endif
		bool CreateFile(const breathe::string_t& sFilename)
		{
#ifdef __WIN__
#pragma pop_macro("CreateFile")

			// Check if this file is already created so that we don't overwrite it
			if (FileExists(sFilename))
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
			if (INVALID_HANDLE_VALUE != handle)
			{
				// This file is created
				CloseHandle(handle);
				return true;
			}

			return false;
#elif defined(__LINUX__)
      std::ofstream file(sFilename.c_str());
      bool bIsOpen = file.good();
      file.close();

      return bIsOpen;
#else
      #error "CreateFile not implemented on this platform"
			return false;
#endif
		}


		// ************************************************* path *************************************************

		path::path(const string_t& file_or_directory)
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

		string_t path::GetDirectory() const // Returns just the directory "/folder1/folder2/"
		{
			assert(IsDirectory());
			return filesystem::GetPath(sPath);
		}

		string_t path::GetFile() const // Returns just the file "file.txt"
		{
			assert(IsFile());
			return filesystem::GetFile(sPath);
		}

		string_t path::GetExtenstion() const // Returns just the extension ".txt"
		{
			assert(IsFile());
			return filesystem::GetExtension(sPath);
		}

		string_t path::str() const // Returns the full path "/folder1/folder2/file.txt"
		{
			return sPath;
		}


		// ********************************************* iterator *********************************************

		/*iterator::iterator()
		{
			//TODO: Set ourselves to the current directory
		}

		iterator::iterator(const string_t& directory)
		{

		}

		iterator::iterator(const iterator& rhs)
		{
		}

		iterator::~iterator()
		{
		}

		string_t iterator::GetName() const
		{
			return TEXT("");
		}

		bool iterator::HasChildren() const
		{
			return false;
		}

		iterator iterator::GetDirectoryIterator() const
		{
			return iterator();
		}

		file_iterator iterator::GetFileIterator() const
		{
			return file_iterator();
		}


		iterator  iterator::operator ++(int)
		{
      // TODO: iterate
			return *this;
		}

		iterator::operator bool() const
		{
			return false;
		}

		iterator iterator::operator =(const iterator& rhs)
		{
			return *this;
		}*/
	}
}
