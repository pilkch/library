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

#include <spitfire/storage/json.h>

namespace spitfire
{
  namespace json
  {
    cNode::cNode() :
      type(TYPE::NULL_),
      iValueInt(0),
      dValueFloat(0.0),
      bValueBool(false)
    {
    }

    cNode::~cNode()
    {
      // Delete all children for the object or array vector
      // NOTE: There could be children no matter what the type as the type may have changed since they were added
      const size_t n = vValueObjectOrArray.size();
      for (size_t i = 0; i < n; i++) SAFE_DELETE(vValueObjectOrArray[i]);
    }

    cNode* cNode::CreateNode()
    {
      return new cNode;
    }

    cNode* cNode::CreateNode(const std::string& _sName)
    {
      cNode* pNode = new cNode;
      pNode->SetName(_sName);
      return pNode;
    }

    void cNode::AppendChild(cNode* pChild)
    {
      ASSERT((type == TYPE::OBJECT) || (type == TYPE::ARRAY));
      vValueObjectOrArray.push_back(pChild);
    }

    const cNode* cNode::GetChild(const std::string& sName) const
    {
      ASSERT(type == TYPE::OBJECT);
      for (auto&& item : vValueObjectOrArray) {
        if (item->GetName() == sName) {
          return item;
        }
      }

      return nullptr;
    }

    cNode* cNode::GetChild(const std::string& sName)
    {
      ASSERT(type == TYPE::OBJECT);
      for (auto&& item : vValueObjectOrArray) {
        if (item->GetName() == sName) {
          return item;
        }
      }

      return nullptr;
    }

    void cNode::Clear()
    {
      type = TYPE::NULL_;
      sNameUTF8.clear();

      const size_t n = vValueObjectOrArray.size();
      for (size_t i = 0; i < n; i++) SAFE_DELETE(vValueObjectOrArray[i]);

      sValueUTF8String.clear();
      iValueInt = 0;
      dValueFloat = 0.0;
      bValueBool = false;
    }

    std::string cNode::GetValueString() const
    {
      switch (type) {
        case TYPE::BOOL_: {
          return spitfire::string::ToString(bValueBool);
        }
        case TYPE::INT: {
          return spitfire::string::ToString(iValueInt);
        }
        case TYPE::FLOAT: {
          std::cout<<"GetValueString \""<<sNameUTF8<<"\" returning "<<dValueFloat<<std::endl;
          // TODO: Move this to ToString?
          std::ostringstream o;
          o<<dValueFloat;
          return o.str();
        }
      }

      return sValueUTF8String;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, uint8_t& value) const
    {
      ASSERT(type == TYPE::OBJECT);
      for (auto&& item : vValueObjectOrArray) {
        if (item->GetName() == sAttribute) {
          const int iValue = item->GetValueInt();
          if ((iValue >= 0) && (iValue <= 0xff)) {
            value = static_cast<uint8_t>(iValue);
            return true;
          }

          break;
        }
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, uint16_t& value) const
    {
      ASSERT(type == TYPE::OBJECT);
      for (auto&& item : vValueObjectOrArray) {
        if (item->GetName() == sAttribute) {
          const int iValue = item->GetValueInt();
          if ((iValue >= 0) && (iValue <= 0xffff)) {
            value = static_cast<uint16_t>(iValue);
            return true;
          }

          break;
        }
      }

      return false;
    }

    bool cNode::GetAttribute(const std::string& sAttribute, std::string& value) const
    {
      ASSERT(type == TYPE::OBJECT);
      for (auto&& item : vValueObjectOrArray) {
        if (item->GetName() == sAttribute) {
          value = item->GetValueString();
          return true;
        }
      }

      return false;
    }

    void cNode::SetAttribute(const std::string& sAttribute, const char* szValue)
    {
      cNode* pNode = CreateNode(sAttribute.c_str());
      AppendChild(pNode);
      pNode->SetTypeString(szValue);
    }

    void cNode::SetAttribute(const std::string& sAttribute, const wchar_t* szValue)
    {
      cNode* pNode = CreateNode(sAttribute.c_str());
      AppendChild(pNode);
      pNode->SetTypeString(spitfire::string::ToUTF8(szValue));
    }

    void cNode::SetAttribute(const std::string& sAttribute, const std::string& sValue)
    {
      cNode* pNode = CreateNode(sAttribute.c_str());
      AppendChild(pNode);
      pNode->SetTypeString(sValue);
    }

    void cNode::SetAttribute(const std::string& sAttribute, const std::wstring& sValue)
    {
      cNode* pNode = CreateNode(sAttribute.c_str());
      AppendChild(pNode);
      pNode->SetTypeString(spitfire::string::ToUTF8(sValue));
    }

    void cNode::SetAttribute(const std::string& sAttribute, const bool value)
    {
      cNode* pNode = CreateNode(sAttribute.c_str());
      AppendChild(pNode);
      pNode->SetTypeBool(value);
    }

    void cNode::SetAttribute(const std::string& sAttribute, const uint64_t value)
    {
      cNode* pNode = CreateNode(sAttribute.c_str());
      AppendChild(pNode);
      pNode->SetTypeInt(value);
    }

    void cNode::SetAttribute(const std::string& sAttribute, const int64_t value)
    {
      cNode* pNode = CreateNode(sAttribute.c_str());
      AppendChild(pNode);
      pNode->SetTypeInt(value);
    }


    // ** reader

    bool reader::ReadFromFile(cDocument& doc, const string_t& filename) const
    {
      std::ifstream f(spitfire::string::ToUTF8(filename).c_str());

      if (!f.is_open()) {
#ifndef FIRESTARTER
        gLog.Error("JSON", spitfire::string::ToUTF8(filename) + " not found, returning false");
        CONSOLE<<"JSON "<<filename<<" not found, returning false"<<std::endl;
#endif
        return false;
      }

      std::ostringstream o;
      std::string line;
      while(!f.eof()) {
        std::getline(f, line);
        o<<line;
      }
      f.close();

      const std::string content = o.str();

      return ReadFromStringUTF8(doc, content);
    }

    bool reader::ReadFromStringUTF8(cDocument& doc, const std::string& content) const
    {
      std::cout<<"reader::ReadFromStringUTF8"<<std::endl;

      doc.Clear();
      doc.SetTypeObject();

      string::cStringParserUTF8 sp(content);
      if (sp.IsEnd()) return false;

      return ReadObjectOrArray(doc, sp);
    }

    bool reader::ReadObjectOrArray(cNode& object, string::cStringParserUTF8& sp) const
    {
      std::cout<<"reader::ReadObjectOrArray"<<std::endl;

      sp.SkipWhiteSpace();

      char c = sp.GetCharacter();
      if ((c != '{') && (c != '[')) {
        std::cout<<"reader::ReadObjectOrArray This is not an object or array, returning false"<<std::endl;
        return false;
      }

      sp.SkipCharacter();

      std::cout<<"reader::ReadObjectOrArray Entering loop"<<std::endl;
      while (true) {
        if (sp.IsEnd()) {
          std::cout<<"reader::ReadObjectOrArray At end of string, returning false"<<std::endl;
          return false;
        }

        std::cout<<"reader::ReadObjectOrArray Inside loop "<<sp.GetToEnd()<<std::endl;
        sp.SkipWhiteSpace();
        c = sp.GetCharacter();
        if ((c == '}') || (c == ']')) {
          // Skip the closing bracket
          sp.SkipCharacter();
          if (sp.IsEnd()) break;

          sp.SkipWhiteSpace();
          if (sp.IsEnd()) break;
          // If we have a comma at the end of the line then we need to skip it
          if (sp.GetCharacter() == ',') sp.SkipCharacter();

          break;
        }

        c = sp.GetCharacter();
        if (sp.GetCharacters(4) == "null") {
          sp.SkipCharacters(4);
          cNode* pNull = object.CreateNode();
          pNull->SetTypeNull();
          object.AppendChild(pNull);

          sp.SkipWhiteSpace();
          // If we have a comma at the end of the line then we need to skip it
          if (sp.GetCharacter() == ',') sp.SkipCharacter();
        } else if (c == '{') {
          cNode* pObject = object.CreateNode();
          pObject->SetTypeObject();
          object.AppendChild(pObject);
          if (!ReadObjectOrArray(*pObject, sp)) {
            std::cout<<"reader::ReadObjectOrArray Error reading object, returning false"<<std::endl;
            return false;
          }
        } else if (c == '[') {
          cNode* pArray = object.CreateNode();
          pArray->SetTypeArray();
          object.AppendChild(pArray);
          if (!ReadObjectOrArray(*pArray, sp)) {
            std::cout<<"reader::ReadObjectOrArray Error reading array, returning false"<<std::endl;
            return false;
          }
        } else {
          std::cout<<"reader::ReadObjectOrArray Reading string "<<sp.GetToEnd()<<std::endl;
          std::string sName;
          if (!ReadString(sName, sp)) {
            std::cout<<"reader::ReadObjectOrArray Error reading string, returning false"<<std::endl;
            return false;
          }

          sp.SkipWhiteSpace();
          c = sp.GetCharacter();
          switch (c) {
            case '{': {
              cNode* pObject = object.CreateNode();
              pObject->SetName(sName);
              pObject->SetTypeObject();

              object.AppendChild(pObject);
              if (!ReadObjectOrArray(*pObject, sp)) {
                std::cout<<"reader::ReadObjectOrArray Error reading object, returning false"<<std::endl;
                return false;
              }

              // If we have a comma at the end of the line then we need to skip it
              if (sp.GetCharacter() == ',') sp.SkipCharacter();

              break;
            }
            case ':': {
              sp.SkipCharacter();
              sp.SkipWhiteSpace();

              c = sp.GetCharacter();
              if (c == '{') {
                cNode* pObject = object.CreateNode();
                pObject->SetName(sName);
                pObject->SetTypeObject();
                object.AppendChild(pObject);
                if (!ReadObjectOrArray(*pObject, sp)) {
                  std::cout<<"reader::ReadObjectOrArray Error reading object, returning false"<<std::endl;
                  return false;
                }
              } else if (c == '[') {
                cNode* pArray = object.CreateNode();
                pArray->SetName(sName);
                pArray->SetTypeArray();
                object.AppendChild(pArray);
                if (!ReadObjectOrArray(*pArray, sp)) {
                  std::cout<<"reader::ReadObjectOrArray Error reading array, returning false"<<std::endl;
                  return false;
                }
              } else if (c == '"') {
                // Read a string value
                std::string sValue;
                if (!ReadString(sValue, sp))  {
                  std::cout<<"reader::ReadObjectOrArray Error reading string, returning false"<<std::endl;
                  return false;
                }

                cNode* pProperty = object.CreateNode();
                pProperty->SetName(sName);
                pProperty->SetTypeString(sValue);
                object.AppendChild(pProperty);
              } else {
                // Read the value
                std::string sValue;
                while (true) {
                  if (sp.IsEnd()) {
                    #ifndef FIRESTARTER
                    gLog.Error("JSON", "Unexpected end of string while reading value, returning false");
                    #endif
                    return false;
                  }

                  c = sp.GetCharacter();
                  if ((c == ',') || (c == '}') || spitfire::string::IsWhiteSpace(c)) break;
                  else {
                    sValue += c;
                    sp.SkipCharacter();
                  }
                }

                std::cout<<"Value \""<<sValue<<"\""<<std::endl;
                cNode* pProperty = object.CreateNode();
                object.AppendChild(pProperty);
                pProperty->SetName(sName);

                if ((sValue == "true") || (sValue == "false")) {
                  std::cout<<"Found bool"<<std::endl;
                  // Read a boolean
                  const bool bIsValueTrue = (sValue == "true");
                  pProperty->SetTypeBool(bIsValueTrue);
                } else if (sValue.find('.') != std::string::npos) {
                  std::cout<<"Found float"<<std::endl;
                  // Read a float value
                  const double dValueFloat = string::ToFloat(string::ToString(sValue));
                  pProperty->SetTypeFloat(dValueFloat);
                } else {
                  std::cout<<"Found int"<<std::endl;
                  // Read an int value
                  const int iValueInt = string::ToInt(string::ToString(sValue));
                  pProperty->SetTypeInt(iValueInt);
                }
              }

              // If we have a comma at the end of the line then we need to skip it
              if (sp.GetCharacter() == ',') sp.SkipCharacter();

              break;
            }
            default: {
              std::cout<<"Unexpected character '"<<c<<"' in object"<<std::endl;
              return false;
            }
          }
        }
      }

      std::cout<<"reader::ReadObjectOrArray Returning true"<<std::endl;
      return true;
    }

    bool reader::ReadString(std::string& sValue, string::cStringParserUTF8& sp) const
    {
      std::cout<<"reader::ReadString"<<std::endl;
      char c = sp.GetCharacterAndSkip();
      if (c != '"') return false;

      while (true) {

        if (sp.IsEnd()) return false;

        c = sp.GetCharacterAndSkip();
        if (c == '"') break;
        else if ((c == '\\') && (sp.GetCharacter() == '"')) {
          // Handle "\""
          sp.SkipCharacter();
          sValue += "\"";
        } else sValue += c;
      }

      return true;
    }


    // ** writer

    bool writer::WriteToFile(const cDocument& doc, const string_t& filename) const
    {
      std::ofstream f(spitfire::string::ToUTF8(filename).c_str());

      if (!f.is_open()) {
#ifndef FIRESTARTER
      gLog.Error("JSON", spitfire::string::ToUTF8(filename) + " not opened, returning false");
#endif
        return false;
      }

      // Write the root object to the file
      WriteObjectOrArray(doc, f, "");
      f<<std::endl;

      f.close();

      return true;
    }

    bool writer::WriteToStringUTF8(const cDocument& doc, std::string& content) const
    {
      bool bResult = true;

      // Write the root object to the string
      std::ostringstream o;
      WriteObjectOrArray(doc, o, "");
      content = o.str() + "\n";

      return bResult;
    }

    bool writer::WriteObjectOrArray(const cNode& object, std::ostream& o, const std::string& _sTabs) const
    {
      const std::string sName = object.GetName();
      if (!sName.empty()) o<<_sTabs<<"\""<<sName<<"\":"<<std::endl;

      if (object.IsTypeObject()) o<<_sTabs<<"{"<<std::endl;
      else o<<_sTabs<<"["<<std::endl;

      {
        const std::string sTabs = _sTabs + "  ";

        const std::vector<cNode*>& children = object.GetValueObjectOrArray();
        const size_t n = children.size();
        std::cout<<"child with "<<n<<" children"<<std::endl;
        for (size_t i = 0; i < n; i++) {
          const cNode& child = *children[i];
          if (child.IsTypeNull()) {
            o<<sTabs<<"null";
          } else if (child.IsTypeObject() || child.IsTypeArray()) {
            if (!WriteObjectOrArray(child, o, sTabs)) return false;
          } else {
            o<<sTabs<<"\""<<child.GetName()<<"\": ";

            if (child.IsTypeString()) o<<"\""<<child.GetValueString()<<"\"";
            else if (child.IsTypeInt()) o<<child.GetValueInt();
            else if (child.IsTypeFloat()) o<<child.GetValueFloat();
            else if (child.IsTypeBool()) o<<spitfire::string::ToUTF8(spitfire::string::ToString(child.GetValueBool()));
            else {
#ifndef FIRESTARTER
              gLog.Error("JSON", "WriteObjectOrArray Error writing child \"" + child.GetName() + "\", it is an unknown type");
#endif
              return false;
            }
          }

          // This is the last item if the next index would be more than our item count
          const bool bIsLast = ((i + 1) >= n);

          // Add a comma if there are more items to come
          if (bIsLast) o<<std::endl;
          else o<<","<<std::endl;
        }
      }

      if (object.IsTypeObject()) o<<_sTabs<<"}";
      else o<<_sTabs<<"]";

      return true;
    }
  }
}
