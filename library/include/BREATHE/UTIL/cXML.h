#ifndef CXML_H
#define CXML_H

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cColour.h>

namespace BREATHE
{
	namespace MATH
	{
		class cVec3;
		class cColour;
	}

	namespace XML
	{
		typedef std::map<std::string, std::string>::iterator iterator;

		// Can be made up of either
		// a) cNode with sName
		// b) cNode with sName, vChild
		// c) cNode with sName, mAttribute
		// d) cNode with sName, mAttribute, vChild
		// e) cNode with sContentOnly, no mAttribute, no vChild
		class cNode
		{
		public:
			cNode(std::string inFilename);
			cNode(cNode *inParent);
			~cNode();
	
#ifdef BUILD_DEBUG
		void PrintToLog(std::string sTab="");
#endif //BUILD_DEBUG

			std::string sName;
			std::map<std::string, std::string> mAttribute; // One of each attribute

			std::string sContentOnly;

			bool IsNameAndAttributesAndChildren() { return !bContentOnly; }
			bool IsContentOnly() { return bContentOnly; }


			template <class T>
				bool GetAttribute(std::string sAttribute, T* pValue)
			{
				iterator iter = mAttribute.find(sAttribute);
				if(iter != mAttribute.end())
				{
					if(pValue)
					{
						std::stringstream stm(iter->second);
						stm >> *pValue;
					}
					return true;
				}

				return false;
			}

			bool GetAttribute(std::string sAttribute, std::string* pValue);
			bool GetAttribute(std::string sAttribute, bool* pValue);
			bool GetAttribute(std::string sAttribute, MATH::cVec3* pValue);
			bool GetAttribute(std::string sAttribute, MATH::cColour* pValue);

			cNode* FirstChild();
			cNode* FindChild(std::string sName);
			cNode* Next();
			cNode* Next(std::string sName);

		protected:
			std::vector<cNode*> vChild;

			cNode* pParent;
			cNode* pNext;

			bool bContentOnly;


			cNode* AddNode();
			void AddAttribute(std::string inAttribute, std::string inValue);
			void AddContent(std::string inContent);

		private:
			std::string Parse(std::string sData, cNode* pPrevious);
		};
	}
}

#endif //CXML_H
