#ifndef CFILESYSTEM_H
#define CFILESYSTEM_H

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
		class directory_iterator;
		class file_iterator;

		void SetThisExecutable(const std::string& executable);
		std::string GetThisApplicationDirectory();

		//Return the full filename "concrete.png" = "data/common/images/" + sFilename
		std::string FindFile(const std::string& sFilename);
		std::string FindFile(const std::string& sPath, const std::string& sFilename);

		void AddDirectory(const std::string& sDirectory);

#ifndef FIRESTARTER
		std::string GetMD5(const std::string& sFilename);
		std::string GetSHA1(const std::string& sFilename);
#endif

		std::string GetPath(const std::string& sFilename);
		std::string GetFile(const std::string& sFilename);
		std::string GetFileNoExtension(const std::string& sFilename);
		std::string GetExtension(const std::string& sFilename);

		bool FileExists(const breathe::string::string_t& sFilename);
		bool CreateDirectory(const breathe::string::string_t& sFoldername);
		bool CreateFile(const breathe::string::string_t& sFilename);


		// File Opening functions
		// No point in totally wrapping ofstream/ifstream.  Because it isnt needed.  Do we even need this?  
		// The idea is that we can search directories to find the file and then open the correct one. 
		std::ifstream& OpenTextFileRead(const breathe::string::string_t& sFilename);
		std::ifstream& OpenBinaryFileRead(const breathe::string::string_t& sFilename);

		std::ofstream& OpenTextFileWrite(const breathe::string::string_t& sFilename);
		std::ofstream& OpenBinaryFileWrite(const breathe::string::string_t& sFilename); 




		class path
		{
		public:
			path(const std::string& file_or_directory);

			bool IsFile() const;
			bool IsDirectory() const;

			std::string GetDirectory() const; // Returns just the directory "/folder1/folder2/"
			std::string GetFile() const; // Returns just the file "file.txt"
			std::string GetExtenstion() const; // Returns just the extension ".txt"
			std::string str() const; // Returns the full path "/folder1/folder2/file.txt"
			
		private:
			path();

			std::string sPath;
		};

		class directory_iterator
		{
		public:
			directory_iterator();
			directory_iterator(const std::string& directory);
			directory_iterator(const directory_iterator& rhs);
			~directory_iterator();

			std::string GetName() const;
			bool HasChildren() const;

			directory_iterator GetDirectoryIterator() const;
			file_iterator GetFileIterator() const;

			operator ++(int);
			operator bool() const;

			operator =(const directory_iterator& rhs);
		};

		class file_iterator
		{
		public:
			file_iterator();
		};
	}
}

#endif //CFILESYSTEM_H
