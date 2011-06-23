#ifndef JSON_H
#define JSON_H

#include <spitfire/storage/document.h>

// http://en.wikipedia.org/wiki/JSON

namespace spitfire
{
  namespace json
  {
    class cNode;

    typedef cNode cDocument;

    class reader;
    class writer;

    class cNode
    {
    public:
      friend class reader;
      friend class writer;

      cNode();
      ~cNode();

      cNode* CreateNode(); // This element is owned by the caller of this function
      void AppendChild(cNode* pChild); // this cNode takes ownership of the node inside this function, do not delete pChild, cNode will do this for you

      std::string GetName() const { return sNameUTF8; }
      void SetName(const std::string& sName) { sNameUTF8 = sName; }

      bool IsTypeNull() const { return (type == TYPE::NULL_); }
      bool IsTypeObject() const { return (type == TYPE::OBJECT); }
      bool IsTypeArray() const { return (type == TYPE::ARRAY); }
      bool IsTypeString() const { return (type == TYPE::STRING); }
      bool IsTypeInt() const { return (type == TYPE::INT); }
      bool IsTypeFloat() const { return (type == TYPE::FLOAT); }
      bool IsTypeBool() const { return (type == TYPE::BOOL_); }

      const std::vector<cNode*>& GetValueObjectOrArray() const { ASSERT(type == TYPE::OBJECT); return vValueObjectOrArray; }
      std::vector<cNode*>& GetValueObjectOrArray() { ASSERT(type == TYPE::OBJECT); return vValueObjectOrArray; }
      std::string GetValueString() const { ASSERT(type == TYPE::STRING); return sValueUTF8String; }
      int GetValueInt() const { ASSERT(type == TYPE::INT); return iValueInt; }
      double GetValueFloat() const { ASSERT(type == TYPE::FLOAT); return dValueFloat; }
      bool GetValueBool() const { ASSERT(type == TYPE::BOOL_); return bValueBool; }

      void SetTypeNull() { type = TYPE::NULL_; }
      void SetTypeObject() { type = TYPE::OBJECT; }
      void SetTypeArray() { type = TYPE::ARRAY; }
      bool SetTypeString(const std::string& sValue) { type = TYPE::STRING; sValueUTF8String = sValue; }
      void SetTypeInt(int iValue) { type = TYPE::INT; iValueInt = iValue; }
      void SetTypeFloat(double fValue) { type = TYPE::FLOAT; dValueFloat = fValue; }
      void SetTypeBool(bool bValue) { type = TYPE::BOOL_; bValueBool = bValue; }


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
      enum class TYPE {
        NULL_,   // null
        OBJECT,  // Object (an unordered collection of key:value pairs, comma-separated and enclosed in curly braces; the key must be a string)
        ARRAY,   // Array (an ordered sequence of values, comma-separated and enclosed in square brackets. The values don't need to have the same type.)
        STRING,  // String (double-quoted Unicode with backslash escaping)
        INT,
        FLOAT,   // Float (double precision floating-point format)
        BOOL_,   // Boolean (true or false)
      };

      std::string sNameUTF8; // Most nodes will have a name as well as one of the following values
      std::vector<cNode*> vValueObjectOrArray;
      std::string sValueUTF8String;
      int iValueInt;
      double dValueFloat;
      bool bValueBool;
    };


    class reader
    {
    public:
      bool ReadFromFile(cDocument& doc, const string_t& filename) const;
      bool ReadFromStringUTF8(cDocument& doc, const std::string& input) const;

    private:
      bool ReadNodeFromStringParserUTF8(cNode& doc, cStringParserUTF8& sp) const;
      bool ReadObject(cNode& object, cStringParserUTF8& sp) const;
      bool ReadArray(cNode& array, cStringParserUTF8& sp) const;
      bool ReadString(std::string& sValue, cStringParserUTF8& sp) const;
    };


   class writer
    {
    public:
      bool WriteToFile(const cDocument& doc, const string_t& filename) const;
      bool WriteToStringUTF8(const cDocument& doc, std::string& output) const;
    };



    // *** Inlines

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
      return (pNode != nullptr);
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
      return (pNode != nullptr);
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

#endif // JSON_H
