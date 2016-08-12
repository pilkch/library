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

#include <spitfire/storage/filesystem.h>
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

    util::PROCESS_RESULT cNode::LoadFromString(util::cProcessInterface& interface, const std::string& sData)
    {
      size_t nProcessedElements = 0;
      spitfire::string::cStringParserUTF8 sp(sData);
      return ParseFromStringParser(interface, nProcessedElements, sp);
    }

    util::PROCESS_RESULT cNode::LoadFromFile(util::cProcessInterface& interface, const string_t& inFilename)
    {
      LOG("\"", inFilename, "\"");
      std::ifstream f(spitfire::string::ToUTF8(inFilename).c_str());

      if (!f.is_open()) {
        CONSOLE<<"XML "<<inFilename<<" not found, returning false"<<std::endl;
        return util::PROCESS_RESULT::FAILED;
      }

      const size_t nFileSizeBytes = filesystem::GetFileSizeBytes(inFilename);
      std::string sData;
      sData.reserve(nFileSizeBytes);
      std::string line;
      while(!f.eof()) {
        std::getline(f, line);

        // Get rid of leading tabs
        sData += string::StripLeadingWhiteSpace(line);
      }
      f.close();

      //LOG(inFilename, " contains \"", spitfire::string::ToString(sData), "\", returning");
      return LoadFromString(interface, sData);
    }

    util::PROCESS_RESULT cNode::ParseFromStringParser(const util::cProcessInterface& interface, size_t& nProcessedElements, spitfire::string::cStringParserUTF8& sp)
    {
      //LOG("\"", sp.GetToEnd(), "\"");

      while (!sp.IsEnd()) {
        if (nProcessedElements > 100) {
          if (interface.IsToStop()) return util::PROCESS_RESULT::STOPPED_BY_INTERFACE;
          nProcessedElements = 0;
        }

        nProcessedElements++;

        //const std::string sTmp = sp.GetToEnd();
        //LOG("While \"", sTmp, "\"");

        sp.SkipWhiteSpace();

        // XML declaration
        // <?xml version="1.0" encoding="UTF-8"?>
        while (sp.StartsWithAndSkip("<?xml")) {
          if (!sp.SkipToString("?>")) {
            LOG("Unterminated XML declaration, returning false");
            return util::PROCESS_RESULT::FAILED;
          }

          //LOG("Found XML declaration node");
          type = TYPE::XML_DECLARATION;
          sp.SkipWhiteSpace();
        }

        // Comments
        // <!-- ... -->
        while (sp.StartsWithAndSkip("<!--")) {
          if (!sp.SkipToString("-->")) {
            LOG("Unterminated Comment, returning false");
            return util::PROCESS_RESULT::FAILED;
          }

          //LOG("Found comment node");
          type = TYPE::COMMENT;
        }

        std::string sContent;
        if (sp.GetToStringAndSkip("<", sContent)) {
          sp.SkipWhiteSpace();

          if (sp.GetCharacter() == '/') {
            sp.SkipCharacter();

            // </name>
            if (!sContent.empty() && (sContent != "/>")) {
              cNode* pNode = CreateNodeAsChildAndAppend();
              ASSERT(pNode != nullptr);
              pNode->SetTypeContentOnly(sContent);
            }

            std::string sClose;
            if (sp.GetToStringAndSkip(">", sClose)) {
              if (sClose != sName) {
                LOG("Opening tag \"", spitfire::string::ToString(sName), "\" doesn't match closing tag \"", spitfire::string::ToString(sClose), "\", returning false");
                return util::PROCESS_RESULT::FAILED;
              }
            } else {
              LOG("Tag \"", spitfire::string::ToString(sName), "\" doesn't have a closing tag, returning false");
              return util::PROCESS_RESULT::FAILED;
            }

            //LOG("Found end of tag, breaking");
            break;
          }

          std::string sResult;
          if (sp.GetToOneOfTheseCharacters("/ >", sResult)) {
            sp.SkipCharacters(sResult.length());

            // Collect name
            std::string inName = string::StripTrailing(sResult, " ");

            // attribute="value"...
            sp.SkipWhiteSpace();

            cNode* p = CreateNodeAsChildAndAppend();

            //LOG("Found node ", spitfire::string::ToString(inName));
            p->sName = inName;

            // Fill in attributes if any, and find the end of the opening tag
            if (sp.GetCharacter() == '/') {
              // />
              if (!sp.SkipToStringAndSkip(">")) {
                LOG("Tag \"", spitfire::string::ToString(inName), "\" doesn't have a closing bracket, returning false");
                return util::PROCESS_RESULT::FAILED;
              }
            } else {
              std::string sData;
              if (!sp.GetToStringAndSkip(">", sData)) {
                LOG("Tag \"", spitfire::string::ToString(inName), "\" opening declaration doesn't terminate, returning false");
                return util::PROCESS_RESULT::FAILED;
              }

              // Append a closing bracket because we took one off when parsing
              sData += ">";

              // attribute="value">
              std::string::iterator iter = sData.begin();

              std::string sAttributeName;
              std::string sAttributeValue;

              while ((iter != sData.end()) && (*iter != '/') && (*iter != '>')) {
                if (*iter == ' ') {
                  p->SetAttribute(sAttributeName, "");
                  sAttributeName = "";
                  sData = string::StripLeading(&*iter, " ");
                  iter = sData.begin();
                  continue;
                } else if (*iter == '=') {
                  iter++;
                  if ((iter != sData.end()) && (*iter == '\"')) {
                    sData.erase(sData.begin(), ++iter);
                    sData = string::StripLeading(sData, " ");

                    std::string::size_type nQuote = sData.find("\"");
                    std::string::size_type nSlashQuote = sData.find("\\\"");

                    while (std::string::npos != nQuote) {
                      if (nQuote < nSlashQuote) {
                        // attribute="value"
                        p->SetAttribute(sAttributeName, sAttributeValue + sData.substr(0, nQuote));
                        sAttributeName = "";
                        sAttributeValue = "";
                        sData = string::StripLeading(sData.substr(nQuote + 1), " ");
                        break;
                      } else {
                        // attribute="value\"innervalue..."
                        sAttributeValue += sData.substr(0, nSlashQuote);
                        sData = string::StripLeading(sData.substr(nSlashQuote + 2), " ");
                      }

                      nQuote = sData.find("\"");
                      nSlashQuote = sData.find("\\\"");
                    };

                    iter = sData.begin();
                    continue;
                  }
                } else sAttributeName += *iter;

                iter++;
              };

              if ('>' == *iter) {
                // >...
                iter++;
                if (!sAttributeName.empty()) {
                  // attribute>
                  p->SetAttribute(sAttributeName, sAttributeValue);
                  sAttributeName = "";
                }

                util::PROCESS_RESULT result = (*vChild.rbegin())->ParseFromStringParser(interface, nProcessedElements, sp);
                if (result != util::PROCESS_RESULT::COMPLETE) {
                  LOGERROR("Error parsing child node, returning");
                  return result;
                }
              }
            }
          } else {
            LOG("here, returning true");
            return util::PROCESS_RESULT::COMPLETE;
          }
        } else {
          // No tags, just content for the parent tag (this)
          cNode* p = CreateNodeAsChildAndAppend();

          p->type = TYPE::CONTENT_ONLY;
          p->sContentOnly += string::StripTrailingWhiteSpace(sp.GetToEnd());
          //LOG("Content only, returning true");
          return util::PROCESS_RESULT::COMPLETE;
        }
      }

      //LOG("At end of function, returning true");
      return util::PROCESS_RESULT::COMPLETE;
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

      LOG("Error could not open \"", spitfire::string::ToString(sFilename), "\", returning false");
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
      if (IsNameAndAttributesAndChildren()) {
        if (!sName.empty()) {
          std::string sTag = sTab + "&lt;" + sName;
          attribute_iterator iter=mAttribute.begin();
          for (;iter!=mAttribute.end();iter++)
            sTag += " " + iter->first + "=\"" + iter->second + "\"";

          if (!vChild.empty()) sTag+="&gt;";
          else sTag+="/&gt;";

          LOG(spitfire::string::ToString(sTag));
        }
      } else LOG(spitfire::string::ToString(sTab), "Content=\"", spitfire::string::ToString(sContentOnly), "\"");

      n = vChild.size();
      for (i=0;i<n;i++) vChild[i]->PrintToLog(sTab + "&nbsp;");

      if (!vChild.empty() && !sName.empty()) LOG(spitfire::string::ToString(sTab) , "&lt;/" , spitfire::string::ToString(sName) , "&gt;");
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


    void cNode::RemoveAttribute(const std::string& sAttribute)
    {
      attribute_iterator iter = mAttribute.find(sAttribute);
      const attribute_iterator iterEnd = mAttribute.end();
      if (iter != iterEnd) mAttribute.erase(iter);
    }


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
        value = spitfire::string::ToBool(spitfire::string::ToString(iter->second));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, uint32_t& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        value = spitfire::string::ToUnsignedInt(spitfire::string::ToString(iter->second));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, int32_t& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        value = spitfire::string::ToInt(spitfire::string::ToString(iter->second));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, uint64_t& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        value = spitfire::string::ToUnsignedInt(spitfire::string::ToString(iter->second));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, int64_t& value) const
    {
      const_attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        value = spitfire::string::ToInt(spitfire::string::ToString(iter->second));

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
        if (n > 0) value.x = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[0]));
        if (n > 1) value.y = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[1]));
        if (n > 2) value.z = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[2]));

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
        if (n > 0) value.x = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[0]));
        if (n > 1) value.y = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[1]));
        if (n > 2) value.z = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[2]));
        if (n > 3) value.w = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[3]));

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
        if (n > 0) value.r = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[0]));
        else if (n > 1) value.g = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[1]));
        else if (n > 2) value.b = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[2]));
        else if (n > 3) value.a = spitfire::string::ToFloat(spitfire::string::ToString(vSplit[3]));

        return true;
      }

      return false;
    }
    #endif // BUILD_XML_MATH
  }
}
