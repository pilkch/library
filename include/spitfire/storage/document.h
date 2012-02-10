#ifndef DOCUMENT_H
#define DOCUMENT_H

// Spitfire headers
#include <spitfire/util/string.h>

#ifdef BUILD_XML_MATH
#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cColour.h>
#endif

// TODO: Move SaveToFile, LoadFromFile, etc. to xml::reader and xml::writer

namespace spitfire
{
  namespace xml
  {
    class reader;
    class writer;
  }

  namespace document
  {
    class cNode;

    typedef cNode cDocument;
    typedef cNode element;
    typedef cNode node;

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
      typedef std::map<std::string, std::string>::const_iterator const_attribute_iterator;

    public:
      friend class xml::reader;
      friend class xml::writer;

      cNode();
      explicit cNode(cNode* inParent);
      explicit cNode(const string_t& inFilename);
      ~cNode();

      bool LoadFromString(const std::string& sData);
      bool LoadFromFile(const string_t& sFilename);
      bool SaveToFile(const string_t& inFilename) const;

#ifdef BUILD_DEBUG
      void PrintToLog(const std::string& sTab="");
#endif // BUILD_DEBUG

      cNode* CreateElement(const std::string& name); // This element is owned by the caller of this function
      cNode* CreateTextNode(const std::string& text); // This element is owned by the caller of this function
      cNode* CreateContentNode(const std::string& content); // This element is owned by the caller of this function
      cNode* CreateCommentNode(const std::string& comment); // This element is owned by the caller of this function

      void AppendChild(element* pChild); // this cNode takes ownership of the node inside this function, do not delete pChild, cNode will do this for you
      void SetAttribute(const std::string& sAttribute, const char* szValue);
      void SetAttribute(const std::string& sAttribute, const wchar_t* szValue);
      void SetAttribute(const std::string& sAttribute, const std::string& sValue);
      void SetAttribute(const std::string& sAttribute, const std::wstring& sValue);
      void SetAttribute(const std::string& sAttribute, const bool value);
      void SetAttribute(const std::string& sAttribute, const uint32_t value);
      void SetAttribute(const std::string& sAttribute, const int32_t value);
      void SetAttribute(const std::string& sAttribute, const uint64_t value);
      void SetAttribute(const std::string& sAttribute, const int64_t value);
      void SetAttribute(const std::string& sAttribute, const float* pValue, size_t nValues);

      #ifdef BUILD_XML_MATH
      void SetAttribute(const std::string& sAttribute, const math::cVec3& value);
      void SetAttribute(const std::string& sAttribute, const math::cQuaternion& value);
      void SetAttribute(const std::string& sAttribute, const math::cColour& value);
      #endif

      bool IsXMLDeclarationOnly() const { return type == TYPE::XML_DELCARATION; }
      bool IsCommentOnly() const { return type == TYPE::COMMENT; }
      bool IsNameAndAttributesAndChildren() const { return type == TYPE::NAME_AND_ATTRIBUTES_AND_CHILDREN; }
      bool IsContentOnly() const { return type == TYPE::CONTENT_ONLY; }

      std::string GetName() const;
      std::string GetContent() const;

      template <class T>
      bool GetAttribute(const std::string& sAttribute, T& value) const
      {
        const_attribute_iterator iter = mAttribute.find(sAttribute);
        if (iter != mAttribute.end()) {
          std::stringstream stm(iter->second);
          stm >> value;
          return true;
        }

        return false;
      }

      bool GetAttribute(const std::string& sAttribute, std::string& value) const;
      bool GetAttribute(const std::string& sAttribute, std::wstring& value) const;
      bool GetAttribute(const std::string& sAttribute, bool& value) const;
      bool GetAttribute(const std::string& sAttribute, uint32_t& value) const;
      bool GetAttribute(const std::string& sAttribute, int32_t& value) const;
      bool GetAttribute(const std::string& sAttribute, uint64_t& value) const;
      bool GetAttribute(const std::string& sAttribute, int64_t& value) const;
      bool GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues) const;

      #ifdef BUILD_XML_MATH
      bool GetAttribute(const std::string& sAttribute, math::cVec3& value) const;
      bool GetAttribute(const std::string& sAttribute, math::cQuaternion& value) const;
      bool GetAttribute(const std::string& sAttribute, math::cColour& value) const;
      #endif

      void Clear();          // Removes all attributes including name and removes all children
      void RemoveChildren(); // Removes all children

      class cConstIterator
      {
      public:
        cConstIterator(const cConstIterator& rhs);
        explicit cConstIterator(const cNode& rhs);

        cConstIterator operator=(const cConstIterator& rhs);
        cConstIterator operator=(const cNode& rhs);

        bool IsValid() const;

        cConstIterator operator++(int);
        void Next();
        void Next(const std::string& sName);

        void FirstChild();
        void FindChild(const std::string& sName);

        cConstIterator GetFirstChild() const;
        cConstIterator GetChild(const std::string& sName) const;

        std::string GetName() const; // Get the name of this node
        std::string GetContent() const; // Get the content of this node
        std::string GetChildContent() const; // Get the content of the (only) child of this node

        template <class T>
        bool GetAttribute(const std::string& sAttribute, T& value) const;

        bool GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues) const;

      private:
        // Forbidden
        cConstIterator();
        operator bool() const;

        const cNode* pNode;
      };

      class cIterator
      {
      public:
        cIterator(const cIterator& rhs);
        explicit cIterator(cNode& rhs);

        cIterator operator=(const cIterator& rhs);
        cIterator operator=(cNode& rhs);

        bool IsValid() const;

        cIterator operator++(int);
        void Next();
        void Next(const std::string& sName);

        void FirstChild();
        void FindChild(const std::string& sName);

        cIterator GetFirstChild() const;
        cIterator GetChild(const std::string& sName) const;

        std::string GetName() const; // Get the name of this node
        std::string GetContent() const; // Get the content of this node
        std::string GetChildContent() const; // Get the content of the (only) child of this node

        template <class T>
        bool GetAttribute(const std::string& sAttribute, T& value) const;

        bool GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues) const;

        // This allows us to edit the node, you should consider the iterator invalid after editing the node
        cNode* Get() { return pNode; }

      private:
        // Forbidden
        cIterator();
        operator bool() const;

        cNode* pNode;
      };

      typedef cConstIterator const_iterator;
      typedef cIterator iterator;

    private:
      cNode* CreateNode();
      cNode* CreateNodeAsChildAndAppend();

      const cNode* GetNext() const;
      const cNode* GetNext(const std::string& sName) const;
      cNode* GetNext();
      cNode* GetNext(const std::string& sName);

      const cNode* FirstChild() const;
      const cNode* FindChild(const std::string& sName) const;
      cNode* FirstChild();
      cNode* FindChild(const std::string& sName);

      void SetTypeElement(const std::string& name);
      void SetTypeContentOnly(const std::string& text);

      std::string ParseFromString(const std::string& sData, cNode* pPrevious);

      void WriteToFile(std::ofstream& file, const std::string& sTab) const;

      enum class TYPE {
        XML_DELCARATION,
        COMMENT,
        NAME_AND_ATTRIBUTES_AND_CHILDREN,
        CONTENT_ONLY
      };


      TYPE type;

      cNode* pParent;
      cNode* pNext;

      std::vector<cNode*> vChild;

      std::string sName;
      std::map<std::string, std::string> mAttribute; // One of each attribute

      std::string sContentOnly;
    };


    // *** Inlines

    // ** cNode

    inline cNode* cNode::CreateElement(const std::string& name)
    {
      cNode* pNode = new cNode;
      pNode->SetTypeElement(name);
      return pNode;
    }

    inline cNode* cNode::CreateTextNode(const std::string& text)
    {
      cNode* pNode = new cNode;
      pNode->SetTypeContentOnly(text);
      return pNode;
    }

    inline cNode* cNode::CreateContentNode(const std::string& content)
    {
      // This is a synonym for CreateTextNode, perhaps we should rename CreateTextNode?
      return CreateTextNode(content);
    }


    // ** cConstIterator

    inline cNode::cConstIterator::cConstIterator(const cConstIterator& rhs) :
      pNode(rhs.pNode)
    {
    }

    inline cNode::cConstIterator::cConstIterator(const cNode& rhs) :
      pNode(&rhs)
    {
    }

    inline cNode::cConstIterator cNode::cConstIterator::operator=(const cConstIterator& rhs)
    {
      pNode = rhs.pNode;
      return *this;
    }

    inline cNode::cConstIterator cNode::cConstIterator::operator=(const cNode& rhs)
    {
      pNode = &rhs;
      return *this;
    }

    inline bool cNode::cConstIterator::IsValid() const
    {
      return pNode != nullptr;
    }

    inline cNode::cConstIterator cNode::cConstIterator::operator++(int)
    {
      ASSERT(pNode != nullptr);
      pNode = pNode->GetNext();
      return *this;
    }

    inline void cNode::cConstIterator::Next()
    {
      ASSERT(pNode != nullptr);
      pNode = pNode->GetNext();
    }

    inline void cNode::cConstIterator::Next(const std::string& sName)
    {
      ASSERT(pNode != nullptr);
      pNode = pNode->GetNext(sName);
    }

    inline void cNode::cConstIterator::FirstChild()
    {
      ASSERT(pNode != nullptr);
      pNode = pNode->FirstChild();
    }

    inline void cNode::cConstIterator::FindChild(const std::string& sName)
    {
      ASSERT(pNode != nullptr);
      pNode = pNode->FindChild(sName);
    }

    inline cNode::cConstIterator cNode::cConstIterator::GetFirstChild() const
    {
      cConstIterator iter(*this);
      iter.FirstChild();

      return iter;
    }

    inline cNode::cConstIterator cNode::cConstIterator::GetChild(const std::string& sName) const
    {
      cConstIterator iter(*this);
      iter.FindChild(sName);

      return iter;
    }

    inline std::string cNode::cConstIterator::GetName() const
    {
      ASSERT(pNode != nullptr);
      return pNode->GetName();
    }

    inline std::string cNode::cConstIterator::GetContent() const
    {
      ASSERT(pNode != nullptr);
      return pNode->GetContent();
    }

    inline std::string cNode::cConstIterator::GetChildContent() const
    {
      ASSERT(pNode != nullptr);
      cConstIterator iter(*this);
      iter.FirstChild();
      if (!iter.IsValid()) return "";

      // Ok, we have a child, let's get it's content
      return iter.GetContent();
    }

    template <class T>
    inline bool cNode::cConstIterator::GetAttribute(const std::string& sAttribute, T& value) const
    {
      ASSERT(pNode != nullptr);
      return pNode->GetAttribute(sAttribute, value);
    }

    inline bool cNode::cConstIterator::GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues) const
    {
      ASSERT(pNode != nullptr);
      ASSERT(pValue != nullptr);
      return pNode->GetAttribute(sAttribute, pValue, nValues);
    }


    // ** cIterator

    inline cNode::cIterator::cIterator(const cIterator& rhs) :
      pNode(rhs.pNode)
    {
    }

    inline cNode::cIterator::cIterator(cNode& rhs) :
      pNode(&rhs)
    {
    }

    inline cNode::cIterator cNode::cIterator::operator=(const cIterator& rhs)
    {
      pNode = rhs.pNode;
      return *this;
    }

    inline cNode::cIterator cNode::cIterator::operator=(cNode& rhs)
    {
      pNode = &rhs;
      return *this;
    }

    inline bool cNode::cIterator::IsValid() const
    {
      return pNode != nullptr;
    }

    inline cNode::cIterator cNode::cIterator::operator++(int)
    {
      ASSERT(pNode != nullptr);
      pNode = pNode->GetNext();
      return *this;
    }

    inline void cNode::cIterator::Next()
    {
      ASSERT(pNode != nullptr);
      pNode = pNode->GetNext();
    }

    inline void cNode::cIterator::Next(const std::string& sName)
    {
      ASSERT(pNode != nullptr);
      pNode = pNode->GetNext(sName);
    }

    inline void cNode::cIterator::FirstChild()
    {
      ASSERT(pNode != nullptr);
      pNode = pNode->FirstChild();
    }

    inline void cNode::cIterator::FindChild(const std::string& sName)
    {
      ASSERT(pNode != nullptr);
      pNode = pNode->FindChild(sName);
    }

    inline cNode::cIterator cNode::cIterator::GetFirstChild() const
    {
      cIterator iter(*this);
      iter.FirstChild();

      return iter;
    }

    inline cNode::cIterator cNode::cIterator::GetChild(const std::string& sName) const
    {
      cIterator iter(*this);
      iter.FindChild(sName);

      return iter;
    }

    inline std::string cNode::cIterator::GetName() const
    {
      ASSERT(pNode != nullptr);
      return pNode->GetName();
    }

    inline std::string cNode::cIterator::GetContent() const
    {
      ASSERT(pNode != nullptr);
      return pNode->GetContent();
    }

    inline std::string cNode::cIterator::GetChildContent() const
    {
      ASSERT(pNode != nullptr);
      cIterator iter(*this);
      iter.FirstChild();
      if (!iter.IsValid()) return "";

      // Ok, we have a child, let's get it's content
      return iter.GetContent();
    }

    template <class T>
    inline bool cNode::cIterator::GetAttribute(const std::string& sAttribute, T& value) const
    {
      ASSERT(pNode != nullptr);
      return pNode->GetAttribute(sAttribute, value);
    }

    inline bool cNode::cIterator::GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues) const
    {
      ASSERT(pNode != nullptr);
      ASSERT(pValue != nullptr);
      return pNode->GetAttribute(sAttribute, pValue, nValues);
    }
  }
}

#endif // DOCUMENT_H
