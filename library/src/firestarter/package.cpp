#include <cassert>

#include <string>
#include <sstream>

#include <vector>
#include <list>
#include <map>

#include <firestarter/firestarter.h>
#include <firestarter/package.h>

#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/xml.h>
#include <breathe/util/filesystem.h>

// 1KB=1024
// 1MB=1048576
// 1GB=1073741824

const unsigned long uiKB = 1024;
const unsigned long uiMB = uiKB * uiKB;
const unsigned long uiGB = uiMB * uiKB;

namespace firestarter
{
	namespace package
	{
		class package
		{
		public:
			package() : 
				installed(false),
				ulSize(0)
			{}
			
			
			std::string sName;
			bool installed;
			unsigned long ulSize;
		};
		
		std::list<package> lPackage;
		std::list<package>::iterator iter;
					
		// For internal use to check before each return that we actually have data to send
		bool IsValid()
		{
			return (lPackage.size() > 0) && (iter != lPackage.end());
		}
		
		
		// *** Iterator
		iterator::iterator()
		{
			// Find packages by iterating through the folders of the "packages/" directory
			breathe::filesystem::directory_iterator directory(GetPackagesDirectory());
			
			while (directory)
			{
				breathe::xml::cNode root(directory.GetName() + "firestarter.xml");

				breathe::xml::cNode::iterator iter(root);
				if (!iter) continue;
					
				iter.FindChild("profiles");
				
				iter.FindChild("profile");
				
				while(iter)
				{

					iter++;
				};
			}


			// Set us to the first element
			iter = lPackage.begin();
		}
		
		iterator::~iterator()
		{
			// Clear packages from memory
			lPackage.clear();
			iter = lPackage.end();
		}
		
		iterator::operator bool() const
		{
			return IsValid();
		}
		
		iterator& iterator::operator++ ()
		{
			iter++;
			return *this;
		}

		void iterator::operator++ (int)
		{
			operator++();
		}
		
		std::string iterator::GetName() const
		{
			if (!IsValid()) return "";
			
			return (*iter).sName;
		}
		
		bool iterator::IsInstalled() const
		{
			if (!IsValid()) return false;
			
			return (*iter).installed;
		}
    
		unsigned long iterator::GetSize() const
		{
			if (!IsValid()) return 0;
			
			return (*iter).ulSize;
		}
		
		unsigned long iterator::GetSizeKB() const
		{
			if (!IsValid()) return 0;
			
			return ((*iter).ulSize) % uiKB;
		}
		
		unsigned long iterator::GetSizeMB() const
		{
			if (!IsValid()) return 0;
			
			return ((*iter).ulSize) % uiMB;
		}

		std::string iterator::GetSizef() const
		{
			if (!IsValid()) return "";
			
			unsigned long ulSize = (*iter).ulSize;

			std::ostringstream t;
			if(ulSize < uiKB)				t << ulSize << "B";
			else if(ulSize < uiMB)	t << (ulSize / uiKB) << "." << ((ulSize % uiKB) / 10UL) << "KB";
			else if(ulSize < uiGB)	t << (ulSize / uiMB) << "." << ((ulSize % uiMB) / 10240UL) << "MB";
			else										t << (ulSize / uiGB) << "." << ((ulSize % uiGB) / 10737418UL) << "GB";

			return t.str();
		}
	}
}
