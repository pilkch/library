#ifndef FILESYSTEM_H
#define FILESYSTEM_H

// http://www.boost.org/doc/libs/1_37_0/libs/filesystem/doc/index.htm
// http://www.boost.org/doc/libs/1_37_0/libs/filesystem/example/file_size.cpp
// http://www.boost.org/doc/libs/1_37_0/libs/filesystem/example/simple_ls.cpp

// Just add these links to filesystem.h and next time there is a problem
// there convert over to boost::filesystem


// Shared folder mirrors the directory structure of each program
//
// ~/Dev/shared/lang.h
// ~/Dev/shared/lang_us.h
// ~/Dev/shared/data/textures/
//                   fonts/
//                   materials/
//
// ~/Dev/drive/lang.h
// ~/Dev/drive/lang_us.h
// ~/Dev/drive/data/textures/
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

// TODO: Move this to a shared build.h, config.h etc.
#define BREATHE_APPLICATION_NAME_LWR "crank"

namespace breathe
{
  namespace filesystem
  {
    void SetThisExecutable(const string_t& executable);
    string_t GetThisApplicationDirectory();
    string_t GetApplicationSettingsDirectory(const string_t& sApplication);
    inline string_t GetThisApplicationSettingsDirectory() { return GetApplicationSettingsDirectory(TEXT(BREATHE_APPLICATION_NAME_LWR)); }
    inline string_t GetBreatheSettingsDirectory() { return GetApplicationSettingsDirectory(TEXT("breathe")); }
    string_t GetHomeDirectory();
    string_t GetHomeImagesDirectory();
    string_t GetHomeMusicDirectory();
    string_t GetTempDirectory();
#ifdef __APPLE__
    string_t GetResourcesPath();
#endif

    uint32_t GetLastModifiedDate(const string_t& sFilename);
    uint64_t GetFileSize();

    string_t GetCurrentDirectory();
    void ChangeToDirectory(const string_t& sDirectory);


#ifndef FIRESTARTER
    string_t GetMD5(const string_t& sFilename);
    string_t GetSHA1(const string_t& sFilename);
#endif

    string_t StripLastDirectory(const string_t& path);
    string_t ExpandPath(const string_t& path);

    string_t GetPath(const string_t& sFilename);
    string_t GetFile(const string_t& sFilename);
    string_t GetFileNoExtension(const string_t& sFilename);
    string_t GetExtension(const string_t& sFilename);

    bool FileExists(const breathe::string_t& sFilename);
    bool CreateDirectory(const breathe::string_t& sFoldername);
    bool CreateFile(const breathe::string_t& sFilename);

    bool DeleteFile(const breathe::string_t& sFilename);
    bool DeleteDirectory(const breathe::string_t& sFoldername);

    void CopyContentsOfFile(const breathe::string_t& sFrom, const breathe::string_t& sTo);

    // File Opening functions
    // No point in totally wrapping ofstream/ifstream.  Because it isnt needed.  Do we even need this?
    // The idea is that we can search directories to find the file and then open the correct one.
    std::ifstream OpenTextFileRead(const breathe::string_t& sFilename);
    std::ifstream OpenBinaryFileRead(const breathe::string_t& sFilename);

    std::ofstream OpenTextFileWrite(const breathe::string_t& sFilename);
    std::ofstream OpenBinaryFileWrite(const breathe::string_t& sFilename);


    void AddDirectory(const string_t& sDirectory);

    // This is the prefered method as it will find files in $shared/data and $app/data as well as any other paths that have been added
    bool FindResourceFile(const string_t& sFilename, string_t& sOutFilename);
    bool FindResourceFile(const string_t& sPath, const string_t& sFilename, string_t& sOutFilename);

    bool FindFile(const string_t& sFilename, string_t& sOutFilename);


    class cScopedDirectoryChange
    {
    public:
      explicit cScopedDirectoryChange(const string_t& sNewDirectory);
      ~cScopedDirectoryChange();

    private:
      string_t sPreviousDirectory;
    };

    class path
    {
    public:
      explicit path(const string_t& sDirectory);
      explicit path(const string_t& sDirectory, const string_t& sFile);
      explicit path(const string_t& sDirectory, const string_t& sSubDirectory, const string_t& sFile);

      bool IsFile() const;
      bool IsDirectory() const;

      string_t GetDirectory() const; // Returns just the directory "/folder1/folder2/"
      string_t GetFile() const; // Returns just the file "file.txt"
      string_t GetExtenstion() const; // Returns just the extension ".txt"
      string_t GetFullPath() const; // Returns the full path "/folder1/folder2/file.txt"
      string_t str() const { return GetFullPath(); } // Returns the full path "/folder1/folder2/file.txt"

    private:
      path();
      NO_COPY(path);

      string_t sPath;
    };


    class iterator
    {
    public:
      iterator();
      explicit iterator(const string_t& directory);
      explicit iterator(const iterator& rhs);

      bool IsFile() const;
      bool IsDirectory() const;

      string_t GetParentFolder() const;
      string_t GetFile() const;
      string_t GetFullPath() const; // Returns the full path "/folder1/folder2/file.txt"

      bool HasChildren() const;

      void GoToFirstChild();

      bool operator==(const iterator& rhs);
      bool operator!=(const iterator& rhs);

      iterator& operator++(int);
      iterator& operator=(const iterator& rhs);

    private:
      bool IsValid() const;

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
  }
}

#endif // FILESYSTEM_H
