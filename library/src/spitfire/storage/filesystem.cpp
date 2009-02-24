#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>

#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#ifdef __LINUX__
#include <dirent.h>
#include <pwd.h>
#include <errno.h>
#elif defined(__WIN__)
#include <windows.h>
#endif

#include <sys/stat.h>

// Spitfire Includes
#include <spitfire/spitfire.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/operatingsystem.h>

#include <spitfire/storage/filesystem.h>

#ifndef FIRESTARTER
#include <spitfire/util/log.h>
#include <spitfire/algorithm/md5.h>
#endif


namespace spitfire
{
  namespace filesystem
  {
    const size_t MAX_PATH_LEN = 1024;
    const string_t sFolderSeparator = TEXT("/");

    std::vector<string_t> vDirectory;
    string_t sApplicationDirectory = TEXT("");

    void SetThisExecutable(const string_t& executable)
    {
      CONSOLE<<"SetThisExecutable executable="<<executable<<std::endl;

#ifdef __WIN__
      sApplicationDirectory = GetPath(spitfire::string::Replace(executable, TEXT("\\"), TEXT("/")));
#else
      sApplicationDirectory = GetCurrentDirectory();
#endif

      ASSERT(!sApplicationDirectory.empty());

      if (!spitfire::string::EndsWith(sApplicationDirectory, TEXT("/"))) sApplicationDirectory += TEXT("/");

      CONSOLE<<"SetThisExecutable application directory="<<sApplicationDirectory<<", returning"<<std::endl;
    }

    string_t GetThisApplicationDirectory()
    {
      ASSERT(sApplicationDirectory.length() > 0);
      return sApplicationDirectory;
    }

// spitfire/src/linux/file.cpp
// /home/chris/.spitfire/spitfire/profile.xml
// /home/chris/.spitfire/spitfire/config.xml
// /home/chris/.spitfire/drive/

// spitfire/src/apple/file.cpp
// /Users/Chris/.spitfire/spitfire/profile.xml
// /Users/Chris/.spitfire/spitfire/config.xml
// /Users/Chris/.spitfire/drive/

// spitfire/src/windows/file.cpp
// c:/User Settings/Chris/App Data/spitfire/spitfire/profile.xml
// c:/User Settings/Chris/App Data/spitfire/spitfire/config.xml
// c:/User Settings/Chris/App Data/spitfire/drive/

#ifdef __APPLE__
    string_t GetApplicationSettingsDirectory(const string_t& sApplication)
    {
      ASSERT(sApplication != string_t(TEXT(SPITFIRE_APPLICATION_NAME_LWR)));
      return GetHomeDirectory() + TEXT(".spitfire/") + sApplication + TEXT("/");
    }
#endif

#ifdef __LINUX__
    string_t GetApplicationSettingsDirectory(const string_t& sApplication)
    {
      ASSERT(sApplication != string_t(TEXT(SPITFIRE_APPLICATION_NAME_LWR)));
      return GetHomeDirectory() + TEXT(".spitfire/") + sApplication + TEXT("/");
    }

    string_t GetTempDirectory()
    {
#ifdef P_tmpdir
      // On some systems this is defined for us
      return spitfire::string::ToString_t(P_tmpdir);
#endif

      string_t sPath;
      if (operatingsystem::GetEnvironmentVariable(TEXT("TMPDIR"), sPath)) return sPath;

      // Last resort
      return TEXT("/tmp");
    }

    bool DeleteFile(const string_t& sFilename)
    {
      return (unlink(spitfire::string::ToUTF8(sFilename).c_str())== 0);
    }

    bool DeleteDirectory(const string_t& sFoldername)
    {
      return (rmdir(spitfire::string::ToUTF8(sFoldername).c_str()) == 0);
    }
#endif

    // This is where we don't want to destroy the creation time etc. of the destination file,
    // also if the destination file is a link to another file it won't destroy the link,
    // it will actually write to the linked to file
    void CopyContentsOfFile(const string_t& sFrom, const string_t& sTo)
    {
      std::ifstream i(spitfire::string::ToUTF8(sFrom).c_str());
      std::ofstream o(spitfire::string::ToUTF8(sTo).c_str());

      char c = '\0';

      while (i.get(c)) o.put(c);
    }

    string_t GetHomeDirectory()
    {
      string_t sPath;
#ifdef WIN32
      char szPath[MAX_PATH_LEN];
      strcpy(szPath, exe_directory);
      ASSERT(SHGetFolderPath(0, CSIDL_APPDATA | CSIDL_FLAG_CREATE, 0, SHGFP_TYPE_CURRENT, szPath) == 0);
      sPath = string_t(szPath);
#endif
#ifdef BUILD_LINUX_UNIX
      char* szHome = getenv("HOME");
      if (szHome != nullptr) string_t(szHome);
      else {
        struct passwd *pw = getpwuid(getuid());
        ASSERT(pw != nullptr);
        sPath = string_t(pw->pw_dir);
      }
#endif
      ASSERT(!sPath.empty());
      return sPath;
    }

    string_t GetHomeImagesDirectory()
    {
      string_t sPath;
#ifdef WIN32
      char szPath[MAX_PATH_LEN];
      strcpy(szPath, exe_directory);
      ASSERT(SHGetFolderPath(0, CSIDL_MYPICTURES | CSIDL_FLAG_CREATE, 0, SHGFP_TYPE_CURRENT, szPath) == 0);
      sPath = string_t(szPath);
#endif
      ASSERT(!sPath.empty());
      return sPath;
    }

    string_t GetHomeMusicDirectory()
    {
      string_t sPath;
#ifdef WIN32
      char szPath[MAX_PATH_LEN];
      strcpy(szPath, exe_directory);
      ASSERT(SHGetFolderPath(0, CSIDL_MYMUSIC | CSIDL_FLAG_CREATE, 0, SHGFP_TYPE_CURRENT, szPath) == 0);
      sPath = string_t(szPath);
#endif
      ASSERT(!sPath.empty());
      return sPath;
    }

#ifdef __APPLE__
    string_t GetBundlePath()
    {
      CFBundleRef mainBundle = CFBundleGetMainBundle();
      ASSERT(mainBundle);

      cCFURL mainBundleURL(CFBundleCopyBundleURL(mainBundle));
      ASSERT(mainBundleURL.IsValid());

      cCFString cfsPath(CFURLCopyFileSystemPath(mainBundleURL.ref, kCFURLPOSIXPathStyle));
      ASSERT(cfsPath.IsValid());

      return cfsPath.GetString_t();
    }

    string_t GetResourcesPath()
    {
      return GetBundlePath() + string_t("/Contents/Resources/");
    }
#endif

    void ChangeDirectoryToExecutablePath(const char* argv0)
    {
      std::string exePath(argv0);
      std::string::size_type last_pos = exePath.find_last_of ("\\/");
      if (last_pos != std::string::npos) {
        std::string exeDir = exePath.substr (0, last_pos);
        std::cerr << "Chdir to " << exeDir << std::endl;
        chdir(exeDir.c_str());
      }
    }




    string_t GetCurrentDirectory()
    {
      char szDirectory[MAX_PATH_LEN];
      getcwd(szDirectory, MAX_PATH_LEN);
      return spitfire::string::ToString_t(szDirectory);
    }

    void ChangeToDirectory(const string_t& sDirectory)
    {
      chdir(spitfire::string::ToUTF8(sDirectory).c_str());
    }

    string_t ExpandPath(const string_t& path)
    {
      LOG<<"ExpandPath path=\""<<path<<"\""<<std::endl;

      // ""
      if (path.empty() || (TEXT("./") == path)) {
        LOG<<"ExpandPath 0 returning \""<<GetThisApplicationDirectory()<<"\""<<std::endl;
        return GetThisApplicationDirectory();
      }

      if (TEXT(".") == path) {
        LOG<<"ExpandPath 1 returning \""<<spitfire::string::StripAfterLastInclusive(GetThisApplicationDirectory(), TEXT("/"))<<"\""<<std::endl;
        return spitfire::string::StripAfterLastInclusive(GetThisApplicationDirectory(), TEXT("/"));
      }

      // "."
      // ".********"
      if ((path == TEXT(".")) || (((path.length() > 2) && (path[0] == TEXT('.'))) && (path[1] != TEXT('.')))) {
        string_t expanded(path.substr(2));
        LOG<<"ExpandPath 2 returning \""<<expanded<<"\""<<std::endl;
        return expanded;
      }

      string_t expanded = path;
      string_t prefix = GetThisApplicationDirectory();
      while (spitfire::string::BeginsWith(expanded, TEXT("../"))) {
        expanded.erase(0, 3);
        LOG<<"ExpandPath prefix=\""<<prefix<<"\""<<std::endl;
        prefix = StripLastDirectory(prefix);
      };

      LOG<<"ExpandPath final prefix=\""<<expanded<<"\" expanded=\""<<expanded<<"\""<<std::endl;

      return prefix + expanded;
    }

    string_t StripLastDirectory(const string_t& path)
    {
      // if "folder1/folder2/folder3" return "folder1/folder2/"
      // else ("folder1/folder2/" so ... ) return "folder1/"

      string_t result(path);
      if (spitfire::string::EndsWith(path, TEXT("/"))) result = spitfire::string::StripAfterLastInclusive(result, TEXT("/"));

      return spitfire::string::StripAfterLast(result, TEXT("/"));
    }

    string_t GetPath(const string_t& sFilename)
    {
      string_t p = TEXT("");
      string_t s = sFilename;

      string_t::size_type i = s.find(TEXT("/"));
      while (i != string_t::npos) {
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
      while (i != string_t::npos) {
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
      while(i != string_t::npos) {
        i++;
        s=s.substr(i);
        i=s.find(TEXT("/"));
      };

      if (i != string_t::npos) s = s.substr(i);

      i = s.find(TEXT("."));
      while (i != string_t::npos) {
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
      const size_t n = vDirectory.size();
      for (i = 0; i < n; i++) {
        if (vDirectory[i] == expanded) return;
      }

      ASSERT(spitfire::string::EndsWith(expanded, TEXT("/")));
      vDirectory.push_back(expanded);
#ifndef FIRESTARTER
      LOG.Success("FileSystem", spitfire::string::ToUTF8(TEXT("Added ") + expanded));
#endif
    }

#ifdef __WIN__
#pragma push_macro("FileExists")
#undef FileExists
#endif
    bool FileExists(const string_t& sFilename)
    {
      LOG<<"FileExists \""<<sFilename<<"\""<<std::endl;
#ifdef __WIN__
#pragma pop_macro("FileExists")
      WIN32_FIND_DATA FindFileData;
      HANDLE hFind = FindFirstFile(sFilename.c_str(), &FindFileData);
      if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        return true;
      }

      return false;
#elif defined(__LINUX__)
      //int iValue = access(spitfire::string::ToUTF8(sFilename).c_str(), F_OK);
      //if (iValue == 0) {
      //  LOG<<"FileExists returning true"<<std::endl;
      //  return true;
      //}

      //if (errno == ENOENT) {
      //  LOG<<"FileExists ENOENT returning false"<<std::endl;
      //  return false;
      //}

      //if (errno == EACCES) LOG<<"FileExists EACCES"<<std::endl;

      //LOG<<"FileExists returning true"<<std::endl;
      //return true;

      return (access(spitfire::string::ToUTF8(sFilename).c_str(), F_OK) == 0);
#else
#error "FileExists not implemented on this platform"
#endif
    }

    bool FindFile(const string_t& sPath, const string_t& sFilename, string_t& sOutFilename)
    {
      sOutFilename.clear();

      string_t temp;
      FindFile(sPath + sFilename, temp);

      if (FileExists(temp)) {
        sOutFilename = temp;
        return true;
      }

      return FindFile(sFilename, sOutFilename);
    }

    bool FindFile(const string_t& sFilename, string_t& sOutFilename)
    {
      CONSOLE<<"FindFile \""<<sFilename<<"\""<<std::endl;

      sOutFilename.clear();

      if (sFilename.empty()) {
        CONSOLE<<"FindFile No file specified "<<sFilename<<" returning false"<<std::endl;
        return false;
      }

      // Check for each directory+sFilename
      std::vector<string_t>::iterator iter = vDirectory.begin();
      const std::vector<string_t>::iterator iterEnd = vDirectory.end();
      while (iter != iterEnd) {
        string_t filename = spitfire::string::ToString_t((*iter) + sFilename);
        CONSOLE<<"FindFile Attempting to open "<<filename<<std::endl;
        if (FileExists(filename)) {
          CONSOLE<<"FindFile Found "<<filename<<" returning true"<<std::endl;
          sOutFilename = filename;
          return true;
        }

        iter++;
      };


      // We used to check each directory + the filename without its directories, but that is quite useless.
      // We now require that each program is more specific in its request and actually knows what a file will be called.
      // Check for each directory+sFilename-path
      // iter = vDirectory.begin();
      // string_t sFile = GetFile(sFilename);
      // while(iter != vDirectory.end()) {
      //   string_t filename = spitfire::string::ToString_t(spitfire::string::ToString_t((*iter) + sFilename));
      //   CONSOLE<<"Attempting to open "<<filename<<std::endl;
      //   if (FileExists(filename)) {
      //     CONSOLE<<"Found "<<filename<<std::endl;
      //     sOutFilename = filename;
      //     return true;
      //   }
      //
      //   iter++;
      // };

      // Check sFilename that was passed in
      CONSOLE<<"FindFile Attempting to open "<<sFilename<<std::endl;
      if (FileExists(sFilename)) {
        CONSOLE<<"FindFileFound "<<sFilename<<" returning true"<<std::endl;
        sOutFilename = sFilename;
        return true;
      }

      CONSOLE<<"FindFile "<<sFilename<<" returning false"<<std::endl;
      return false;
    }

    bool FindResourceFile(const string_t& sPath, const string_t& sFilename, string_t& sOutFilename)
    {
      const string_t sNewPath(TEXT("data/") + sPath);
      return FindFile(sNewPath, sFilename, sOutFilename);
    }

    bool FindResourceFile(const string_t& sFilename, string_t& sOutFilename)
    {
      const string_t sNewFilename(TEXT("data/") + sFilename);
      return FindFile(sNewFilename, sOutFilename);
    }


    uint64_t GetFileSize(const string_t& sFilename)
    {
      struct stat results;

      if (stat(spitfire::string::ToUTF8(sFilename).c_str(), &results) == 0) return results.st_size;

      return 0;
    }


#ifndef FIRESTARTER
    string_t GetMD5(const string_t& sFilename)
    {
      cMD5 m;
      m.CheckFile(spitfire::string::ToUTF8(sFilename));
      return spitfire::string::ToString_t(m.GetResult());
    }
#endif

#ifdef __WIN__
#pragma push_macro("CreateDirectory")
#undef CreateDirectory
#endif
    bool CreateDirectory(const string_t& sFoldername)
    {
#ifdef __WIN__
#pragma pop_macro("CreateDirectory")

#ifdef UNICODE
      return (ERROR_PATH_NOT_FOUND != ::CreateDirectoryW(sFoldername.c_str(), NULL));
#else
      return (ERROR_PATH_NOT_FOUND != ::CreateDirectoryA(sFoldername.c_str(), NULL));
#endif // !UNICODE

#elif defined(__LINUX__)
      // Create the directory
      if (0 != mkdir(spitfire::string::ToUTF8(sFoldername).c_str(), S_IRWXU | S_IRWXG | S_IRWXO)) return false;

      // Set owner and group
      struct passwd *pw = getpwuid(getuid());
      ASSERT(pw != nullptr);
      return (0 == chown(spitfire::string::ToUTF8(sFoldername).c_str(), pw->pw_uid, pw->pw_gid));
#else
      #error "CreateDirectory not implemented on this platform"
      return false;
#endif
    }

#ifdef __WIN__
#pragma push_macro("CreateFile")
#undef CreateFile
#endif
    bool CreateFile(const string_t& sFilename)
    {
#ifdef __WIN__
#pragma pop_macro("CreateFile")

      // Check if this file is already created so that we don't overwrite it
      if (FileExists(sFilename)) return true;

      // File not found, we can now create the file
#ifdef UNICODE
      HANDLE handle = ::CreateFileW(
#else
      HANDLE handle = ::CreateFileA(
#endif
        sFilename.c_str(),      // file to create
        GENERIC_WRITE,          // open for writing
        0,                      // do not share
        NULL,                   // default security
        CREATE_ALWAYS,          // overwrite existing
        FILE_ATTRIBUTE_NORMAL | // normal file
        FILE_FLAG_OVERLAPPED,   // asynchronous I/O
        NULL                    // no attr. template
      );
      if (INVALID_HANDLE_VALUE != handle) {
        // This file is created
        CloseHandle(handle);
        return true;
      }

      return false;
#elif defined(__LINUX__)
      std::ofstream file(spitfire::string::ToUTF8(sFilename).c_str());
      bool bIsOpen = file.good();
      file.close();

      return bIsOpen;
#else
      #error "CreateFile not implemented on this platform"
      return false;
#endif
    }

    string_t MakeFilePath(const string_t& sDirectory, const string_t& sFile)
    {
      string_t sFullPath(sDirectory);
      if (!spitfire::string::EndsWith(sDirectory, sFolderSeparator)) sFullPath += sFolderSeparator;
      return sFullPath + spitfire::string::StripLeading(sFile, sFolderSeparator);
    }

    string_t MakeFilePath(const string_t& sDirectory, const string_t& sSubDirectory, const string_t& sFile)
    {
      string_t sFullPath(sSubDirectory);
      if (!spitfire::string::EndsWith(sSubDirectory, sFolderSeparator)) sFullPath += sFolderSeparator;
      return MakeFilePath(sDirectory, sFullPath + spitfire::string::StripLeading(sFile, sFolderSeparator));
    }

    // ************************************************* path *************************************************

    path::path(const string_t& sDirectory) :
      sPath(sDirectory)
    {
    }

    path::path(const string_t& sDirectory, const string_t& sFile) :
      sPath(MakeFilePath(sDirectory, sFile))
    {
    }

    path::path(const string_t& sDirectory, const string_t& sSubDirectory, const string_t& sFile) :
      sPath(MakeFilePath(sDirectory, sSubDirectory, sFile))
    {
    }

    bool path::IsFile() const
    {
#ifdef __WIN__
      WIN32_FIND_DATA FindFileData;
      HANDLE hFind = FindFirstFile(sPath.c_str(), &FindFileData);
      if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        return true;
      }

      return false;
#elif defined(__LINUX__)
      struct stat _stat;
      int result = lstat(spitfire::string::ToUTF8(sPath).c_str(), &_stat);
      if (0 > result) {
        LOG<<"path::IsFile lstat FAILED returned "<<result<<" for file "<<sPath<<std::endl;
        return false;
      }
      return S_ISREG(_stat.st_mode);
#else
#error "path::IsFile not implemented on this platform"
#endif
    }

    bool path::IsDirectory() const
    {
#ifdef __WIN__
      WIN32_FIND_DATA FindFileData;
      HANDLE hFind = FindFirstFile(sPath.c_str(), &FindFileData);
      if (hFind != INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        return true;
      }

      return false;
#elif defined(__LINUX__)
      struct stat _stat;
      int result = lstat(spitfire::string::ToUTF8(sPath).c_str(), &_stat);
      if (0 > result) {
        LOG<<"path::IsDirectory lstat FAILED returned "<<result<<" for file "<<sPath<<std::endl;
        return false;
      }
      return S_ISDIR(_stat.st_mode);
#else
#error "path::IsDirectory not implemented on this platform"
#endif
    }

    string_t path::GetDirectory() const // Returns just the directory "/folder1/folder2/"
    {
      ASSERT(IsDirectory());
      return filesystem::GetPath(sPath);
    }

    string_t path::GetFile() const // Returns just the file "file.txt"
    {
      ASSERT(IsFile());
      return filesystem::GetFile(sPath);
    }

    string_t path::GetExtenstion() const // Returns just the extension ".txt"
    {
      ASSERT(IsFile());
      return filesystem::GetExtension(sPath);
    }

    string_t path::GetFullPath() const // Returns the full path "/folder1/folder2/file.txt"
    {
      return sPath;
    }


    // ********************************************* cScopedDirectoryChange *********************************************

    cScopedDirectoryChange::cScopedDirectoryChange(const string_t& sNewDirectory)
    {
      sPreviousDirectory = GetCurrentDirectory();
      ChangeToDirectory(sNewDirectory);
    }

    cScopedDirectoryChange::~cScopedDirectoryChange()
    {
      ChangeToDirectory(sPreviousDirectory);
    }


    // ********************************************* iterator *********************************************

    iterator::iterator() :
      bIsEndIterator(true),
      i(0),
      sParentFolder(TEXT(""))
    {
    }

    iterator::iterator(const string_t& directory) :
      bIsEndIterator(false),
      i(0),
      sParentFolder(directory)
    {
#ifdef __WIN__
#error "iterator::iterator not implemented in windows"
#elif defined(__LINUX__)
      DIR* d = opendir(spitfire::string::ToUTF8(sParentFolder).c_str());
      struct dirent* dirp;
      if (d != nullptr) {
        while ((dirp = readdir(d)) != NULL ) {
          if ((0 != strcmp(".", dirp->d_name)) &&
              (0 != strcmp("..", dirp->d_name)))
            paths.push_back(spitfire::string::ToString_t(dirp->d_name));
        }
      }
      closedir(d);
#else
#error "iterator::iterator not implemented on this platform"
#endif
    }

    iterator::iterator(const iterator& rhs)
    {
      bIsEndIterator = true;
      i = rhs.i;
      sParentFolder = rhs.sParentFolder;
      paths = rhs.paths;
    }


    iterator& iterator::operator++(int)
    {
      const size_t n = paths.size();
      if (i < n) {
        i++;
        if (i == n) bIsEndIterator = true;
      }

      return *this;
    }

    iterator& iterator::operator=(const iterator& rhs)
    {
      bIsEndIterator = rhs.bIsEndIterator;
      i = rhs.i;
      sParentFolder = rhs.sParentFolder;
      paths = rhs.paths;

      return *this;
    }

    bool iterator::operator==(const iterator& rhs)
    {
      // If we were never initialised or have iterator through our path lists
      return ((!IsValid() && !rhs.IsValid()) || ((sParentFolder == rhs.sParentFolder) && (paths.size() == rhs.paths.size())));
    }

    bool iterator::operator!=(const iterator& rhs)
    {
      return !(*this == rhs);
    }

    string_t iterator::GetFile() const
    {
      ASSERT(IsValid());
      return paths[i];
    }

    string_t iterator::GetFullPath() const
    {
      ASSERT(IsValid());
      return MakeFilePath(sParentFolder, paths[i]);
    }

    bool iterator::HasChildren() const
    {
      return !paths.empty();
    }

    bool iterator::IsValid() const
    {
      return (!bIsEndIterator && !paths.empty() && (i < paths.size()));
    }

    bool iterator::IsFile() const
    {
      ASSERT(IsValid());
      path p(sParentFolder, paths[i]);
      return p.IsFile();
    }

    bool iterator::IsDirectory() const
    {
      ASSERT(IsValid());
      path p(sParentFolder, paths[i]);
      return p.IsDirectory();
    }



















    enum PRIORITY
    {
      PRIORITY_LOW = 0,
      PRIORITY_DEFAULT,
      PRIORITY_HIGH
    };


   // ** cEntry
   // cEntry is extensible by inheriting and overriding _GetFile();
   // TODO: We don't actually have any classes that build upon cEntry
   // For example: 7z, zip, rar, http, ftp, last.fm stream (MP3 served via http but requires login),
   // 7z, zip, rar would be extracted to a temporary folder in the temp directory and that folder would then be mounted

    class cEntry
    {
    public:
      cEntry(const string_t& sFullPath, PRIORITY priority);
      virtual ~cEntry() {}

      const string_t& GetFullPath() const { return sFullPath; }
      PRIORITY GetPriority() const { return priority; }
      void SetPriority(PRIORITY _priority) { priority = _priority; }

    // Returns true and sets sFullPath to the fullpath ie. "/home/chris/.spitfire/shared/data/texture/testing.png" if found
      bool GetFile(const string_t& sFile, string_t& sFullPath) const { return _GetFile(sFile, sFullPath); }

    private:
      virtual bool _GetFile(const string_t& sFile, string_t& sFullPath) const;

      string_t sFullPath;
      PRIORITY priority;
    };

    cEntry::cEntry(const string_t& _sFullPath, PRIORITY _priority) :
      sFullPath(_sFullPath),
      priority(_priority)
    {
    }

    bool cEntry::_GetFile(const string_t& sFile, string_t& sOutFullPath) const
    {
      LOG<<"cEntry::_GetFile"<<std::endl;

      // If the file doesn't exists return false;
      if (!filesystem::FileExists(sFullPath + sFile)) {
        LOG<<"cEntry::_GetFile File \""<<(sFullPath + sFile).c_str()<<"\" not found, returning false"<<std::endl;
        return false;
      }

      // File exists, fill out the filename and return true
      LOG<<"cEntry::_GetFile File \""<<(sFullPath + sFile).c_str()<<"\" found, returning true"<<std::endl;
      sOutFullPath = sFullPath + sFile;
      return true;
    }


    class cVirtualFileSystem
    {
    public:
      ~cVirtualFileSystem();

      void Create();
      void Destroy();

      void MountWithPriority(const string_t& sPath, PRIORITY priority);

      void Unmount(const string_t& sPath);

      bool GetFile(const string_t& sFile, string_t& sFullPath) const;

    private:
      void Sort();
      static bool EntryCompare(const cEntry* lhs, const cEntry* rhs);

      // Paths sorted by priority, front/begin is the highest priority, back/end is the lowest
      std::vector<cEntry*> paths;
    };

    cVirtualFileSystem::~cVirtualFileSystem()
    {
      ASSERT(paths.empty());
    }

    void cVirtualFileSystem::Create()
    {
      ASSERT(paths.empty());
    }

    void cVirtualFileSystem::Destroy()
    {
      // Delete and remove each entry from the map
      std::vector<cEntry*>::iterator iter = paths.begin();
      const std::vector<cEntry*>::iterator iterEnd = paths.end();

      while (iter != iterEnd) {
        cEntry* pEntry = (*iter);
        ASSERT(pEntry != nullptr);

        SAFE_DELETE(pEntry);

        iter++;
      }

      paths.clear();
    }

    // *** Comparison for sorting particles based on depth

    inline bool cVirtualFileSystem::EntryCompare(const cEntry* lhs, const cEntry* rhs)
    {
      return (lhs->GetPriority() > rhs->GetPriority());
    }

    void cVirtualFileSystem::Sort()
    {
      std::sort(paths.begin(), paths.end(), cVirtualFileSystem::EntryCompare);
    }

    void cVirtualFileSystem::MountWithPriority(const string_t& sPath, PRIORITY priority)
    {
      // Find the current entry if there is one
      std::vector<cEntry*>::iterator iter = paths.begin();
      const std::vector<cEntry*>::iterator iterEnd = paths.end();

      while (iter != iterEnd) {
        cEntry* pEntry = (*iter);
        ASSERT(pEntry != nullptr);

        if (pEntry->GetFullPath() == sPath) break;

        iter++;
      };

      // If we didn't find an entry, create a new entry and return
      if (iter == iterEnd) {
        // Create a new entry
        cEntry* pEntry = new cEntry(sPath, priority);
        paths.push_back(pEntry);
      } else {
        // We already have this element, set the priority and return
        cEntry* pEntry = (*iter);
        ASSERT(pEntry != nullptr);

        pEntry->SetPriority(priority);
      }

      // Now that we have a valid entry we need to sort the list
      Sort();
    }

    void cVirtualFileSystem::Unmount(const string_t& sPath)
    {
      // Find the current entry if there is one
      std::vector<cEntry*>::iterator iter = paths.begin();
      const std::vector<cEntry*>::iterator iterEnd = paths.end();

      while (iter != iterEnd) {
        cEntry* pEntry = (*iter);
        ASSERT(pEntry != nullptr);

        if (pEntry->GetFullPath() == sPath) break;

        iter++;
      };

      // If we didn't find an entry, return
      if (iter == iterEnd) return;

      // Delete the data that this element points to
      cEntry* pEntry = (*iter);
      ASSERT(pEntry != nullptr);
      SAFE_DELETE(pEntry);

      // Now remove the actual map element
      paths.erase(iter);
    }

    bool cVirtualFileSystem::GetFile(const string_t& sFile, string_t& sFullPath) const
    {
      LOG<<"cVirtualFileSystem::GetFile"<<std::endl;

      // Check each entry in order from highest to lowest priority
      std::vector<cEntry*>::const_iterator iter = paths.begin();
      const std::vector<cEntry*>::const_iterator iterEnd = paths.end();

      while (iter != iterEnd) {
        cEntry* pEntry = (*iter);
        ASSERT(pEntry != nullptr);

        if (pEntry->GetFile(sFile, sFullPath)) return true;

        iter++;
      }

      return false;
    }

    cVirtualFileSystem vfs;


    // These are global functions that call the virtual file system versions

    void Create()
    {
      vfs.Create();
    }

    void Destroy()
    {
      vfs.Destroy();
    }

    void Mount(const string_t& sPath)
    {
      vfs.MountWithPriority(sPath, PRIORITY_DEFAULT);
    }

    void MountHighPriority(const string_t& sPath)
    {
      vfs.MountWithPriority(sPath, PRIORITY_HIGH);
    }

    void MountLowPriority(const string_t& sPath)
    {
      vfs.MountWithPriority(sPath, PRIORITY_LOW);
    }

    void Unmount(const string_t& sPath)
    {
      vfs.Unmount(sPath);
    }

    bool GetFile(const string_t& sFile, string_t& sFullPath)
    {
      return vfs.GetFile(sFile, sFullPath);
    }
  }
}
