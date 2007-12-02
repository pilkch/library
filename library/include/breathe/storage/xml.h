#ifndef XML_H
#define XML_H

#ifndef FIRESTARTER
#include <breathe/math/math.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cColour.h>
#endif

/*
TODO:
Identify and replace these in XML:
&amp;	&
&lt;	<
&gt;	>
&quot;	"
&apos;	'
change comments <!-- --> to comment object (bComment = true). 
*/

namespace breathe
{
	namespace xml
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
			cNode(const std::string& inFilename);
			cNode(cNode *inParent);
			~cNode();

			void SaveToFile(const std::string& inFilename);

#ifdef BUILD_DEBUG
			void PrintToLog(const std::string& sTab="");
#endif //BUILD_DEBUG

			bool IsNameAndAttributesAndChildren() const { return !bContentOnly; }
			bool IsContentOnly() const { return bContentOnly; }

			std::string GetName() const;

			template <class T>
			bool GetAttribute(const std::string& sAttribute, T& value)
			{
				attribute_iterator iter = mAttribute.find(sAttribute);
				if (iter != mAttribute.end())
				{
					std::stringstream stm(iter->second);
					stm >> value;
					return true;
				}

				return false;
			}

			bool GetAttribute(const std::string& sAttribute, std::string& pValue);
			bool GetAttribute(const std::string& sAttribute, std::wstring& pValue);
			bool GetAttribute(const std::string& sAttribute, bool& pValue);
			bool GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues);
			
#ifndef FIRESTARTER
			bool GetAttribute(const std::string& sAttribute, math::cVec3& pValue);
			bool GetAttribute(const std::string& sAttribute, math::cColour& pValue);
#endif

			class cIterator
			{
			public:
				cIterator(const cIterator& rhs);
				cIterator(const cNode& rhs);
				cIterator& operator=(const cIterator& rhs);
				cIterator& operator=(const cNode& rhs);

				operator bool() const;

				operator++(int);
				void Next(const std::string& sName);
				
				void FirstChild();
				void FindChild(const std::string& sName);

				std::string GetName() const;

				template <class T>
				bool GetAttribute(const std::string& sAttribute, T& value);
				
				bool GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues);

			private:
				cNode* pNode;

				cIterator();
			};

			typedef cIterator iterator;

		private:
			cNode* GetNext();
			cNode* GetNext(const std::string& sName);
			
			cNode* FirstChild();
			cNode* FindChild(const std::string& sName);

			cNode* AddNode();
			void AddAttribute(const std::string& inAttribute, const std::string& inValue);
			void AddContent(const std::string& inContent);


			std::vector<cNode*> vChild;

			cNode* pParent;
			cNode* pNext;

			std::string sName;
			std::map<std::string, std::string> mAttribute; // One of each attribute

			bool bContentOnly;
			std::string sContentOnly;

			std::string ParseFromString(const std::string& sData, cNode* pPrevious);
			
			void LoadFromFile(const std::string& sFilename);
			void WriteToFile(std::ofstream& file, const std::string& sTab);
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

		inline cNode::cIterator::operator bool() const
		{
			return pNode != nullptr;
		}

		inline cNode::cIterator::operator++(int)
		{
			assert(pNode != nullptr);
			pNode = pNode->GetNext();
			return *this;
		}

		inline void cNode::cIterator::Next(const std::string& sName)
		{
			assert(pNode != nullptr);
			pNode = pNode->GetNext(sName);
		}

		inline void cNode::cIterator::FirstChild()
		{
			assert(pNode != nullptr);
			pNode = pNode->FirstChild();
		}
		
		inline void cNode::cIterator::FindChild(const std::string& sName)
		{
			assert(pNode != nullptr);
			pNode = pNode->FindChild(sName);
		}

		inline std::string cNode::cIterator::GetName() const
		{
			assert(pNode != nullptr);
			return pNode->GetName();
		}

		template <class T>
		inline bool cNode::cIterator::GetAttribute(const std::string& sAttribute, T& value)
		{
			assert(pNode != nullptr);
			return pNode->GetAttribute(sAttribute, value);
		}
		
		inline bool cNode::cIterator::GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues)
		{
			assert(pNode != nullptr);
			assert(pValue != nullptr);
			return pNode->GetAttribute(sAttribute, pValue, nValues);
		}
	}
}

#endif //XML_H
