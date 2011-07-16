#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <spitfire/util/cString.h>

// http://www.boost.org/doc/libs/1_39_0/libs/filesystem/doc/index.htm
// http://www.boost.org/doc/libs/1_39_0/libs/filesystem/doc/index.htm#tutorial
// http://www.boost.org/doc/libs/1_39_0/libs/filesystem/example/file_size.cpp
// http://www.boost.org/doc/libs/1_39_0/libs/filesystem/example/simple_ls.cpp
// http://www.ibm.com/developerworks/aix/library/au-boostfs/index.html

// Shared folder mirrors the directory structure of each program
//
// ~/dev/shared/lang.h
// ~/dev/shared/lang_us.h
// ~/dev/shared/data/textures/
//                   fonts/
//                   materials/
//
// ~/dev/drive/lang.h
// ~/dev/drive/lang_us.h
// ~/dev/drive/data/textures/
//                  fonts/
//                  materials/
//

#ifdef __WIN__
#pragma push_macro("CreateDirectory")
#undef CreateDirectory
#pragma push_macro("CreateFile")
#undef CreateFile
#pragma push_macro("FileExists")
#undef FileExists
#endif

namespace spitfire
{
  namespace filesystem
  {
    void SetThisExecutable(const string_t& executable);
    string_t GetThisApplicationDirectory();
    string_t GetApplicationSettingsDirectory(const string_t& sApplication);
#ifdef SPITFIRE_APPLICATION_NAME_LWR
    inline string_t GetThisApplicationSettingsDirectory() { return GetApplicationSettingsDirectory(TEXT(SPITFIRE_APPLICATION_NAME_LWR)); }
    inline string_t GetSpitfireSettingsDirectory() { return GetApplicationSettingsDirectory(TEXT("spitfire")); }
#endif
    string_t GetHomeDirectory();
    string_t GetHomeConfigurationFilesDirectory();
    string_t GetHomeDesktopDirectory();
    string_t GetHomeImagesDirectory();
    string_t GetHomeMusicDirectory();
    string_t GetHomeDownloadsDirectory();
    string_t GetTempDirectory();
#ifdef __APPLE__
    string_t GetResourcesPath();
#endif

    uint32_t GetLastModifiedDate(const string_t& sFilename);
    uint64_t GetFileSize(const string_t& sFilename);

    string_t GetCurrentDirectory();
    void ChangeToDirectory(const string_t& sDirectory);


#ifdef BUILD_SUPPORT_MD5
    string_t GetMD5(const string_t& sFilename);
#endif
#ifdef BUILD_SUPPORT_SHA1
    string_t GetSHA1(const string_t& sFilename);
#endif


    // Permissions

    void SetFileExecutableForThisUser(const string_t& sFilename);
    void SetFileExecutableForEveryone(const string_t& sFilename);
    void SetFileReadAndWriteAccessForThisUser(const string_t& sFilename);
    void SetFileReadAndWriteAccessForEveryone(const string_t& sFilename);
    void SetFolderReadAndWriteAccessForThisUserRecursively(const string_t& sFolder);
    void SetFolderReadAndWriteAccessForEveryoneRecursively(const string_t& sFolder);

    bool IsFolderWritable(const string_t& sFolderPath);

    string_t StripLastDirectory(const string_t& path);
    string_t ExpandPath(const string_t& path);

    string_t GetPath(const string_t& sFilename);
    string_t GetFile(const string_t& sFilename);
    string_t GetFileNoExtension(const string_t& sFilename);
    string_t GetExtension(const string_t& sFilename);

    bool FileExists(const string_t& sFilename);
    bool DirectoryExists(const string_t& sFoldername);
    bool CreateFile(const string_t& sFilename);
    bool CreateDirectory(const string_t& sFoldername);

    bool DeleteFile(const string_t& sFilename);
    bool DeleteDirectory(const string_t& sFoldername);

    void CopyContentsOfFile(const string_t& sFrom, const string_t& sTo);

    // File Opening functions
    // No point in totally wrapping ofstream/ifstream.  Because it isnt needed.  Do we even need this?
    // The idea is that we can search directories to find the file and then open the correct one.
    std::ifstream OpenTextFileRead(const string_t& sFilename);
    std::ifstream OpenBinaryFileRead(const string_t& sFilename);

    std::ofstream OpenTextFileWrite(const string_t& sFilename);
    std::ofstream OpenBinaryFileWrite(const string_t& sFilename);


    void AddDirectory(const string_t& sDirectory);

    // This is the prefered method as it will find files in $shared/data and $app/data as well as any other paths that have been added
    bool FindResourceFile(const string_t& sFilename, string_t& sOutFilename);
    bool FindResourceFile(const string_t& sPath, const string_t& sFilename, string_t& sOutFilename);

    bool FindFile(const string_t& sFilename, string_t& sOutFilename);


    string_t MakeFilePath(const string_t& sDirectory, const string_t& sFile);
    string_t MakeFilePath(const string_t& sDirectory, const string_t& sSubDirectory, const string_t& sFile);


    // TODO: Remove this
    class cScopedDirectoryChange
    {
    public:
      explicit cScopedDirectoryChange(const string_t& sNewDirectory);
      ~cScopedDirectoryChange();

    private:
      string_t sPreviousDirectory;
    };

    class cScopedTemporaryFolder
    {
    public:
      cScopedTemporaryFolder();
      ~cScopedTemporaryFolder();

      const string_t& GetFolder() const { return sTemporarySubFolder; }

    private:
      string_t sTemporarySubFolder;
    };



    class cPathBuilder
    {
    public:
      explicit cPathBuilder(const string_t& sFolderPath);

      void PushBackFolder(const string_t& sFolderName) { folders.push_back(sFolderName); }
      void PopFolder() { folders.pop_back(); }

      string_t GetFullPath() const { return GetFullPathWithFolderSeparator(); }
      string_t GetFullPathWithFolderSeparator() const;
      string_t GetFullPathWithFolderSeparatorAndAddedFileName(const string_t& sFileName) const;

      size_t GetNumberOfFolders() { return folders.size(); }

    private:
      std::list<string_t> folders;
    };


    class cFilePathParser
    {
    public:
      explicit cFilePathParser(const string_t& sFilePath);

      size_t GetFolderCount() const { return vFolderNames.size(); }
      const string_t& GetFolder(size_t index) { ASSERT(index < GetFolderCount()); return vFolderNames[index]; }

      bool IsFile() const { return !sFileName.empty(); }
      const string_t& GetFile() const { ASSERT(IsFile()); return sFileName; }

    private:
      std::vector<string_t> vFolderNames;
      string_t sFileName;
    };


    class cPath
    {
    public:
      explicit cPath(const string_t& sDirectory);
      explicit cPath(const string_t& sDirectory, const string_t& sFile);
      explicit cPath(const string_t& sDirectory, const string_t& sSubDirectory, const string_t& sFile);

      bool IsFile() const;
      bool IsDirectory() const;

      string_t GetDirectory() const; // Returns just the directory "/folder1/folder2/"
      string_t GetFile() const; // Returns just the file "file.txt"
      string_t GetExtenstion() const; // Returns just the extension ".txt"
      string_t GetFullPath() const; // Returns the full path "/folder1/folder2/file.txt"

    private:
      cPath();

      string_t sPath;
    };


    class cFolderIterator
    {
    public:
      cFolderIterator();
      explicit cFolderIterator(const string_t& directory);
      cFolderIterator(const cFolderIterator& rhs);

      bool SetIgnoreHiddenFilesAndFolders();

      bool IsFile() const;
      bool IsDirectory() const;

      string_t GetParentFolder() const;
      string_t GetFile() const;
      string_t GetFullPath() const; // Returns the full path "/folder1/folder2/file.txt"

      bool HasChildren() const;

      void GoToFirstChild();
      void Next();

      bool operator==(const cFolderIterator& rhs);
      bool operator!=(const cFolderIterator& rhs);

      cFolderIterator& operator=(const cFolderIterator& rhs);

    private:
      bool IsValid() const;

      bool bIsIgnoreHiddenFilesAndFolders;
      bool bIsEndIterator;
      size_t i;
      string_t sParentFolder;
      std::vector<string_t> paths;
    };




    void Create();
    void Destroy();

    void Mount(const string_t& sPath);
    void MountHighPriority(const string_t& sPath);
    void MountLowPriority(const string_t& sPath);

    void Unmount(const string_t& sPath);

    bool GetFile(const string_t& sFile, string_t& sFullPath);

    bool UnzipFileToFolder(const string_t& sFullPathToZipFile, const string_t& sFullPathToExtractFilesTo);
  }
}

#endif // FILESYSTEM_H
