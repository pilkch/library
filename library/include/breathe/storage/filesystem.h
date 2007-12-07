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
		class directory_iterator;
		class file_iterator;

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




		class path
		{
		public:
			path(const string_t& file_or_directory);

			bool IsFile() const;
			bool IsDirectory() const;

			string_t GetDirectory() const; // Returns just the directory "/folder1/folder2/"
			string_t GetFile() const; // Returns just the file "file.txt"
			string_t GetExtenstion() const; // Returns just the extension ".txt"
			string_t str() const; // Returns the full path "/folder1/folder2/file.txt"
			
		private:
			path();
			NO_COPY(path);

			string_t sPath;
		};


		class iterator_node
		{
		public:
			iterator_node() : pParent(nullptr) {}
			
			iterator_node* pParent;
		};

		class iterator
		{
		public:
			iterator();
			iterator(const string_t& directory);
			iterator(const iterator& rhs);
			~iterator();

			string_t GetName() const;
			bool HasChildren() const;

			void GoToFirstChild();
			
			operator bool() const;

			iterator operator ++(int);
			iterator operator =(const iterator& rhs);

		private:
			path currentPath;
			std::vector<string_t> paths;
		};
	}
}

#endif //FILESYSTEM_H
