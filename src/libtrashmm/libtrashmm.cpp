// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <sys/stat.h>
#include <sys/time.h>

#include <libgen.h>

// Boost headers
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <fcntl.h>
#include <sys/ioctl.h>

// libtrashmm headers
#include <libtrashmm/libtrashmm.h>

// libxdgmm headers
#include <libxdgmm/libxdgmm.h>

// Until C++0x we need to define this ourselves
#define nullptr NULL

namespace trash
{
  bool IsUserRoot()
  {
    return (getuid() == 0); // If getuid is 0 then we are root
  }

  std::string ToString(size_t i)
  {
    std::ostringstream o;
    o<<i;
    return o.str();
  }

  // Get the path without the file
  std::string GetFolderPathFromFilePath(const std::string& sFilePath)
  {
    std::string sFilePathMutable(sFilePath);
    return dirname(&sFilePathMutable[0]);
  }

  // Get the file without the path
  std::string GetFileFromFilePath(const std::string& sFilePath)
  {
    std::ostringstream o;

    std::string::const_iterator iter = sFilePath.begin();
    const std::string::const_iterator iterEnd = sFilePath.end();
    while (iter != iterEnd) {
      if (*iter == '/') o.str("");
      else o<<*iter;

      iter++;
    }

    return o.str();
  }

  bool TestFileExists(const std::string& sFilePath)
  {
    return (access(sFilePath.c_str(), F_OK) == 0);
  }

  bool TestFolderExists(const std::string& sFolderPath)
  {
    return (access(sFolderPath.c_str(), F_OK) == 0);
  }

  // Get current time in RFC 3339 format
  // 2012-03-29T21:27:38
  std::string GetCurrentTimeStampRFC3393()
  {
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    struct tm* pTime = nullptr;
    pTime = localtime(&tv.tv_sec);
    if (pTime != nullptr) {
      char fmt[64];
      strftime(fmt, sizeof(fmt), "%Y-%m-%dT%H:%M:%S", pTime);
      char buf[64];
      snprintf(buf, sizeof(buf), fmt, tv.tv_usec);
      return buf;
    }

    return "";
  }

  std::string GetUnixErrorString()
  {
    char szError[255];
    szError[0] = 0; // Just in case strerror_r fails
    strerror_r(errno, szError, 255);
    return std::string(szError);
  }

  bool IsFile(const std::string& sFilePath)
  {
    struct stat stBuffer;
    if (stat(sFilePath.c_str(), &stBuffer) == 0) return S_ISREG(stBuffer.st_mode);

    std::cout<<"IsFile FAILED for \""<<sFilePath<<"\""<<std::endl;
    //LTRACEUNIXERRORMSG("IsFile failed due to: ");
    return false;
  }

  bool IsFolder(const std::string& sFolderPath)
  {
    struct stat stBuffer;
    if (stat(sFolderPath.c_str(), &stBuffer) == 0) return S_ISDIR(stBuffer.st_mode);

    std::cout<<"IsFolder FAILED for \""<<sFolderPath<<"\""<<std::endl;
    //LTRACEUNIXERRORMSG("IsFolder FAILED due to: ");
    return false;
  }

  bool IsSymlink(const std::string& sPath)
  {
    struct stat st;
    return ((lstat(sPath.c_str(), &st) != -1) && ((st.st_mode & S_IFMT) == S_IFLNK));
  }

  bool MoveFile(const std::string& sFromFile, const std::string& sToFile)
  {
    // NOTE: sToFile must be the actual file that we want to move to, not the parent folder
    assert(!IsFolder(sToFile));

    int iResult = rename(sFromFile.c_str(), sToFile.c_str());
    if (iResult != 0) {
      std::cout<<"MoveFile From \""<<sFromFile<<"\" to \""<<sToFile<<"\" FAILED Due to "<<GetUnixErrorString()<<std::endl;
      return false;
    }

    return true;
  }

  bool MoveFolder(const std::string& sFromFolder, const std::string& sToFolder)
  {
    int iResult = rename(sFromFolder.c_str(), sToFolder.c_str());
    if (iResult != 0) {
      std::cout<<"MoveFolder From \""<<sFromFolder<<"\" to \""<<sToFolder<<"\" FAILED Due to "<<GetUnixErrorString()<<std::endl;
      return false;
    }

    return true;
  }

  bool CreateFolder(const std::string& sFolderPath)
  {
    return boost::filesystem::create_directories(sFolderPath);
  }

  std::string URIEncodeFilePath(const std::string& sFilePath)
  {
    return sFilePath;
  }

  // Returns "/home/$USER/.local/share/Trash/info"
  std::string GetInfoFolder()
  {
    xdg::cXdg xdg;
    return xdg.GetHomeDataDirectory() + "/Trash/info";
  }

  // Returns "/home/$USER/.local/share/Trash/files"
  std::string GetFilesFolder()
  {
    xdg::cXdg xdg;
    return xdg.GetHomeDataDirectory() + "/Trash/files";
  }

  RESULT MoveFileToTrash(const std::string& sFilePath)
  {
    assert(!IsUserRoot());

    if (!TestFileExists(sFilePath)) return RESULT::ERROR_FILE_DOES_NOT_EXIST;

    // TODO: Files that the user trashes from the same file system (device/partition) should be stored here.
    // If this directory is needed for a trashing operation but does not exist, the implementation SHOULD automatically create it.

    const std::string sInfoDirectory = GetInfoFolder();
    if (!TestFolderExists(sInfoDirectory)) CreateFolder(sInfoDirectory);

    const std::string sFilesDirectory = GetFilesFolder();
    if (!TestFolderExists(sFilesDirectory)) CreateFolder(sFilesDirectory);

    const std::string sDirectory = GetFolderPathFromFilePath(sFilePath); // Get the path without the file
    const std::string sFile = GetFileFromFilePath(sFilePath); // Get the file without the path

    // Find a free path to copy the file to
    std::string sTrashFile = "/" + sFile;
    std::string sTrashFilePath = sFilesDirectory + "/" + sTrashFile;
    size_t i = 0;
    while (TestFileExists(sTrashFilePath)) {
      i++;
      sTrashFile = sFile + ToString(i);
      sTrashFilePath = sFilesDirectory + "/" + sTrashFile;
    }
    const std::string sTrashInfoPath = sInfoDirectory + "/" + sTrashFile + ".trashinfo";

    std::ofstream o(sTrashInfoPath);
    o<<"[Trash Info]"<<std::endl;
    o<<"Path="<<sFilePath<<std::endl;
    o<<"DeletionDate="<<GetCurrentTimeStampRFC3393()<<std::endl;

    MoveFile(sFilePath, sTrashFilePath);

    /*// Check if the process should work or not
    For each file that would be moved to the trash:
      if (file owner is different) return RESULT::ERROR_FILE_DIFFERENT_OWNER;
      if (file permissions would not allow moving it) return
    RESULT::ERROR_FILE_INCORRECT_PERMISSIONS; needs rw
      on other error return RESULT::ERROR_UNKNOWN;

    // Create entry in "info" folder
    const std::string sRandomString = generate random string
    create file sFilePath + "." + sRandomString + ".trashinfo"

    // Move the file or folder to the "files" folder
    const std::string sFilesFolder = GetFilesFolder();
    if (file) Move file to sFilesFolder + "." + sRandomString;
    else Move folder to sFilesFolder + "." + sRandomString;

    The $trash/info directory contains an “information file” for every
    file and directory in $trash/files. This file MUST have exactly the
    same name as the file or directory in $trash/files, plus the extension
    “.trashinfo”.

    The key “DeletionDate” contains the date and time when the
    file/directory was trashed. The date and time are to be in the
    YYYY-MM-DDThh:mm:ss format (see RFC 3339). The time zone should be the
    user's (or filesystem's) local time. The value type for this key is
    “string”.

    Example:
    [Trash Info]
    Path=/home/user/Music/Artist/Artist - Title.mp3
    DeletionDate=20040831T22:32:08

    if ("Folder" already exists) create "Folder.1";

    if ("Thumbs.db" already exists) create "Thumbs.6.db";

    When trashing a file or directory, the implementation MUST create
    the corresponding file in $trash/info first. The names of the
    files/directories in $trash/info SHOULD be somehow related to original
    file names. This can help manual recovery in emergency cases (for
    example, if the corresponding info file is lost).*/

    return RESULT::SUCCESS;
  }

  RESULT MoveFolderToTrash(const std::string& sFolderPath)
  {
    return MoveFileToTrash(sFolderPath);
  }
}
