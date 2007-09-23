#ifndef CFILESYSTEM_H
#define CFILESYSTEM_H

#ifdef PLATFORM_WINDOWS
#pragma push_macro("CreateDirectory")
#undef CreateDirectory
#pragma push_macro("CreateFile")
#undef CreateFile
#pragma push_macro("FileExists")
#undef FileExists
#endif

namespace BREATHE
{
	namespace FILESYSTEM
	{		
		//Return the full filename "concrete.png" = "data/common/images/" + sFilename
		std::string FindFile(std::string sFilename);

		void AddDirectory(std::string sDirectory);

		std::string GetMD5(std::string sFilename);
		std::string GetSHA1(std::string sFilename);

		std::string GetPath(std::string sFilename);
		std::string GetFile(std::string sFilename);
		std::string GetFileNoExtension(std::string sFilename);
		std::string GetExtension(std::string sFilename);


		bool FileExists(std::string sFilename);
		bool CreateDirectory(std::string sFoldername);
		bool CreateFile(std::string sFilename);

		class iterator
		{
		public:
			iterator();
			~iterator();

			std::string GetName() const;

      operator ++(int);
			operator bool() const;
		};
	}
}

#endif //CFILESYSTEM_H
