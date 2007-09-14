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
		// Can be made up of either
		// a) cNode with sName
		// b) cNode with sName, vChild
		// c) cNode with sName, mAttribute
		// d) cNode with sName, mAttribute, vChild
		// e) cNode with sContentOnly, no mAttribute, no vChild
		class cNode
		{
		private:
			typedef std::map<std::string, std::string>::iterator attribute_iterator;

		public:
			cNode(std::string inFilename);
			cNode(cNode *inParent);
			~cNode();

			void SaveToFile(std::string inFilename);

			bool IsNameAndAttributesAndChildren() { return !bContentOnly; }
			bool IsContentOnly() { return bContentOnly; }

			std::string GetName();

			template <class T>
			bool GetAttribute(std::string sAttribute, T* pValue)
			{
				attribute_iterator iter = mAttribute.find(sAttribute);
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

			class cIterator
			{
			public:
				cIterator(const cIterator& rhs);
				cIterator(const cNode& rhs);
				cIterator& operator=(const cIterator& rhs);
				cIterator& operator=(const cNode& rhs);

				operator bool();

				operator++(int);
				void Next(std::string sName);
				
				void FirstChild();
				void FindChild(std::string sName);

				std::string GetName();

				template <class T>
				bool GetAttribute(std::string sAttribute, T* pValue);

			private:
				cNode* pNode;

				cIterator();
			};

			typedef cIterator iterator;

		private:
			cNode* GetNext();
			cNode* GetNext(std::string sName);
			
			cNode* FirstChild();
			cNode* FindChild(std::string sName);

			cNode* AddNode();
			void AddAttribute(std::string inAttribute, std::string inValue);
			void AddContent(std::string inContent);

#ifdef BUILD_DEBUG
			void PrintToLog(std::string sTab="");
#endif //BUILD_DEBUG


			std::vector<cNode*> vChild;

			cNode* pParent;
			cNode* pNext;

			std::string sName;
			std::map<std::string, std::string> mAttribute; // One of each attribute

			bool bContentOnly;
			std::string sContentOnly;

			std::string ParseFromString(std::string sData, cNode* pPrevious);
			void WriteToFile(std::ofstream& file, std::string sTab);
		};


		// *** Inlines
		
		inline cNode::cIterator::cIterator(const cIterator& rhs) :
			pNode(rhs.pNode)
		{
		}

		inline cNode::cIterator::cIterator(const cNode& rhs) :
			pNode(&const_cast<cNode&>(rhs))
		{
		}

		inline cNode::cIterator& cNode::cIterator::operator=(const cIterator& rhs)
		{
			pNode = rhs.pNode;
			return *this;
		}

		inline cNode::cIterator& cNode::cIterator::operator=(const cNode& rhs)
		{
			pNode = &const_cast<cNode&>(rhs);
			return *this;
		}

		inline cNode::cIterator::operator bool()
		{
			return pNode != NULL;
		}

		inline cNode::cIterator::operator++(int)
		{
			assert(pNode != NULL);
			pNode = pNode->GetNext();
			return *this;
		}

		inline void cNode::cIterator::Next(std::string sName)
		{
			assert(pNode != NULL);
			pNode = pNode->GetNext(sName);
		}

		inline void cNode::cIterator::FirstChild()
		{
			assert(pNode != NULL);
			pNode = pNode->FirstChild();
		}
		
		inline void cNode::cIterator::FindChild(std::string sName)
		{
			assert(pNode != NULL);
			pNode = pNode->FindChild(sName);
		}

		inline std::string cNode::cIterator::GetName()
		{
			assert(pNode != NULL);
			return pNode->GetName();
		}

		template <class T>
		inline bool cNode::cIterator::GetAttribute(std::string sAttribute, T* pValue)
		{
			assert(pNode != NULL);
			return pNode->GetAttribute(sAttribute, pValue);
		}
	}
}

#endif //CXML_H
