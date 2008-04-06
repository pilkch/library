#include <cassert>

#include <string>
#include <list>
#include <map>
#include <vector>

#include <firestarter/firestarter.h>
#include <firestarter/profile.h>

#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/xml.h>

namespace firestarter
{
	namespace profile
	{
		class profile
		{
		public:
			profile() : 
				bFemale(true),
				fRed(0.0f),
				fGreen(0.0f),
				fBlue(0.0f)
			{}
			
			std::string sName;
			bool bFemale;
			std::string sCharacterCute;
			std::string sCharacterRealistic;
			float fRed;
			float fGreen;
			float fBlue;
		};
		
		std::list<profile> lProfile;
		std::list<profile>::iterator iter;
					
		// For internal use to check before each return that we actually have data to send
		bool IsValid()
		{
			return (lProfile.size() > 0) && (iter != lProfile.end());
		}
		
		
		// *** Iterator
		iterator::iterator()
		{
			// Load our profiles from the xml file
			{
				breathe::xml::cNode root(GetProfilesPath());

				breathe::xml::cNode::iterator iter(root);
				if (!iter) return;
					
				iter.FindChild("profiles");
				
				iter.FindChild("profile");
				
				while(iter)
				{
					// Ok, we have a profile, load it up
					profile p;

					iter.GetAttribute("name", p.sName);

					std::string gender;
					iter.GetAttribute("gender", gender);
					p.bFemale = (gender != "male");

          iter.GetAttribute("cute", p.sCharacterCute);
          iter.GetAttribute("realistic", p.sCharacterRealistic);
					
					float values[3];
					iter.GetAttribute("colour", &values[0], 3);
					p.fRed = values[0];
					p.fGreen = values[1];
					p.fBlue = values[2];
          
					// Actually add it to the list
					lProfile.push_back(p);

					// Go to the next profile
					iter++;
				};
			}

			// Set us to the first element
			iter = lProfile.begin();
		}
		
		iterator::~iterator()
		{
			// Clear profiles from memory
			lProfile.clear();
			iter = lProfile.end();
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
		
		bool iterator::IsFemale() const
		{
			if (!IsValid()) return true;
			
			return (*iter).bFemale;
		}
		
		bool iterator::IsMale() const
		{
			if (!IsValid()) return false;
			
			return !(*iter).bFemale;
		}

		// Default character: if cute has been chosen return that, else return realistic
		std::string iterator::GetCharacter() const
		{
			if (!IsValid()) return "";
			
			if ((*iter).sCharacterCute != "") return (*iter).sCharacterCute;
			if ((*iter).sCharacterRealistic != "") return (*iter).sCharacterRealistic;
			return "";
		}
		
		std::string iterator::GetCharacterCute() const
		{
			if (!IsValid()) return "";
			
			return (*iter).sCharacterCute;
		}
		
		std::string iterator::GetCharacterRealistic() const
		{
			if (!IsValid()) return "";
			
			return (*iter).sCharacterRealistic;
		}
		
		void iterator::GetColour(float* fRed, float* fGreen, float* fBlue) const
		{
			assert(fRed != NULL);
			assert(fGreen != NULL);
			assert(fBlue != NULL);

			if (!IsValid()) 
			{
				*fRed = *fGreen = *fBlue = 0.0f;
				return;
			}
			
			*fRed = (*iter).fRed;
			*fGreen = (*iter).fGreen;
			*fBlue = (*iter).fBlue;
		}
	}
}
