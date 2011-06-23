// Standard headers
#include <string.h>

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

// Boost headers
#include <boost/filesystem.hpp>

#ifdef __LINUX__
#include <dirent.h>
#include <pwd.h>
#include <errno.h>

// TODO: Remove these
#include <sys/stat.h>
#elif defined(__WIN__)
#include <windows.h>
#endif

// xdg headers
#include <libxdgmm/libxdgmm.h>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/cString.h>

#include <spitfire/platform/operatingsystem.h>
#include <spitfire/platform/pipe.h>

#include <spitfire/storage/filesystem.h>

#ifndef FIRESTARTER
#include <spitfire/util/log.h>
#endif

#ifdef BUILD_SUPPORT_MD5
#include <spitfire/algorithm/md5.h>
#endif

#ifdef BUILD_SUPPORT_SHA1
#include <spitfire/algorithm/sha1.h>
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
      sApplicationDirectory = GetPath(executable);
#endif

      ASSERT(!sApplicationDirectory.empty());

      // Replace ./ with the current directory
      if (spitfire::string::BeginsWith(sApplicationDirectory, TEXT("./"))) {
        const string_t sCurrentDirectory = GetCurrentDirectory();
        const string_t sEnd = spitfire::string::StripLeading(sApplicationDirectory, TEXT("./"));

        // Join the two parts together
        sApplicationDirectory = sCurrentDirectory + sEnd;
      }

      if (!spitfire::string::EndsWith(sApplicationDirectory, TEXT("/"))) sApplicationDirectory += TEXT("/");

      CONSOLE<<"SetThisExecutable application directory="<<sApplicationDirectory<<", returning"<<std::endl;
    }

    string_t GetThisApplicationDirectory()
    {
      ASSERT(!sApplicationDirectory.empty());
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
// C:/User Settings/Chris/App Data/.spitfire/spitfire/profile.xml
// C:/User Settings/Chris/App Data/.spitfire/spitfire/config.xml
// C:/User Settings/Chris/App Data/.spitfire/drive/

// C:/Documents and Settings/Chris/App Data/.spitfire/spitfire/profile.xml
// C:/Documents and Settings/Chris/App Data/.spitfire/spitfire/config.xml
// C:/Documents and Settings/Chris/App Data/.spitfire/drive/

// C:/Users/Chris/.spitfire/spitfire/profile.xml
// C:/Users/Chris/.spitfire/spitfire/config.xml
// C:/Users/Chris/.spitfire/drive/



#if defined(__LINUX__) || defined(__APPLE__)
    string_t GetApplicationSettingsDirectory(const string_t& sApplication)
    {
      return GetHomeConfigurationFilesDirectory() + TEXT("/") + sApplication + TEXT("/");
    }
#endif

#ifdef __LINUX__
    string_t GetTempDirectory()
    {
#ifdef P_tmpdir
      ASSERT(P_tmpdir != nullptr);
      ASSERT(P_tmpdir[0] != 0);
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
      CONSOLE<<"DeleteFile \""<<spitfire::string::ToUTF8(sFilename)<<"\""<<std::endl;
      const boost::filesystem::path file(spitfire::string::ToUTF8(sFilename));
      boost::filesystem::remove(file);
      return !FileExists(sFilename);
    }

    bool DeleteDirectory(const string_t& sFoldername)
    {
      CONSOLE<<"DeleteDirectory \""<<spitfire::string::ToUTF8(sFoldername)<<"\""<<std::endl;
      const boost::filesystem::path file(spitfire::string::ToUTF8(sFoldername));
      return (boost::filesystem::remove_all(file) != 0);
    }
#endif

    // This is where we don't want to destroy the creation time etc. of the destination file, also if the destination
    // file is a link to another file it won't destroy the link, it will actually write to the linked to file.
    // NOTE: This is very very slow, only reading one character at a time, this could take minutes even on a few GB file?
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
#elif defined(__APPLE__)
      FSRef dataFolderRef;
      OSErr theError = FSFindFolder(kUserDomain, kCurrentUserFolderType, kCreateFolder, &dataFolderRef);
      if (theError != noErr) {
        LOG<<"GetHomeDirectory FSFindFolder FAILED"<<std::endl;
        string_t sUser = GetUserName();
        sPath = TEXT("/Users/") + sUser;
      } else {
        char szPath[MAX_PATH_LEN];
        szPath[0] = 0;
        FSRefMakePath(&dataFolderRef, (UInt8*)szPath, 200);
        sPath = spitfire::string::ToString_t(szPath);
      }
#elif defined(PLATFORM_LINUX_OR_UNIX)
      sPath = TEXT("/opt");

      // Try getpwuid
      struct passwd* pw = nullptr;
      setpwent();
      pw = getpwuid(getuid());
      endpwent();

      if (pw != nullptr) {
        // We have a valid password entity
        sPath = string::ToString_t(pw->pw_dir);
      } else {
        // Try XDG
        std::string sDirectory;
        xdg::GetHomeDirectory(sDirectory);
        sPath = string::ToString_t(sDirectory);
      }
#endif
      ASSERT(!sPath.empty());
      return sPath;
    }

    string_t GetHomeConfigurationFilesDirectory()
    {
      std::string sDirectory;
      xdg::GetConfigHomeDirectory(sDirectory);
      return string::ToString_t(sDirectory);
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

      return cfsPath.GetString();
    }

    string_t GetResourcesPath()
    {
      return GetBundlePath() + string_t("/Contents/Resources/");
    }
#endif

    void ChangeDirectoryToExecutablePath(const char* argv0)
    {
      string_t sExecutablePath = spitfire::string::ToString_t(argv0);
      string_t::size_type last_pos = sExecutablePath.find_last_of(TEXT("\\/"));
      if (last_pos != string_t::npos) {
        const string_t sExecutableFolder = sExecutablePath.substr(0, last_pos);
        ChangeToDirectory(sExecutableFolder);
      }
    }




    string_t GetCurrentDirectory()
    {
      char szDirectory[MAX_PATH_LEN];
      const char* szResult = getcwd(szDirectory, MAX_PATH_LEN);
      if (szResult == nullptr) LOG<<"GetCurrentDirectory getcwd FAILED errno="<<errno<<std::endl;
      return spitfire::string::ToString_t(szDirectory);
    }

    void ChangeToDirectory(const string_t& sDirectory)
    {
      ASSERT(DirectoryExists(sDirectory));
      int iResult = chdir(spitfire::string::ToUTF8(sDirectory).c_str());
      if (iResult != 0) LOG<<"ChangeToDirectory chdir FAILED iResult="<<iResult<<" errno="<<errno<<std::endl;
    }

    bool IsFolderWritable(const string_t& sFolder)
    {
      bool bIsFolderWritable = false;

      cPathBuilder builder(sFolder);
      builder.PushBackFolder(TEXT(".breathe_folder_writable_test"));
      const string_t sSubFolder = builder.GetFullPath();
      if (CreateDirectory(sSubFolder)) {
          DeleteDirectory(sSubFolder);
          bIsFolderWritable = true;
      }

      return bIsFolderWritable;
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
      string_t s = sFilename;

      string_t::size_type i = s.find(TEXT("/"));;
      while(i != string_t::npos) {
        i++;
        s = s.substr(i);
        i = s.find(TEXT("/"));
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
#ifdef __WIN__
#pragma pop_macro("FileExists")
#endif
      const boost::filesystem::path file(spitfire::string::ToUTF8(sFilename));
      return boost::filesystem::exists(file);
    }

#ifdef __WIN__
#pragma push_macro("DirectoryExists")
#undef DirectoryExists
#endif
    bool DirectoryExists(const string_t& sFolderName)
    {
#ifdef __WIN__
#pragma pop_macro("DirectoryExists")
#endif
      const boost::filesystem::path folder(spitfire::string::ToUTF8(sFolderName));
      return boost::filesystem::exists(folder);
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

    bool FindFile(const string_t& sInFilename, string_t& sOutFilename)
    {
      CONSOLE<<"FindFile \""<<sInFilename<<"\""<<std::endl;
      char sz[1024];
      strcpy(sz, spitfire::string::ToUTF8(sInFilename).data());
      CONSOLE<<"FindFile sz=\""<<sz<<"\""<<std::endl;

      sOutFilename.clear();

      if (sInFilename.empty()) {
        CONSOLE<<"FindFile No file specified "<<sInFilename<<" returning false"<<std::endl;
        return false;
      }

      // Check if the filename that was passed in is an absolute path
      CONSOLE<<"FindFile Attempting to open "<<sInFilename<<std::endl;
      if (sInFilename[0] == TEXT('/')) {
        if (FileExists(sInFilename)) {
          CONSOLE<<"FindFileFound "<<sInFilename<<" returning true"<<std::endl;
          sOutFilename = sInFilename;
          return true;
        }

        CONSOLE<<"FindFile "<<sInFilename<<" returning false"<<std::endl;
        return false;
      }

      string_t sFilename(sInFilename);
      while (!sFilename.empty()) {
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

        // Ok, that didn't work, let's try without the first directory
        const size_t nBefore = sFilename.length();
        sFilename = spitfire::string::StripBeforeInclusive(sFilename, TEXT("/"));
        if (nBefore == sFilename.length()) break;
      }

      CONSOLE<<"FindFile "<<sInFilename<<" returning false"<<std::endl;
      return false;
    }

    bool FindResourceFile(const string_t& sPath, const string_t& sFilename, string_t& sOutFilename)
    {
      if (FindFile(sPath, sFilename, sOutFilename)) return true;

      const string_t sNewPath(TEXT("data/") + sPath);
      return FindFile(sNewPath, sFilename, sOutFilename);
    }

    bool FindResourceFile(const string_t& sFilename, string_t& sOutFilename)
    {
      if (FindFile(sFilename, sOutFilename)) return true;

      const string_t sNewFilename(TEXT("data/") + sFilename);
      return FindFile(sNewFilename, sOutFilename);
    }


    uint64_t GetFileSize(const string_t& sFilename)
    {
      ASSERT(FileExists(sFilename));
      const boost::filesystem::path file(spitfire::string::ToUTF8(sFilename));
      return boost::filesystem::file_size(file);
    }


#ifdef BUILD_SUPPORT_MD5
    string_t GetMD5(const string_t& sFilename)
    {
      cMD5 m;
      m.CheckFile(spitfire::string::ToUTF8(sFilename));
      return spitfire::string::ToString_t(m.GetResult());
    }
#endif

#ifdef BUILD_SUPPORT_SHA1
    string_t GetSHA1(const string_t& sFilename)
    {
      cSHA1 s;
      s.CheckFile(spitfire::string::ToUTF8(sFilename));
      return spitfire::string::ToString_t(s.GetResult());
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
#endif

      return boost::filesystem::create_directories(sFoldername);
    }

#ifdef __WIN__
#pragma push_macro("CreateFile")
#undef CreateFile
#endif
    bool CreateFile(const string_t& sFilename)
    {
#ifdef __WIN__
#pragma pop_macro("CreateFile")
#endif

      // Check if this file is already created so that we don't overwrite it
      if (FileExists(sFilename)) return true;

      // File not found, we can now create the file
      std::ofstream file(spitfire::string::ToUTF8(sFilename).c_str());
      bool bIsOpen = file.good();
      file.close();

      return bIsOpen;
    }

    bool UnzipFileToFolder(const string_t& sFullPathToZipFile, const string_t& sFullPathToExtractFilesTo)
    {
      // I know this is a hack, but it is the easiest and one of the most portable around
      // The only platform this won't work on is stupid Windows
      // NOTE: We should be using 7-zip, Boost iostreams or zlib but those are all much more complex compared to this
      ostringstream_t o;
      o<<"unzip ";
      o<<"\""<<sFullPathToZipFile<<"\"";
      o<<" -d ";
      o<<"\""<<sFullPathToExtractFilesTo<<"\"";
      return (system(spitfire::string::ToUTF8(o.str()).c_str()) == 0);
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


    // ** cPathBuilder

    cPathBuilder::cPathBuilder(const string_t& sFolderPath)
    {
      string::cStringParser sp(sFolderPath);

      while (!sp.IsEnd()) {
        string_t sResult;
        if (sp.GetToStringAndSkip(sFolderSeparator, sResult)) folders.push_back(sResult);
      };

      folders.push_back(sp.GetToEnd());
    }

    string_t cPathBuilder::GetFullPathWithFolderSeparator() const
    {
      string_t sFullPath;

      #ifndef __WIN__
      sFullPath += sFolderSeparator;
      #endif

      std::list<string_t>::const_iterator iter = folders.begin();
      const std::list<string_t>::const_iterator iterEnd = folders.end();
      while (iter != iterEnd) {
          sFullPath += *iter + sFolderSeparator;

          iter++;
      }

      return sFullPath;
    }


    // ************************************************* cFilePathParser *************************************************

    cFilePathParser::cFilePathParser(const string_t& sFilePath)
    {
      ASSERT(false);
    }


    // ************************************************* cPath *************************************************

    cPath::cPath(const string_t& sDirectory) :
      sPath(sDirectory)
    {
    }

    cPath::cPath(const string_t& sDirectory, const string_t& sFile) :
      sPath(MakeFilePath(sDirectory, sFile))
    {
    }

    cPath::cPath(const string_t& sDirectory, const string_t& sSubDirectory, const string_t& sFile) :
      sPath(MakeFilePath(sDirectory, sSubDirectory, sFile))
    {
    }

    bool cPath::IsFile() const
    {
      const boost::filesystem::path file(spitfire::string::ToUTF8(sPath));
      return (boost::filesystem::exists(file) && boost::filesystem::is_regular(file));
    }

    bool cPath::IsDirectory() const
    {
      const boost::filesystem::path file(spitfire::string::ToUTF8(sPath));
      return (boost::filesystem::exists(file) && boost::filesystem::is_directory(file));
    }

    string_t cPath::GetDirectory() const // Returns just the directory "/folder1/folder2/"
    {
      ASSERT(IsDirectory());
      return filesystem::GetPath(sPath);
    }

    string_t cPath::GetFile() const // Returns just the file "file.txt"
    {
      ASSERT(IsFile());
      return filesystem::GetFile(sPath);
    }

    string_t cPath::GetExtenstion() const // Returns just the extension ".txt"
    {
      ASSERT(IsFile());
      return filesystem::GetExtension(sPath);
    }

    string_t cPath::GetFullPath() const // Returns the full path "/folder1/folder2/file.txt"
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

    #ifdef __LINUX__
    // Linux does not define strlcpy
    void strlcpy(char* szDestination, const char* szSource, size_t len)
    {
      // NOTE: This doesn't actually check length
      strcpy(szDestination, szSource);
    }
    #endif

    // ********************************************* cScopedTemporaryFolder *********************************************
    cScopedTemporaryFolder::cScopedTemporaryFolder()
    {
      LOG<<"cScopedTemporaryFolder::cScopedTemporaryFolder"<<std::endl;

    #ifdef __WIN__
      char_t szTempPath[MAX_PATH_LEN];
      ::GetTempPath(MAX_PATH_LEN, szTempPath);

      // GetTempFileName
      // http://msdn.microsoft.com/en-us/library/aa364991.aspx
      // Creates a name for a temporary file
      // If a unique file name is generated, an empty file is created and the handle to it is released; otherwise, only a file name is generated
      char_t szTempFolderPath[MAX_PATH_LEN];
      ::GetTempFileName(szTempPath, TEXT("tmp"), 0, szTempFolderPath);

      sTemporarySubFolder = szTempFolderPath;
    #else
      cPath p(TEXT(P_tmpdir), TEXT("0XXXXXX"));

      char szTempFolderPath[MAX_PATH_LEN];
      strlcpy(szTempFolderPath, spitfire::string::ToUTF8(p.GetFullPath()).c_str(), MAX_PATH_LEN);
      const char* szResult = mkdtemp(szTempFolderPath);
      ASSERT(szResult != nullptr);

      sTemporarySubFolder = spitfire::string::ToString_t(szTempFolderPath);
    #endif

      if (!DirectoryExists(sTemporarySubFolder)) {
          LOG<<"cScopedTemporaryFolder::cScopedTemporaryFolder Creating sub folder \""<<sTemporarySubFolder<<"\""<<std::endl;
          CreateDirectory(sTemporarySubFolder);
      }
    }

    cScopedTemporaryFolder::~cScopedTemporaryFolder()
    {
      LOG<<"cScopedTemporaryFolder::~cScopedTemporaryFolder"<<std::endl;
      ASSERT(DirectoryExists(sTemporarySubFolder));

      DeleteDirectory(sTemporarySubFolder);
    }



    // ********************************************* cFolderIterator *********************************************

    cFolderIterator::cFolderIterator() :
      bIsEndIterator(true),
      i(0),
      sParentFolder(TEXT(""))
    {
    }

    cFolderIterator::cFolderIterator(const string_t& directory) :
      bIsEndIterator(false),
      i(0),
      sParentFolder(directory)
    {
#ifdef __WIN__
#error "cFolderIterator::cFolderIterator not implemented in windows"
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
#error "cFolderIterator::cFolderIterator not implemented on this platform"
#endif
    }

    cFolderIterator::cFolderIterator(const cFolderIterator& rhs)
    {
      bIsEndIterator = true;
      i = rhs.i;
      sParentFolder = rhs.sParentFolder;
      paths = rhs.paths;
    }


    void cFolderIterator::Next()
    {
      const size_t n = paths.size();
      if (i < n) {
        i++;
        if (i == n) bIsEndIterator = true;
      }
    }

    cFolderIterator& cFolderIterator::operator=(const cFolderIterator& rhs)
    {
      bIsEndIterator = rhs.bIsEndIterator;
      i = rhs.i;
      sParentFolder = rhs.sParentFolder;
      paths = rhs.paths;

      return *this;
    }

    bool cFolderIterator::operator==(const cFolderIterator& rhs)
    {
      // If we were never initialised or have cFolderIterator through our path lists
      return ((!IsValid() && !rhs.IsValid()) || ((sParentFolder == rhs.sParentFolder) && (paths.size() == rhs.paths.size())));
    }

    bool cFolderIterator::operator!=(const cFolderIterator& rhs)
    {
      return !(*this == rhs);
    }

    string_t cFolderIterator::GetFile() const
    {
      ASSERT(IsValid());
      return paths[i];
    }

    string_t cFolderIterator::GetFullPath() const
    {
      ASSERT(IsValid());
      return MakeFilePath(sParentFolder, paths[i]);
    }

    bool cFolderIterator::HasChildren() const
    {
      return !paths.empty();
    }

    bool cFolderIterator::IsValid() const
    {
      return (!bIsEndIterator && !paths.empty() && (i < paths.size()));
    }

    bool cFolderIterator::IsFile() const
    {
      ASSERT(IsValid());
      cPath p(sParentFolder, paths[i]);
      return p.IsFile();
    }

    bool cFolderIterator::IsDirectory() const
    {
      ASSERT(IsValid());
      cPath p(sParentFolder, paths[i]);
      return p.IsDirectory();
    }



















    enum PRIORITY {
      LOW = 0,
      DEFAULT,
      HIGH
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

    // *** Comparison for sorting entries based on priority

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
      vfs.MountWithPriority(sPath, PRIORITY::DEFAULT);
    }

    void MountHighPriority(const string_t& sPath)
    {
      vfs.MountWithPriority(sPath, PRIORITY::HIGH);
    }

    void MountLowPriority(const string_t& sPath)
    {
      vfs.MountWithPriority(sPath, PRIORITY::LOW);
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

#if 0
void UnitTest()
{
   #ifdef __WIN__
   const string_t sDrive = TEXT("C:");
   #else
   const string_t sDrive = TEXT("");
   #endif

   const string_t sInitial = sDrive + TEXT(FOLDER_SEPARATOR) TEXT("test") TEXT(FOLDER_SEPARATOR) TEXT("a");
   cPathBuilder builder(sInitial);

   if (builder.GetFullPath() != sInitial + TEXT(FOLDER_SEPARATOR)) {
      ...
   }

   builder.PushBackFolder(TEXT("b"));

   if (builder.GetFullPath() != sInitial + TEXT(FOLDER_SEPARATOR) + TEXT("b") + TEXT(FOLDER_SEPARATOR)) {
      ...
   }

   builder.PopFolder();

   if (builder.GetFullPath() != sInitial + TEXT(FOLDER_SEPARATOR)) {
      ...
   }
}
#endif
