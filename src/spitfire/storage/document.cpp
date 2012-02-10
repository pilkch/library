#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>

// Writing to and from text files
#include <iostream>
#include <fstream>

#include <list>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stack>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>

#ifndef FIRESTARTER
#include <spitfire/util/log.h>
#endif

#include <spitfire/storage/document.h>

#ifdef BUILD_XML_MATH
#include <spitfire/math/math.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cColour.h>
#endif

namespace spitfire
{
  namespace document
  {
    //cNode

    cNode::cNode() :
      type(TYPE::NAME_AND_ATTRIBUTES_AND_CHILDREN),
      pParent(nullptr),
      pNext(nullptr)
    {
    }

    cNode::cNode(cNode* inParent) :
      type(TYPE::NAME_AND_ATTRIBUTES_AND_CHILDREN),
      pParent(inParent),
      pNext(nullptr)
    {
    }

    cNode::cNode(const string_t& inFilename) :
      type(TYPE::NAME_AND_ATTRIBUTES_AND_CHILDREN),
      pParent(nullptr),
      pNext(nullptr)
    {
      LoadFromFile(inFilename);
    }

    cNode::~cNode()
    {
      Clear();
    }

    void cNode::Clear()
    {
      type = TYPE::NAME_AND_ATTRIBUTES_AND_CHILDREN;

      pParent = nullptr;
      pNext = nullptr;

      RemoveChildren();

      sName.clear();
      mAttribute.clear();

      sContentOnly.clear();
    }

    void cNode::RemoveChildren()
    {
      const size_t n = vChild.size();
      for (size_t i = 0; i < n; i++) SAFE_DELETE(vChild[i]);
      vChild.clear();
    }

    bool cNode::LoadFromString(const std::string& sData)
    {
      ParseFromString(sData, nullptr);

      return (!sData.empty());
    }

    bool cNode::LoadFromFile(const string_t& inFilename)
    {
      LOG<<"cNode::LoadFromFile \""<<inFilename<<"\""<<std::endl;
      std::ifstream f(spitfire::string::ToUTF8(inFilename).c_str());

      if (!f.is_open()) {
        CONSOLE<<"XML "<<inFilename<<" not found, returning false"<<std::endl;
        return false;
      }

      std::string sData;
      std::string line;
      while(!f.eof()) {
        std::getline(f, line);

        // Get rid of leading tabs
        sData += string::StripLeadingWhiteSpace(line);
      }
      f.close();

      LOG<<"XML "<<inFilename<<" contains \""<<spitfire::string::ToString_t(sData)<<"\", returning"<<std::endl;
      return LoadFromString(sData);
    }

    std::string cNode::ParseFromString(const std::string& data, cNode* pPrevious)
    {
      std::string sData(data);
      while (!sData.empty()) {
        sData = string::StripLeadingWhiteSpace(sData);

        // XML declaration
        // <?xml version="1.0" encoding="UTF-8"?>
        while (sData.find("<?xml") == 0) {
          const size_t nEndDeclaration = sData.find("?>");

          if (nEndDeclaration == std::string::npos) {
#ifndef FIRESTARTER
            LOG<<"XML Unterminated XML declaration"<<std::endl;
#endif
            return "";
          }

          LOG<<"Found XML declaration node"<<std::endl;
          type = TYPE::XML_DELCARATION;
          sData = string::StripLeadingWhiteSpace(sData.substr(nEndDeclaration));
        }

        // Comments
        // <!-- ... -->
        while (sData.find("<!--") == 0) {
          const size_t nEndComment = sData.find("-->");

          if (nEndComment == std::string::npos) {
            LOG<<"XML Unterminated Comment"<<std::endl;
            return "";
          }

          LOG<<"Found comment node"<<std::endl;
          type = TYPE::COMMENT;
          sData = string::StripLeadingWhiteSpace(sData.substr(nEndComment));
        }

        std::string::size_type angleBracket=sData.find("<");
        if (angleBracket != std::string::npos) {
          std::string sContent;

          if (angleBracket) {
            //Content</name>
            sContent = sData.substr(0, angleBracket);
          }

          // example attribute="value"...
          sData = string::StripLeading(sData.substr(angleBracket+1), " ");

          if (sData[0] == '/') {
            // </name>
            if (!sContent.empty() && sContent != "/>") {
              cNode* pNode = CreateNodeAsChildAndAppend();
              ASSERT(pNode != nullptr);
              pNode->SetTypeContentOnly(sContent);
            }

            angleBracket = sData.find(">");

            if (angleBracket != std::string::npos) {
              std::string sClose=string::StripTrailing(string::StripLeading(sData.substr(1, angleBracket-1), " "), " ");

              if (sClose != sName) {
                LOG<<"XML Opening tag \""<<spitfire::string::ToString_t(sName)<<"\" doesn't match closing tag \""<<spitfire::string::ToString_t(sClose)<<"\""<<std::endl;
                return "";
              }

              sData=string::StripLeading(sData.substr(angleBracket+1), " ");
            } else {
              LOG<<"XML Tag \""<<spitfire::string::ToString_t(sName)<<"\" doesn't have a closing tag"<<std::endl;
              return "";
            }

            return sData;
          }

          const std::string sSlashSpaceRightBracket = "/ >";
          std::string::size_type n = sData.find_first_of(sSlashSpaceRightBracket);

          if (std::string::npos != n) {
            // Collect name
            std::string inName=string::StripTrailing(sData.substr(0, n), " ");

            // attribute="value"...
            sData = string::StripLeading(sData.substr(n), " ");

            cNode* p = CreateNodeAsChildAndAppend();

            LOG<<"Found node "<<spitfire::string::ToString_t(inName)<<std::endl;
            p->sName = inName;

            // Fill in attributes if any, and find the end of the opening tag
            if ('/' == sData[0]) {
              // />
              n=sData.find(">");
              if (std::string::npos==n) return "";

              sData=sData.substr(n+1);
            } else {
              // attribute="value">
              std::string::iterator iter=sData.begin();

              std::string sAttributeName;
              std::string sAttributeValue;

              while(iter!=sData.end() && *iter!='/' && *iter!='>') {
                if (*iter == ' ') {
                  p->SetAttribute(sAttributeName, "");
                  sAttributeName = "";
                  sData=string::StripLeading(&*iter, " ");
                  iter=sData.begin();
                  continue;
                } else if (*iter == '=') {
                  iter++;
                  if ((iter != sData.end()) && (*iter == '\"')) {
                    sData.erase(sData.begin(), ++iter);
                    sData=string::StripLeading(sData, " ");

                    std::string::size_type nQuote=sData.find("\"");
                    std::string::size_type nSlashQuote=sData.find("\\\"");

                    while (std::string::npos != nQuote) {
                      if (nQuote < nSlashQuote) {
                        // attribute="value"
                        p->SetAttribute(sAttributeName, sAttributeValue + sData.substr(0, nQuote));
                        sAttributeName="";
                        sAttributeValue="";
                        sData=string::StripLeading(sData.substr(nQuote+1), " ");
                        break;
                      }
                      else
                      {
                        // attribute="value\"innervalue..."
                        sAttributeValue+=sData.substr(0, nSlashQuote);
                        sData=string::StripLeading(sData.substr(nSlashQuote+2), " ");
                      }

                      nQuote=sData.find("\"");
                      nSlashQuote=sData.find("\\\"");
                    };

                    iter=sData.begin();
                    continue;
                  }
                } else sAttributeName+=*iter;

                iter++;
              };

              if ('>'==*iter)
              {
                // >...
                iter++;
                if (!sAttributeName.empty()) {
                  // attribute>
                  p->SetAttribute(sAttributeName, sAttributeValue);
                  sAttributeName = "";
                }
                sData.erase(sData.begin(), iter);
                sData=(*vChild.rbegin())->ParseFromString(sData, this);
                iter = sData.begin();
              }
            }
          }
          else
          {
            return "";
          }
        }
        else
        {
          // No tags, just content for the parent tag (this)
          cNode* p = CreateNodeAsChildAndAppend();

          p->type = TYPE::CONTENT_ONLY;
          p->sContentOnly += string::StripTrailingWhiteSpace(sData);
          return "";
        }
      }

      return sData;
    }


    bool cNode::SaveToFile(const string_t& sFilename) const
    {
      std::ofstream f(spitfire::string::ToUTF8(sFilename).c_str());

      if (f.is_open()) {
        // Write the header
        f<<"<?xml version=\"1.0\"?>"<<std::endl;

        const size_t n = vChild.size();
        for (size_t i = 0;i < n; i++) vChild[i]->WriteToFile(f, "");

        f.close();

        return true;
      }

      LOG<<"XML Error could not open \""<<spitfire::string::ToString_t(sFilename)<<"\", returning false"<<std::endl;
      return false;
    }

    void cNode::WriteToFile(std::ofstream& f, const std::string& sTab) const
    {
      ASSERT(f.is_open());

      if (IsNameAndAttributesAndChildren()) {
        if (!sName.empty()) {
          std::string sTag = sTab + "<" + sName;
          const_attribute_iterator iter = mAttribute.begin();
          const const_attribute_iterator iterEnd = mAttribute.end();
          while (iter != iterEnd) {
            if (!iter->second.empty()) sTag += " " + iter->first + "=\"" + iter->second + "\"";
            else sTag += " " + iter->first;

            iter++;
          }

          if (vChild.size() == 0) {
            f<<sTag<<"/>"<<std::endl;
            return;
          }

          f<<sTag<<">";

          if (!vChild[0]->IsContentOnly()) f<<std::endl;
        }
      }
      else
        f<<sContentOnly;

      const size_t n = vChild.size();
      for (size_t i = 0; i < n; i++) vChild[i]->WriteToFile(f, sTab + "  ");

      if (!sName.empty()) {
        if (!vChild.empty() || (!vChild.empty() && !vChild[0]->IsContentOnly()))
          f<<sTab<<"</"<<sName<<">"<<std::endl;
        else
          f<<"</"<<sName<<">"<<std::endl;
      }
    }

#ifdef BUILD_DEBUG
    void cNode::PrintToLog(const std::string& sTab)
    {
      size_t i, n;
      if (IsNameAndAttributesAndChildren())
      {
        if (!sName.empty())
        {
          std::string sTag = sTab + "&lt;" + sName;
          attribute_iterator iter=mAttribute.begin();
          for (;iter!=mAttribute.end();iter++)
            sTag += " " + iter->first + "=\"" + iter->second + "\"";

          if (!vChild.empty()) sTag+="&gt;";
          else sTag+="/&gt;";

          LOG<<"XML "<<spitfire::string::ToString_t(sTag)<<std::endl;
        }
      } else LOG<<"XML "<<spitfire::string::ToString_t(sTab)<<"Content=\""<<spitfire::string::ToString_t(sContentOnly)<<"\""<<std::endl;

      n = vChild.size();
      for (i=0;i<n;i++) vChild[i]->PrintToLog(sTab + "&nbsp;");

      if (!vChild.empty() && !sName.empty()) LOG<<"XML "<<spitfire::string::ToString_t(sTab)<<"&lt;/"<<spitfire::string::ToString_t(sName)<<"&gt;"<<std::endl;
    }
#endif // BUILD_DEBUG

    const cNode* cNode::FirstChild() const
    {
      if (!vChild.empty()) return vChild[0];

      return nullptr;
    }

    cNode* cNode::FirstChild()
    {
      if (!vChild.empty()) return vChild[0];

      return nullptr;
    }

    const cNode* cNode::FindChild(const std::string& sName) const
    {
      if (!vChild.empty()) {
        const cNode* p = vChild[0];
        while (p != nullptr) {
          if (sName == p->sName) return p;
          p = p->pNext;
        }
      }

      return nullptr;
    }

    cNode* cNode::FindChild(const std::string& sName)
    {
      if (!vChild.empty()) {
        cNode* p = vChild[0];
        while (p != nullptr) {
          if (sName == p->sName) return p;
          p = p->pNext;
        }
      }

      return nullptr;
    }

    const cNode* cNode::GetNext() const
    {
      return pNext;
    }

    cNode* cNode::GetNext()
    {
      return pNext;
    }

    const cNode* cNode::GetNext(const std::string& sName) const
    {
      const cNode* p = pNext;
      while (p != nullptr) {
        if (sName == p->sName) return p;
        p = p->pNext;
      };

      return nullptr;
    }

    cNode* cNode::GetNext(const std::string& sName)
    {
      cNode* p = pNext;
      while (p != nullptr) {
        if (sName == p->sName) return p;
        p = p->pNext;
      };

      return nullptr;
    }

    cNode* cNode::CreateNode()
    {
      cNode* pNode = new cNode;
      return pNode;
    }

    cNode* cNode::CreateNodeAsChildAndAppend()
    {
      cNode* pChild = new cNode(this);
      AppendChild(pChild);
      return pChild;
    }

    void cNode::AppendChild(element* pChild)
    {
      ASSERT(pChild != nullptr);

      if (!vChild.empty()) {
        element* pLast = vChild.back();
        ASSERT(pLast != nullptr);
        pLast->pNext = pChild;
      }

      vChild.push_back(pChild);
    }

    void cNode::SetTypeElement(const std::string& inName)
    {
      sName = string::StripTrailingWhiteSpace(inName);
      type = TYPE::NAME_AND_ATTRIBUTES_AND_CHILDREN;
    }

    void cNode::SetTypeContentOnly(const std::string& inContent)
    {
      sContentOnly = string::StripTrailingWhiteSpace(inContent);
      type = TYPE::CONTENT_ONLY;
    }

    std::string cNode::GetName() const
    {
      return sName;
    }

    std::string cNode::GetContent() const
    {
      return sContentOnly;
    }


    void cNode::SetAttribute(const std::string& sAttribute, const char* value)
    {
      mAttribute[sAttribute] = value;
    }

    void cNode::SetAttribute(const std::string& sAttribute, const wchar_t* value)
    {
      mAttribute[sAttribute] = spitfire::string::ToUTF8(value);
    }

    void cNode::SetAttribute(const std::string& sAttribute, const std::string& value)
    {
      mAttribute[sAttribute] = value;
    }

    void cNode::SetAttribute(const std::string& sAttribute, const std::wstring& value)
    {
      mAttribute[sAttribute] = spitfire::string::ToUTF8(value);
    }

    void cNode::SetAttribute(const std::string& sAttribute, const bool value)
    {
      mAttribute[sAttribute] = spitfire::string::ToUTF8(spitfire::string::ToString(value));
    }

    void cNode::SetAttribute(const std::string& sAttribute, uint32_t value)
    {
      mAttribute[sAttribute] = spitfire::string::ToUTF8(spitfire::string::ToString(value));
    }

    void cNode::SetAttribute(const std::string& sAttribute, int32_t value)
    {
      mAttribute[sAttribute] = spitfire::string::ToUTF8(spitfire::string::ToString(value));
    }

    void cNode::SetAttribute(const std::string& sAttribute, uint64_t value)
    {
      mAttribute[sAttribute] = spitfire::string::ToUTF8(spitfire::string::ToString(value));
    }

    void cNode::SetAttribute(const std::string& sAttribute, int64_t value)
    {
      mAttribute[sAttribute] = spitfire::string::ToUTF8(spitfire::string::ToString(value));
    }

    void cNode::SetAttribute(const std::string& sAttribute, const float* pValue, size_t nValues)
    {
      ASSERT(pValue != nullptr);

      std::ostringstream o;

      size_t i = 0;
      for (; i < nValues; i++) {
        o<<pValue[i];
      };

      for (; i < nValues; i++) {
        o<<", ";
        o<<pValue[i];
      };

      mAttribute[sAttribute] = o.str();
    }

    #ifdef BUILD_XML_MATH
    void cNode::SetAttribute(const std::string& sAttribute, const math::cVec3& value)
    {
      std::ostringstream o;

      o<<value.x;
      o<<", ";
      o<<value.y;
      o<<", ";
      o<<value.z;

      mAttribute[sAttribute] = o.str();
    }

    void cNode::SetAttribute(const std::string& sAttribute, const math::cQuaternion& value)
    {
      std::ostringstream o;

      o<<value.x;
      o<<", ";
      o<<value.y;
      o<<", ";
      o<<value.z;
      o<<", ";
      o<<value.w;

      mAttribute[sAttribute] = o.str();
    }

    void cNode::SetAttribute(const std::string& sAttribute, const math::cColour& value)
    {
      std::ostringstream o;

      o<<value.r;
      o<<", ";
      o<<value.g;
      o<<", ";
      o<<value.b;
      o<<", ";
      o<<value.a;

      mAttribute[sAttribute] = o.str();
    }
    #endif // BUILD_XML_MATH


    bool cNode::GetAttribute(const std::string& sAttribute, std::string& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      const_attribute_iterator iterEnd = mAttribute.end();
      if (iter != iterEnd) {
        value = iter->second;
        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, std::wstring& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      const_attribute_iterator iterEnd = mAttribute.end();
      if (iter != iterEnd) {
        value = spitfire::string::ToWchar_t(iter->second);
        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, bool& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        value = spitfire::string::ToBool(spitfire::string::ToString_t(iter->second));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, uint32_t& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        value = spitfire::string::ToUnsignedInt(spitfire::string::ToString_t(iter->second));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, int32_t& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        value = spitfire::string::ToInt(spitfire::string::ToString_t(iter->second));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, uint64_t& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        value = spitfire::string::ToUnsignedInt(spitfire::string::ToString_t(iter->second));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, int64_t& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        value = spitfire::string::ToInt(spitfire::string::ToString_t(iter->second));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues) const
    {
      ASSERT(pValue != nullptr);
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        char c;
        std::stringstream stm(iter->second);
        stm >> std::skipws;

        size_t i = 0;
        while (i != nValues) {
          stm >> pValue[i++];
          stm >> c;
        };

        return true;
      }

      return false;
    }

    #ifdef BUILD_XML_MATH
    bool cNode::GetAttribute(const std::string& sAttribute, math::cVec3& value) const
    {
      value.SetZero();

      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        std::vector<std::string> vSplit;
        spitfire::string::Split(iter->second, ',', vSplit);

        const size_t n = vSplit.size();
        if (n > 0) value.x = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[0]));
        if (n > 1) value.y = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[1]));
        if (n > 2) value.z = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[2]));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, math::cQuaternion& value) const
    {
      value.LoadIdentity();

      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        std::vector<std::string> vSplit;
        spitfire::string::Split(iter->second, ',', vSplit);

        const size_t n = vSplit.size();
        if (n > 0) value.x = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[0]));
        if (n > 1) value.y = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[1]));
        if (n > 2) value.z = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[2]));
        if (n > 3) value.w = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[3]));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, math::cColour& value) const
    {
      value.Clear();

      value.a = 1.0f;

      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        std::vector<std::string> vSplit;
        spitfire::string::Split(iter->second, ',', vSplit);

        const size_t n = vSplit.size();
        if (n > 0) value.r = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[0]));
        if (n > 1) value.g = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[1]));
        if (n > 2) value.b = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[2]));
        if (n > 3) value.a = spitfire::string::ToFloat(spitfire::string::ToString_t(vSplit[3]));

        return true;
      }

      return false;
    }
    #endif // BUILD_XML_MATH
  }
}
