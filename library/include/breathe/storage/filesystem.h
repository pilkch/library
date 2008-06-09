#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#ifdef __WIN__
#pragma push_macro("CreateDirectory")
#undef CreateDirectory
#pragma push_macro("CreateFile")
#undef CreateFile
#pragma push_macro("FileExists")
#undef FileExists
#endif

namespace breathe
{
	namespace filesystem
	{
		void SetThisExecutable(const string_t& executable);
		string_t GetThisApplicationDirectory();

		//Return the full filename "concrete.png" = "data/common/images/" + sFilename
		string_t FindFile(const string_t& sFilename);
		string_t FindFile(const string_t& sPath, const string_t& sFilename);

		void AddDirectory(const string_t& sDirectory);

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

		// File Opening functions
		// No point in totally wrapping ofstream/ifstream.  Because it isnt needed.  Do we even need this?
		// The idea is that we can search directories to find the file and then open the correct one.
		std::ifstream OpenTextFileRead(const breathe::string_t& sFilename);
		std::ifstream OpenBinaryFileRead(const breathe::string_t& sFilename);

		std::ofstream OpenTextFileWrite(const breathe::string_t& sFilename);
		std::ofstream OpenBinaryFileWrite(const breathe::string_t& sFilename);

    uint32_t GetFileSize();

    string_t GetCurrentDirectory();
    void ChangeToDirectory(const string_t& sDirectory);

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
  }
}

#endif //FILESYSTEM_H
