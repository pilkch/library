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

#include <breathe/breathe.h>

#include <breathe/util/cString.h>

#ifndef FIRESTARTER
#include <breathe/util/log.h>
#endif

#include <breathe/storage/xml.h>

#ifndef FIRESTARTER
#include <breathe/math/math.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cColour.h>
#endif

namespace breathe
{
  namespace xml
  {
    //cNode

    cNode::cNode() :
      type(TYPE_NAME_AND_ATTRIBUTES_AND_CHILDREN),
      pParent(nullptr),
      pNext(nullptr)
    {
    }

    cNode::cNode(cNode* inParent) :
      type(TYPE_NAME_AND_ATTRIBUTES_AND_CHILDREN),
      pParent(inParent),
      pNext(nullptr)
    {
    }

    cNode::cNode(const string_t& inFilename) :
      type(TYPE_NAME_AND_ATTRIBUTES_AND_CHILDREN),
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
      type = TYPE_NAME_AND_ATTRIBUTES_AND_CHILDREN;

      pParent = nullptr;
      pNext = nullptr;

      size_t n = vChild.size();
      for (size_t i=0;i<n;i++) SAFE_DELETE(vChild[i]);

      sName.clear();
      mAttribute.clear();

      sContentOnly.clear();
    }

    void cNode::LoadFromFile(const string_t& inFilename)
    {
      LOG<<"cNode::LoadFromFile \""<<inFilename<<"\""<<std::endl;
      std::ifstream f(breathe::string::ToUTF8(inFilename).c_str());

      if (!f.is_open()) {
#ifndef FIRESTARTER
        LOG.Error("XML", breathe::string::ToUTF8(inFilename) + " not found, returning");
        CONSOLE<<"XML "<<inFilename<<" not found, returning"<<std::endl;
#endif
        LOG<<"XML "<<inFilename<<" not found, returning"<<std::endl;
        return;
      }

      std::string sData;
      std::string line;
      while(!f.eof()) {
        std::getline(f, line);

        // Get rid of leading tabs
        sData += string::StripLeadingWhiteSpace(line);
      }
      f.close();

      LOG<<"XML "<<inFilename<<" contains \""<<sData<<"\""<<std::endl;
      ParseFromString(sData, nullptr);
    }

    std::string cNode::ParseFromString(const std::string& data, cNode* pPrevious)
    {
      std::string sData(data);
      while (sData.length()) {
        sData = string::StripLeadingWhiteSpace(sData);

        // XML declaration
        // <?xml version="1.0" encoding="UTF-8"?>
        while (sData.find("<?xml") == 0) {
          const size_t nEndDeclaration = sData.find("?>");

          if (nEndDeclaration == std::string::npos) {
#ifndef FIRESTARTER
            LOG.Error("XML", "Unterminated XML declaration");
#endif
            return "";
          }

          LOG<<"Found XML declaration node"<<std::endl;
          type = TYPE_XML_DELCARATION;
          sData = string::StripLeadingWhiteSpace(sData.substr(nEndDeclaration));
        }

        // Comments
        // <!-- ... -->
        while (sData.find("<!--") == 0) {
          const size_t nEndComment = sData.find("-->");

          if (nEndComment == std::string::npos) {
#ifndef FIRESTARTER
            LOG.Error("XML", "Unterminated Comment");
#endif
            return "";
          }

          LOG<<"Found comment node"<<std::endl;
          type = TYPE_COMMENT;
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
#ifndef FIRESTARTER
                LOG.Error("XML", "Opening tag \"" + sName + "\" doesn't match closing tag \"" + sClose + "\"");
#endif
                return "";
              }

              sData=string::StripLeading(sData.substr(angleBracket+1), " ");
            } else {
#ifndef FIRESTARTER
              LOG.Error("XML", "Tag \"" + sName + "\" doesn't have a closing tag");
#endif
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

            LOG<<"Found node "<<inName<<std::endl;
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
                  p->AddAttribute(sAttributeName, "");
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
                        p->AddAttribute(sAttributeName, sAttributeValue + sData.substr(0, nQuote));
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
                if (sAttributeName != "")
                {
                  // attribute>
                  p->AddAttribute(sAttributeName, sAttributeValue);
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

          p->type = TYPE_CONTENT_ONLY;
          p->sContentOnly += string::StripTrailingWhiteSpace(sData);
          return "";
        }
      }

      return sData;
    }


    void cNode::SaveToFile(const string_t& inFilename) const
    {
      std::ofstream f(breathe::string::ToUTF8(inFilename).c_str());

      if (f.is_open()) {
        const size_t n = vChild.size();
        for (size_t i=0;i<n;i++) vChild[i]->WriteToFile(f, "");

        f.close();
      }
#ifndef FIRESTARTER
      else LOG.Error("XML", breathe::string::ToUTF8(inFilename) + " not found");
#endif
    }

    void cNode::WriteToFile(std::ofstream& f, const std::string& sTab) const
    {
      ASSERT(f.is_open());

      // Write the header
      f<<"<?xml version=\"1.0\"?>"<<std::endl;

      if (IsNameAndAttributesAndChildren()) {
        if (sName != "") {
          std::string sTag = sTab + "<" + sName;
          const_attribute_iterator iter = mAttribute.begin();
          const const_attribute_iterator iterEnd = mAttribute.end();
          while (iter != iterEnd) {
            if (iter->second.length() > 0) sTag += " " + iter->first + "=\"" + iter->second + "\"";
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
      for (size_t i=0;i<n;i++) vChild[i]->WriteToFile(f, sTab + "\t");

      if (sName != "") {
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
        if (sName != "")
        {
          std::string sTag = sTab + "&lt;" + sName;
          attribute_iterator iter=mAttribute.begin();
          for (;iter!=mAttribute.end();iter++)
            sTag += " " + iter->first + "=\"" + iter->second + "\"";

          if (!vChild.empty()) sTag+="&gt;";
          else sTag+="/&gt;";

#ifndef FIRESTARTER
          LOG.Success("XML", sTag.c_str());
#endif
        }
      }
#ifndef FIRESTARTER
      else
        LOG.Success("XML", sTab + "Content=\"" + sContentOnly + "\"");
#endif

      n = vChild.size();
      for (i=0;i<n;i++) vChild[i]->PrintToLog(sTab + "&nbsp;");

#ifndef FIRESTARTER
      if (!vChild.empty() && sName != "")
        LOG.Success("XML", (sTab + "&lt;/" + sName + "&gt;").c_str());
#endif
    }
#endif // BUILD_DEBUG

    cNode* cNode::FirstChild()
    {
      if (!vChild.empty()) return vChild[0];

      return NULL;
    }

    cNode* cNode::FindChild(const std::string& sName)
    {
      if (!vChild.empty()) {
        cNode* p=vChild[0];
        while (p != nullptr) {
          if (sName == p->sName) return p;
          p = p->pNext;
        }
      }

      return NULL;
    }

    cNode* cNode::GetNext()
    {
      return pNext;
    }

    cNode* cNode::GetNext(const std::string& sName)
    {
      cNode* p = pNext;
      while (p != nullptr) {
        if (sName == p->sName) return p;
        p = p->pNext;
      };

      return NULL;
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

      if (!vChild.empty()) vChild.back()->pNext = pChild;

      vChild.push_back(pChild);
    }

    void cNode::SetTypeElement(const std::string& inName)
    {
      sName = string::StripTrailingWhiteSpace(inName);
      type = TYPE_NAME_AND_ATTRIBUTES_AND_CHILDREN;
    }

    void cNode::SetTypeContentOnly(const std::string& inContent)
    {
      sContentOnly = string::StripTrailingWhiteSpace(inContent);
      type = TYPE_CONTENT_ONLY;
    }

    std::string cNode::GetName() const
    {
      return sName;
    }

    std::string cNode::GetContent() const
    {
      return sContentOnly;
    }

    void cNode::AddAttribute(const std::string& inAttribute, const std::string& inValue)
    {
      mAttribute[inAttribute] = inValue;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, std::string& value)
    {
      attribute_iterator iter = mAttribute.find(sAttribute);
      attribute_iterator iterEnd = mAttribute.end();
      if (iter != iterEnd) {
        value = iter->second;
        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, std::wstring& value)
    {
      attribute_iterator iter = mAttribute.find(sAttribute);
      attribute_iterator iterEnd = mAttribute.end();
      if (iter != iterEnd) {
        value = breathe::string::ToWchar_t(iter->second);
        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, bool& value)
    {
      attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        value = breathe::string::ToBool(breathe::string::ToString_t(iter->second));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, float* pValue, size_t nValues)
    {
      assert(pValue != nullptr);
      attribute_iterator iter = mAttribute.find(sAttribute);
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

#ifndef FIRESTARTER
    bool cNode::GetAttribute(const std::string& sAttribute, math::cVec3& value)
    {
      attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        std::vector<std::string> vSplit;
        breathe::string::Split(iter->second, ',', vSplit);

        if (vSplit.size() > 0) value.x = breathe::string::ToFloat(breathe::string::ToString_t(vSplit[0]));
        if (vSplit.size() > 1) value.y = breathe::string::ToFloat(breathe::string::ToString_t(vSplit[1]));
        if (vSplit.size() > 2) value.z = breathe::string::ToFloat(breathe::string::ToString_t(vSplit[2]));

        return true;
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, math::cColour& value)
    {
      attribute_iterator iter = mAttribute.find(sAttribute);
      if (iter != mAttribute.end()) {
        std::vector<std::string> vSplit;
        breathe::string::Split(iter->second, ',', vSplit);

        if (vSplit.size() > 0) value.r = breathe::string::ToFloat(breathe::string::ToString_t(vSplit[0]));
        if (vSplit.size() > 1) value.g = breathe::string::ToFloat(breathe::string::ToString_t(vSplit[1]));
        if (vSplit.size() > 2) value.b = breathe::string::ToFloat(breathe::string::ToString_t(vSplit[2]));
        if (vSplit.size() > 3) value.a = breathe::string::ToFloat(breathe::string::ToString_t(vSplit[3]));

        return true;
      }

      return false;
    }
#endif


    bool reader::ReadFromFile(document& doc, const string_t& filename) const
    {
      doc.Clear();

      //doc.LoadFromFile(filename);

      return false;
    }

    bool reader::ReadFromString(document& doc, const string_t& content) const
    {
      doc.Clear();

      //doc.LoadFromString(content);

      return false;
    }


    bool writer::WriteToFile(const document& doc, const string_t& filename) const
    {
      doc.SaveToFile(filename);

      return false;
    }
  }
}
