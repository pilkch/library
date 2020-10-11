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

// C++17 headers
#include <filesystem>

#ifdef __LINUX__
#include <dirent.h>
#include <pwd.h>
#include <errno.h>

// TODO: Remove these
#include <sys/stat.h>
#else
#include <windows.h>
#include <shlobj.h>
#include <direct.h>
#endif

#ifdef __LINUX__
// libtrashmm headers
#include <libtrashmm/libtrashmm.h>

// libxdgmm headers
#include <libxdgmm/libxdgmm.h>
#else
// libwin32mm headers
#include <libwin32mm/filesystem.h>
#endif

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/string.h>
#include <spitfire/util/thread.h>
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
    #ifdef __WIN__
    const string_t sFolderSeparator = TEXT("\\");
    #else
    const string_t sFolderSeparator = TEXT("/");
    #endif

    std::vector<string_t> vDirectory;
    string_t sApplicationDirectory = TEXT("");

    void SetThisExecutable(const string_t& executable)
    {
      CONSOLE<<"SetThisExecutable executable="<<executable<<std::endl;

      #if defined(__WIN__) && defined(BUILD_DEBUG)
      // Remove the debug part of the path to leave the application directory
      sApplicationDirectory = GetFolder(spitfire::string::Replace(executable, TEXT("project\\x64\\Debug\\"), TEXT("")));
      #else
      sApplicationDirectory = GetFolder(executable);
      #endif

      ASSERT(!sApplicationDirectory.empty());

      // Replace ./ with the current directory
      if (spitfire::string::StartsWith(sApplicationDirectory, TEXT(".") + sFolderSeparator)) {
        const string_t sCurrentDirectory = GetCurrentDirectory();
        const string_t sEnd = spitfire::string::StripLeading(sApplicationDirectory, TEXT(".") + sFolderSeparator);

        // Join the two parts together
        sApplicationDirectory = sCurrentDirectory + sEnd;
      }

      if (!spitfire::string::EndsWith(sApplicationDirectory, sFolderSeparator)) sApplicationDirectory += sFolderSeparator;

      CONSOLE<<"SetThisExecutable application directory="<<sApplicationDirectory<<", returning"<<std::endl;
    }

    string_t GetThisApplicationDirectory()
    {
      ASSERT(!sApplicationDirectory.empty());
      return sApplicationDirectory;
    }

// spitfire/src/linux/file.cpp
// /home/chris/.config/spitfire/profile.xml
// /home/chris/.config/spitfire/config.xml
// /home/chris/.config/drive/

// spitfire/src/apple/file.cpp
// /Users/Chris/.spitfire/spitfire/profile.xml
// /Users/Chris/.spitfire/spitfire/config.xml
// /Users/Chris/.spitfire/drive/

// spitfire/src/windows/file.cpp
// C:/User Settings/Chris/App Data/.spitfire/spitfire/profile.xml
// C:/User Settings/Chris/App Data/.spitfire/spitfire/config.xml
// C:/User Settings/Chris/App Data/.spitfire/drive/

// C:/Users/Chris/.spitfire/spitfire/profile.xml
// C:/Users/Chris/.spitfire/spitfire/config.xml
// C:/Users/Chris/.spitfire/drive/



    string_t GetApplicationSettingsDirectory(const string_t& sApplication)
    {
      return GetHomeConfigurationFilesDirectory() + sFolderSeparator + sApplication + sFolderSeparator;
    }

    bool DeleteFile(const string_t& sFilename)
    {
      CONSOLE<<"DeleteFile \""<<sFilename<<"\""<<std::endl;
      const std::filesystem::path file(spitfire::string::ToUTF8(sFilename));
      std::filesystem::remove(file);
      return !FileExists(sFilename);
    }

    bool DeleteDirectory(const string_t& sFoldername)
    {
      CONSOLE<<"DeleteDirectory \""<<sFoldername<<"\""<<std::endl;
      const std::filesystem::path file(spitfire::string::ToUTF8(sFoldername));
      return (std::filesystem::remove_all(file) != 0);
    }

    void MoveFileToTrash(const string_t& sFilePath)
    {
      #ifdef __WIN__
      win32mm::MoveFileOrFolderToRecycleBin(sFilePath);
      #else
      trash::MoveFileToTrash(string::ToUTF8(sFilePath));
      #endif
    }

    void MoveFolderToTrash(const string_t& sFolderPath)
    {
      #ifdef __WIN__
      win32mm::MoveFileOrFolderToRecycleBin(sFolderPath);
      #else
      trash::MoveFolderToTrash(string::ToUTF8(sFolderPath));
      #endif
    }

    void CopyFile(const string_t& sFrom, const string_t& sTo)
    {
      if (FileExists(sTo)) return;

      const std::filesystem::path from(spitfire::string::ToUTF8(sFrom));
      const std::filesystem::path to(spitfire::string::ToUTF8(sTo));
      std::filesystem::copy_file(from, to, std::filesystem::copy_options::none);
    }

    void CopyFileOverwrite(const string_t& sFrom, const string_t& sTo)
    {
      const std::filesystem::path from(spitfire::string::ToUTF8(sFrom));
      const std::filesystem::path to(spitfire::string::ToUTF8(sTo));
      std::filesystem::copy_file(from, to, std::filesystem::copy_options::overwrite_existing);
    }

    // This is where we don't want to destroy the creation time etc. of the destination file, also if the destination
    // file is a link to another file it won't destroy the link, it will actually write to the linked to file.
    // NOTE: This is very very slow, only reading one character at a time, this could take minutes even on a few GB file?
    void CopyContentsOfFile(const string_t& sFrom, const string_t& sTo)
    {
      std::ifstream i(spitfire::string::ToUTF8(sFrom).c_str());
      std::ofstream o(spitfire::string::ToUTF8(sTo).c_str());

      char c = '\0';

      while (i.get(c)) o.put(c);

      // Set the modification date so that it matches the source file
      SetLastModifiedDate(sTo, GetLastModifiedDate(sFrom));
    }

    bool MoveFile(const string_t& sFrom, const string_t& sTo)
    {
      //CONSOLE<<"MoveFile From \""<<sFrom<<"\", to \""<<sTo<<"\""<<std::endl;
      const std::filesystem::path from(spitfire::string::ToUTF8(sFrom));
      const std::filesystem::path to(spitfire::string::ToUTF8(sTo));
      std::filesystem::rename(from, to);
      return FileExists(sTo);
    }


    #ifdef __WIN__    
    string_t GetWin32UserFolder(uint16_t type)
    {
      char_t szPath[MAX_PATH_LEN];
      szPath[0] = 0;
      ::SHGetFolderPath(NULL, type | CSIDL_FLAG_CREATE, 0, SHGFP_TYPE_CURRENT, szPath);
      return string_t(szPath);
    }
    #endif

    string_t GetHomeDirectory()
    {
      string_t sPath;
#ifdef __WIN__
      sPath = GetWin32UserFolder(CSIDL_PROFILE);
#elif defined(__APPLE__)
      FSRef dataFolderRef;
      OSErr theError = FSFindFolder(kUserDomain, kCurrentUserFolderType, kCreateFolder, &dataFolderRef);
      if (theError != noErr) {
        LOG("FSFindFolder FAILED"<<std::endl;
        string_t sUser = GetUserName();
        sPath = TEXT("/Users/") + sUser;
      } else {
        char szPath[MAX_PATH_LEN];
        szPath[0] = 0;
        FSRefMakePath(&dataFolderRef, (UInt8*)szPath, 200);
        sPath = spitfire::string::ToString(szPath);
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
        sPath = string::ToString(pw->pw_dir);
      } else {
        // Try XDG
        xdg::cXdg xdg;
        sPath = string::ToString(xdg.GetHomeDirectory());
      }
#endif
      ASSERT(!sPath.empty());
      return sPath;
    }

    string_t GetHomeConfigurationFilesDirectory()
    {
      #ifdef __WIN__
      // C:\Documents and Settings\username\Local Settings\Application Data
      char_t szAppData[MAX_PATH_LEN];
      szAppData[0] = 0;
      SHGetFolderPath(0, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, 0, SHGFP_TYPE_CURRENT, szAppData);
      ASSERT(szAppData[0] != 0);
      LOG(TEXT("returning \""), szAppData, TEXT("\""));
      const string_t sFullPath = MakeFilePath(szAppData, TEXT(SPITFIRE_APPLICATION_COMPANY_NAME));
      return sFullPath;
      #else
      xdg::cXdg xdg;
      return string::ToString(xdg.GetHomeConfigDirectory());
      #endif
    }

    string_t GetHomePicturesDirectory()
    {
      string_t sPath;
#ifdef WIN32
      char_t szPath[MAX_PATH_LEN];
      szPath[0] = 0;
      SHGetFolderPath(0, CSIDL_MYPICTURES | CSIDL_FLAG_CREATE, 0, SHGFP_TYPE_CURRENT, szPath);
      sPath = string_t(szPath);
#else
      xdg::cXdg xdg;
      return string::ToString(xdg.GetHomePicturesDirectory());
#endif
      ASSERT(!sPath.empty());
      return sPath;
    }

    string_t GetHomeMusicDirectory()
    {
      string_t sPath;
#ifdef WIN32
      char_t szPath[MAX_PATH_LEN];
      szPath[0] = 0;
      SHGetFolderPath(0, CSIDL_MYMUSIC | CSIDL_FLAG_CREATE, 0, SHGFP_TYPE_CURRENT, szPath);
      sPath = string_t(szPath);
#else
      xdg::cXdg xdg;
      return string::ToString(xdg.GetHomeMusicDirectory());
#endif
      ASSERT(!sPath.empty());
      return sPath;
    }

    string_t GetHomeTempDirectory()
    {
      #ifdef __WIN__
      //  Gets the temp path env string (no guarantee it's a valid path).
      char_t szPath[MAX_PATH];
      szPath[0] = 0;
      const int iResult = GetTempPath(MAX_PATH, szPath);
      if (iResult > MAX_PATH || (iResult == 0)) {
        LOG("GetTempPath FAILED iResult=", spitfire::string::ToString(iResult));
        szPath[0] = 0;
      }
      return szPath;
      #else
      xdg::cXdg xdg;
      return string::ToString(xdg.GetHomeTempDirectory());
      #endif
    }

    #ifdef __LINUX__
    string_t GetDrivesDirectory()
    {
      return TEXT("/media/");
    }
    #elif defined(__APPLE__)
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
#elif defined(__WIN__)
    string_t GetProgramFilesDirectory()
    {
      string_t sPath;
      char_t szPath[MAX_PATH_LEN];
      szPath[0] = 0;
      ::SHGetFolderPath(0, CSIDL_PROGRAM_FILESX86 | CSIDL_FLAG_CREATE, 0, SHGFP_TYPE_CURRENT, szPath);
      sPath = string_t(szPath);
      ASSERT(!sPath.empty());
      return sPath;
    }
#endif

    void ChangeDirectoryToExecutablePath(const char* argv0)
    {
      string_t sExecutablePath = spitfire::string::ToString(argv0);
      string_t::size_type last_pos = sExecutablePath.find_last_of(TEXT("\\/"));
      if (last_pos != string_t::npos) {
        const string_t sExecutableFolder = sExecutablePath.substr(0, last_pos);
        ChangeToDirectory(sExecutableFolder);
      }
    }




    string_t GetCurrentDirectory()
    {
      char szDirectory[MAX_PATH_LEN];
      #ifdef __WIN__
      const char* szResult = _getcwd(szDirectory, MAX_PATH_LEN);
      #else
      const char* szResult = getcwd(szDirectory, MAX_PATH_LEN);
      #endif
      if (szResult == nullptr) LOG("getcwd FAILED errno=", errno);
      return spitfire::string::ToString(szDirectory);
    }

    void ChangeToDirectory(const string_t& sDirectory)
    {
      ASSERT(DirectoryExists(sDirectory));
      #ifdef __WIN__
      int iResult = _chdir(spitfire::string::ToUTF8(sDirectory).c_str());
      #else
      int iResult = chdir(spitfire::string::ToUTF8(sDirectory).c_str());
      #endif
      if (iResult != 0) LOG("chdir FAILED iResult=", iResult, " errno=", errno);
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

    string_t StripLastDirectory(const string_t& path)
    {
      // if "folder1/folder2/folder3" return "folder1/folder2/"
      // else ("folder1/folder2/" so ... ) return "folder1/"

      string_t result(path);
      if (spitfire::string::EndsWith(path, sFolderSeparator)) result = spitfire::string::StripAfterLastInclusive(result, sFolderSeparator);

      result = spitfire::string::StripAfterLast(result, sFolderSeparator);
      #ifdef __WIN__
      // Add a folder separator if we have something like "C:"
      if ((result.length() == 2) && (result[1] == TEXT(':'))) result += sFolderSeparator;
      #endif
      return result;
    }

    string_t GetFolder(const string_t& sFilePath)
    {
      string_t p = TEXT("");
      string_t s = sFilePath;

      string_t::size_type i = s.find(sFolderSeparator);
      while (i != string_t::npos) {
        i++;
        p += s.substr(0, i);
        s = s.substr(i);
        i = s.find(sFolderSeparator);
      };

      return p;
    }

    string_t GetLastDirectory(const string_t& sFolderPath)
    {
      return GetFile(sFolderPath);
    }

    string_t GetFile(const string_t& sFilename)
    {
      string_t::size_type i = sFilename.rfind(sFolderSeparator);

      // We didn't find a folder, so just return the whole path
      if (string_t::npos == i) return sFilename;

      i++;
      return sFilename.substr(i);
    }

    string_t GetFileNoExtension(const string_t& sFilename)
    {
      string_t::size_type i = sFilename.find(sFolderSeparator);
      string_t temp = sFilename;
      while (i != string_t::npos) {
        i++;
        temp = temp.substr(i);
        i = temp.find(sFolderSeparator);
      };

      i = temp.find(TEXT("."));
      if (i != string_t::npos) return temp.substr(0, i);

      return TEXT("");
    }

    string_t GetExtensionNoDot(const string_t& sFilename)
    {
      const string_t sExtensionWithDot = GetExtension(sFilename);

      // If it starts with a dot then return the rest of the string
      if (!sExtensionWithDot.empty() && (sExtensionWithDot.c_str()[0] == TEXT('.'))) return sExtensionWithDot.substr(1);

      return sExtensionWithDot;
    }

    string_t GetExtension(const string_t& sFilename)
    {
      return string::ToString(std::filesystem::path(sFilename).extension());
    }



    bool IsPathAbsolute(const string_t& sPath)
    {
      return (
        (sPath[0] == TEXT('/')) ||
        (sPath[0] == TEXT('\\')) ||
        ((sPath[0] != 0) && (sPath[1] == TEXT(':')))
      );
    }

    bool IsPathRelative(const string_t& sFilePath)
    {
      return !IsPathAbsolute(sFilePath);
    }

    string_t MakePathAbsolute(const string_t& sRootPath, const string_t& sRelativePath)
    {
      LOG("");

      // ""
      if (sRelativePath.empty() || (TEXT("./") == sRelativePath)) {
        LOG("0 returning \"", sRootPath, "\"");
        return sRootPath;
      }

      if (TEXT(".") == sRelativePath) {
        LOG("1 returning \"", spitfire::string::StripAfterLastInclusive(sRootPath, TEXT("/")), "\"");
        return spitfire::string::StripAfterLastInclusive(sRootPath, TEXT("/"));
      }

      // "."
      // ".********"
      if ((sRelativePath == TEXT(".")) || (((sRelativePath.length() > 2) && (sRelativePath[0] == TEXT('.'))) && (sRelativePath[1] != TEXT('.')))) {
        string_t expanded(sRelativePath.substr(2));
        LOG("2 returning \"", expanded, "\"");
        return expanded;
      }

      string_t expanded = sRelativePath;
      string_t prefix = sRootPath;
      while (spitfire::string::StartsWith(expanded, TEXT("../"))) {
        expanded.erase(0, 3);
        LOG("prefix=\"", prefix, "\"");
        prefix = StripLastDirectory(prefix);
      };

      LOG("returning \"", prefix, expanded, "\"");
      return prefix + expanded;
    }

    string_t MakePathRelative(const string_t& sBaseFolder, const string_t& sAbsolutePath)
    {
      // Find the last folder separator where the paths differ
      size_t idx = 0;
      size_t iLastFolder = 0;
      char_t cFolder = 0;
      const size_t nBaseLen = sBaseFolder.length();
      const size_t nAbsoluteLen = sAbsolutePath.length();
      while ((idx < nBaseLen) && (idx < nAbsoluteLen) && (sBaseFolder[idx] == sAbsolutePath[idx])) {
        // Store which folder separator to use when creating the relative path later
        if ((sBaseFolder[idx] == '\\') || (sBaseFolder[idx] == '/')) {
          cFolder = sBaseFolder[idx];
          iLastFolder = idx;
        }

        idx++;
      }

      // If cFolder is 0 and we aren't at the end of sBaseFolder or the next character of the Absolute Path isn't a folder separator,
      // the absolute path is completely different (on a different drive, server, etc.) than the base folder so just return the absolute path
      if ((cFolder == 0) &&
        ((sBaseFolder[idx] != 0) || ((sAbsolutePath[idx] != '\\') && (sAbsolutePath[idx] != '/')))
      ) {
        return sAbsolutePath;
      }

      if ((idx == nBaseLen) && ((sAbsolutePath[idx] == '/') ||
        (sAbsolutePath[idx] == '\\') || (sAbsolutePath[idx - 1] == '/') ||
        (sAbsolutePath[idx - 1] == '\\'))
      ) {
        // The whole base folder name is in the file path, so we just trim off the base folder path to get the current file path
        if ((sAbsolutePath[idx] == '/') || (sAbsolutePath[idx] == '\\')) idx++;

        return sAbsolutePath.substr(idx);
      }

      // The absolute path is not a child directory of the base folder so we need to add "../" for each directory up to the parent
      // First find out how many levels deeper we are than the common directory
      size_t nLevels = 1;
      for (; idx < nBaseLen - 1; idx++) {
        if ((sBaseFolder[idx] == '/') || (sBaseFolder[idx] == '\\')) nLevels++;
      }

      // Add a "../" for each folder we have to go up
      ostringstream_t o;
      for (size_t i = 0; i < nLevels; i++) {
        o<<TEXT("..");
        o<<cFolder;
      }

      // Copy the rest of the filename into the result string
      o<<sAbsolutePath.substr(iLastFolder + 1);

      return o.str();
    }


    void AddDirectory(const string_t& sDirectory)
    {
      string_t expanded = MakePathAbsolute(GetThisApplicationDirectory(), sDirectory);

      size_t i = 0;
      const size_t n = vDirectory.size();
      for (i = 0; i < n; i++) {
        if (vDirectory[i] == expanded) return;
      }

      ASSERT(spitfire::string::EndsWith(expanded, sFolderSeparator));
      vDirectory.push_back(expanded);
      LOG("Added ", expanded);
    }

    bool IsFile(const string_t& sFilePath)
    {
      const std::filesystem::path file(spitfire::string::ToUTF8(sFilePath));
      return std::filesystem::is_regular_file(file);
    }

    bool IsFolder(const string_t& sFolderPath)
    {
      const std::filesystem::path file(spitfire::string::ToUTF8(sFolderPath));
      return std::filesystem::is_directory(file);
    }

    bool IsSymlink(const std::string& sPath)
    {
      const std::filesystem::path file(spitfire::string::ToUTF8(sPath));
      return std::filesystem::is_symlink(file);
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
      const std::filesystem::path file(spitfire::string::ToUTF8(sFilename));
      return std::filesystem::exists(file);
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
      const std::filesystem::path folder(spitfire::string::ToUTF8(sFolderName));
      return std::filesystem::exists(folder);
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
          string_t filename = spitfire::string::ToString((*iter) + sFilename);
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
        //   string_t filename = spitfire::string::ToString(spitfire::string::ToString((*iter) + sFilename));
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
        sFilename = spitfire::string::StripBeforeInclusive(sFilename, sFolderSeparator);
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

    spitfire::util::cDateTime GetLastModifiedDate(const string_t& sFilePath)
    {
      ASSERT(FileExists(sFilePath));
      const std::filesystem::path file(spitfire::string::ToUTF8(sFilePath));
      spitfire::util::cDateTime dateTime;
      #if defined(__WIN__) || defined(__LINUX__)
      // HACK: MSVC uses a different type for the std::filesystem::file_time_type so it doesn't have a simple conversion to time_t
      // https://stackoverflow.com/questions/61030383/how-to-convert-stdfilesystemfile-time-type-to-time-t
      //dateTime.Set(std::filesystem::last_write_time(file));
      //const auto ftime = std::filesystem::last_write_time(file);
      //const std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
      //dateTime.SetFromTimeT(cftime);
      const auto tp = std::filesystem::last_write_time(file);
      const auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp - decltype(tp)::clock::now()
                + std::chrono::system_clock::now());
      dateTime.SetFromTimeT(std::chrono::system_clock::to_time_t(sctp));
      #else
      dateTime.SetFromTimeT(std::chrono::system_clock::to_time_t(std::filesystem::last_write_time(file)));
      #endif
      return dateTime;
    }

    void SetLastModifiedDate(const string_t& sFilePath, const util::cDateTime& dateTime)
    {
      ASSERT(FileExists(sFilePath));
      #if defined(__WIN__) || defined(__LINUX__)
      // TODO: This won't work nicely in MSVC or gcc until C++20 because std::filesystem::last_write_time is not a time_t
      (void)dateTime;
      #else
      const std::filesystem::path file(spitfire::string::ToUTF8(sFilePath));
      std::filesystem::last_write_time(file, std::chrono::system_clock::from_time_t(dateTime.GetTimeT()));
      #endif
    }

    uint64_t GetFileSizeBytes(const string_t& sFilename)
    {
      ASSERT(FileExists(sFilename));
      const std::filesystem::path file(spitfire::string::ToUTF8(sFilename));
      return std::filesystem::file_size(file);
    }


#ifdef __WIN__
#pragma pop_macro("CreateFile")
#endif

    // Check that these two paths are pointing to the same file
    bool IsSameFile(const string_t& sFileA, const string_t& sFileB)
    {
      #ifdef __WIN__
      // http://stackoverflow.com/a/5419758

      HANDLE h2 = NULL;

      BY_HANDLE_FILE_INFORMATION bhfi1 = { 0 };
      BY_HANDLE_FILE_INFORMATION bhfi2 = { 0 };
      DWORD access = 0;
      DWORD share = FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE;

      HANDLE h1 = CreateFile(sFileA.c_str(), access, share, NULL, OPEN_EXISTING, ((GetFileAttributes(sFileA.c_str()) & FILE_ATTRIBUTE_DIRECTORY) != 0) ? FILE_FLAG_BACKUP_SEMANTICS : 0, NULL);
      if (INVALID_HANDLE_VALUE != h1) {
        if (!GetFileInformationByHandle(h1, &bhfi1)) bhfi1.dwVolumeSerialNumber = 0;
        h2 = CreateFile(sFileB.c_str(), access, share, NULL, OPEN_EXISTING, ((GetFileAttributes(sFileB.c_str()) & FILE_ATTRIBUTE_DIRECTORY) != 0) ? FILE_FLAG_BACKUP_SEMANTICS : 0, NULL);
        if (!GetFileInformationByHandle(h2, &bhfi2)) bhfi2.dwVolumeSerialNumber = bhfi1.dwVolumeSerialNumber + 1;
        CloseHandle(h2);
      }
      CloseHandle(h1);

      return (INVALID_HANDLE_VALUE != h1) && (INVALID_HANDLE_VALUE != h2) &&
        (bhfi1.dwVolumeSerialNumber == bhfi2.dwVolumeSerialNumber) &&
        (bhfi1.nFileIndexHigh == bhfi2.nFileIndexHigh) &&
        (bhfi1.nFileIndexLow == bhfi2.nFileIndexLow);
      #else
      struct stat stA;
      bool bResultA = (stat(spitfire::string::ToUTF8(sFileA).c_str(), &stA) == 0);
      struct stat stB;
      bool bResultB = (stat(spitfire::string::ToUTF8(sFileB).c_str(), &stB) == 0);

      // Check that the inodes these files match
      return (bResultA && bResultB && (stA.st_ino == stB.st_ino));
      #endif
    }

#ifdef __WIN__
#pragma push_macro("CreateFile")
#undef CreateFile
#endif


    // Check that these two paths are pointing to the same folder
    bool IsSameFolder(const string_t& sFolderA, const string_t& sFolderB)
    {
      return IsSameFile(sFolderA, sFolderB);
    }

#ifdef BUILD_SUPPORT_MD5
    string_t GetMD5(const string_t& sFilename)
    {
      cMD5 m;
      m.CheckFile(spitfire::string::ToUTF8(sFilename));
      return spitfire::string::ToString(m.GetResult());
    }
#endif

#ifdef BUILD_SUPPORT_SHA1
    string_t GetSHA1(const string_t& sFilename)
    {
      cSHA1 s;
      s.CheckFile(spitfire::string::ToUTF8(sFilename));
      return spitfire::string::ToString(s.GetResult());
    }
#endif

    bool CreateDirectory(const string_t& sFolderPath)
    {
      return std::filesystem::create_directories(sFolderPath);
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

    #ifndef __WIN__
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
    #endif

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
      LOG("\"", sFilePath, "\"");
      const string_t sPath = spitfire::filesystem::GetFolder(sFilePath);
      sFileName = spitfire::filesystem::GetFile(sFilePath);

      string::Split(sFilePath, cFilePathSeparator, vFolderNames);
      const size_t n = vFolderNames.size();
      for (size_t i = 0; i < n; i++) LOG("Part=\"", vFolderNames[i], "\"");
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
      const std::filesystem::path file(spitfire::string::ToUTF8(sPath));
      return (std::filesystem::exists(file) && std::filesystem::is_regular_file(file));
    }

    bool cPath::IsFolder() const
    {
      const std::filesystem::path file(spitfire::string::ToUTF8(sPath));
      return (std::filesystem::exists(file) && std::filesystem::is_directory(file));
    }

    string_t cPath::GetDirectory() const // Returns just the directory "/folder1/folder2/"
    {
      ASSERT(IsFolder());
      return filesystem::GetFolder(sPath);
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


    // ** cScopedDirectoryChangeMainThread

    cScopedDirectoryChangeMainThread::cScopedDirectoryChangeMainThread(const string_t& sNewDirectory)
    {
      // chdir and ChangeDirectory are not thread safe so we only allow changing the directory on the main thread
      ASSERT(util::IsMainThread());

      sPreviousDirectory = GetCurrentDirectory();
      ChangeToDirectory(sNewDirectory);
    }

    cScopedDirectoryChangeMainThread::~cScopedDirectoryChangeMainThread()
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
      LOG("");

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
      cPath p(GetHomeTempDirectory(), TEXT("0XXXXXX"));

      char szTempFolderPath[MAX_PATH_LEN];
      strlcpy(szTempFolderPath, spitfire::string::ToUTF8(p.GetFullPath()).c_str(), MAX_PATH_LEN);
      const char* szResult = mkdtemp(szTempFolderPath);
      (void)szResult;
      ASSERT(szResult != nullptr);

      sTemporarySubFolder = spitfire::string::ToString(szTempFolderPath);
    #endif

      if (!DirectoryExists(sTemporarySubFolder)) {
        LOG("Creating sub folder \"", sTemporarySubFolder, "\"");
          CreateDirectory(sTemporarySubFolder);
      }
    }

    cScopedTemporaryFolder::~cScopedTemporaryFolder()
    {
      LOG("");
      ASSERT(DirectoryExists(sTemporarySubFolder));

      DeleteDirectory(sTemporarySubFolder);
    }


    cScopedFolderDeleter::cScopedFolderDeleter(const string_t& _sFullPath) :
      sFullPath(_sFullPath)
    {
    }

    cScopedFolderDeleter::~cScopedFolderDeleter()
    {
      LOG("");
      ASSERT(DirectoryExists(sFullPath));

      DeleteDirectory(sFullPath);
    }


    // ********************************************* cFolderIterator *********************************************

    cFolderIterator::cFolderIterator() :
      bIsIgnoreHiddenFilesAndFolders(false),
      i(0),
      sParentFolder(TEXT(""))
    {
    }

    cFolderIterator::cFolderIterator(const string_t& directory) :
      bIsIgnoreHiddenFilesAndFolders(false),
      i(0),
      sParentFolder(directory)
    {
      if (!DirectoryExists(sParentFolder)) {
        LOG("Folder \"", sParentFolder, "\" does not exist");
        return;
      }
      const std::filesystem::directory_iterator iterEnd;
      for (std::filesystem::directory_iterator iter(sParentFolder); iter != iterEnd; iter++) {
        const string_t sFullPath = string::ToString(iter->path().string());
        const string_t sFile = filesystem::GetFile(sFullPath);
        if ((sFile != TEXT(".")) && (sFile != TEXT(".."))) {
          //LOG("Adding \"", sFile, "\"");
          paths.push_back(sFile);
        }
      }

      // Use a lambda to sort with folders first like this:
      // foldera/
      // folderb/
      // filea.txt
      // fileb.txt
      std::sort(paths.begin(), paths.end(),
        [&](const string_t& lhs, const string_t& rhs)
        {
          const cPath lhsPath(sParentFolder, lhs);
          const cPath rhsPath(sParentFolder, rhs);
          if (lhsPath.IsFolder()) {
            if (!rhsPath.IsFolder()) return true;
          } else if (rhsPath.IsFolder()) return false;

          return (lhs < rhs);
        }
      );
    }

    cFolderIterator::cFolderIterator(const cFolderIterator& rhs)
    {
      bIsIgnoreHiddenFilesAndFolders = rhs.bIsIgnoreHiddenFilesAndFolders;
      i = rhs.i;
      sParentFolder = rhs.sParentFolder;
      paths = rhs.paths;
    }

    void cFolderIterator::SetIgnoreHiddenFilesAndFolders()
    {
      bIsIgnoreHiddenFilesAndFolders = true;
      if (IsValid() && IsHiddenFileOrFolder(GetFileOrFolder())) Next();
    }

    bool cFolderIterator::IsHiddenFileOrFolder(const string_t& sFileOrFolder) const
    {
      return (!sFileOrFolder.empty()) && (sFileOrFolder[0] == TEXT('.'));
    }

    void cFolderIterator::Next()
    {
      const size_t n = paths.size();
      if (i < n) i++;

      if (bIsIgnoreHiddenFilesAndFolders) {
        // Keep iterating until we find a non hidden file or folder
        while ((i < n) && IsHiddenFileOrFolder(paths[i])) {
          i++;
        }
      }
    }

    cFolderIterator& cFolderIterator::operator=(const cFolderIterator& rhs)
    {
      i = rhs.i;
      sParentFolder = rhs.sParentFolder;
      paths = rhs.paths;

      return *this;
    }

    size_t cFolderIterator::GetFileAndFolderCount() const
    {
      return paths.size();
    }

    string_t cFolderIterator::GetFileOrFolder() const
    {
      ASSERT(IsValid());
      return paths[i];
    }

    string_t cFolderIterator::GetFullPath() const
    {
      ASSERT(IsValid());
      return MakeFilePath(sParentFolder, paths[i]);
    }

    bool cFolderIterator::IsValid() const
    {
      return (!paths.empty() && (i < paths.size()));
    }

    bool cFolderIterator::IsFile() const
    {
      ASSERT(IsValid());
      cPath p(sParentFolder, paths[i]);
      return p.IsFile();
    }

    bool cFolderIterator::IsFolder() const
    {
      ASSERT(IsValid());
      cPath p(sParentFolder, paths[i]);
      return p.IsFolder();
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
      bool GetFile(const string_t& sFile, string_t& _sFullPath) const { return _GetFile(sFile, _sFullPath); }

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
      LOG("");

      // If the file doesn't exists return false;
      if (!filesystem::FileExists(sFullPath + sFile)) {
        LOG("File \"", (sFullPath + sFile).c_str(), "\" not found, returning false");
        return false;
      }

      // File exists, fill out the filename and return true
      LOG("File \"", (sFullPath + sFile).c_str(), "\" found, returning true");
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
      LOG("");

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
