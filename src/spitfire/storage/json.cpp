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

#include <spitfire/util/cString.h>

#ifndef FIRESTARTER
#include <spitfire/util/log.h>
#endif

#include <spitfire/storage/json.h>

namespace spitfire
{
  namespace json
  {
    cNode::cNode() :
      type(TYPE::NULL_)
      iValueInt(0)
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

    void cNode::AppendChild(cNode* pChild)
    {
      ASSERT((type == TYPE::OBJECT) || (type == TYPE::ARRAY));
      vValueObjectOrArray.push_back(pChild);
    }

    // ** reader

    bool reader::ReadFromFile(cDocument& doc, const string_t& filename) const
    {
      std::string content;
      ... read filename into content
      return ReadFromStringUTF8(doc, content);
    }

    bool reader::ReadFromStringUTF8(cDocument& doc, const std::string& content) const
    {
      doc.Clear();
      doc.SetTypeObject();

      cStringParserUTF8 sp(content);
      if (sp.IsEnd()) return false;

      return ReadObject(doc, sp);
    }

    bool reader::ReadObject(cNode& object, cStringParserUTF8& sp) const
    {
      sp.SkipWhiteSpace();

      char c = sp.GetCharacter();
      if (c != '{') return false;

      while (true) {
        if (sp.IsEnd()) return false;

        sp.SkipWhiteSpace();
        if (sp.GetCharacter() == "}") break;

        if (sp.GetCharacters(4) == "null") {
          sp.SkipCharacters(4);
          cNode* pNull = object.Create();
          pNull->SetTypeNull();
          object.AppendChild(pNull);
        } else {
          c = sp.GetCharacter();
          switch (c) {
            case '{': {
              cNode* pObject = object.Create();
              pObject->SetName(sName);
              pObject->SetTypeObject();

              object.AppendChild(pObject);
              if (!ReadObject(*pObject, sp)) return false;

              break;
            }
            case '"': {
              std::string sName;
              if (!ReadString(sName, sp)) return false;

              sp.SkipWhiteSpace();
              c = sp.GetCharacter();
              if (c == ':') {
	sp.SkipCharacter();
	sp.SkipWhiteSpace();

	c = sp.GetCharacter();
	if (c == '{') {
	  cNode* pObject = object.Create();
	  pObject->SetName(sName);
	  pObject->SetTypeObject();
	  object.AppendChild(pObject);
	  if (!ReadObject(*pObject, sp)) return false;
	} else if (c == '[') {
	  cNode* pArray = object.Create();
	  pArray->SetName(sName);
	  pArray->SetTypeArray();
	  object.AppendChild(pArray);
	  if (!ReadArray(*pArray, sp)) return false;
	} else if (c == '"') {
	  // Read a string value
	  std::string sValue;
	  if (!ReadString(sValue, sp)) return false;

	  cNode* pProperty = object.Create();
	  pProperty->SetName(sName);
	  pProperty->SetTypeString(sValue);
	  object.AppendChild(pProperty);
	} else {
	  const std::string sValue = sp.GetToWhiteSpaceAndSkip();

	  if ((sValue == "true") || (sValue == "false")) {
	    // Read a boolean
	    cNode* pProperty = object.Create();
	    const bool bIsValueTrue = (sValue == "true");
	    pProperty->SetTypeBool(bIsValueTrue);
	    object.AppendChild(pProperty);
	  } else if (sValue.find('.')) {
	    // Read a float value
	    dValueFloat = string::ToFloat(string::ToString_t(sValue));
	  } else {
	    // Read an int value
	    iValueInt = string::ToInt(string::ToString_t(sValue));
	  }
	}
              } else {
	std::cout<<"Unexpected character in object after name"<<std::endl;
              }

              break;
            }
            case ',': {
              sp.SkipCharacter();
              break;
            }
            default: {
              std::cout<<"Unexpected character in object"<<std::endl;
              return false;
            }
          }
        }
      }

      return true;
    }

    bool reader::ReadArray(cNode& object, cStringParserUTF8& sp) const
    {
      return ReadObject(array, sp);
    }

    bool reader::ReadString(std::string& sValue, cStringParserUTF8& sp) const
    {
      char c = sp.GetCharacterAndSkip();
      if c != '"') return false;

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
      std::string content;
      WriteToStringUTF8(doc, content);

      std::ofstream f(spitfire::string::ToUTF8(filename).c_str());

      if (!f.is_open()) {

#ifndef FIRESTARTER
      LOG.Error("JSON", spitfire::string::ToUTF8(filename) + " not opened, returning false");
#endif
        return false;
      }

      // Write the header
      f<<content<<std::endl;

      f.close();

      return true;
    }

    bool writer::WriteToStringUTF8(const cDocument& doc, const std::string& content) const
    {
      bool bResult = true;

      std::ostringstream o(content);
      WriteObjectOrArray(doc, o, "");

      return bResult;
    }

    bool writer::WriteObjectOrArray(const cNode& object, std::ostringstream& o, const std::string& _sTabs) const
    {
      const std::string sName = object.GetName();
      if (!sName.empty()) o<<sTabs<<"\""<<sName<<"\":"<<std::endl;

      if (object.IsTypeObject()) o<<_sTabs<<"{"<<std::endl;
      else o<<_sTabs<<"["<<std::endl;

      {
        const std::string sTabs = _sTabs + "  ";

        const std::vector<cNode*>& children = object.GetValueObjectOrArray();
        const size_t n = children.size();
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
            else if (child.IsTypeBool()) o<<child.GetValueBool();
            else {
#ifndef FIRESTARTER
              LOG.Error("JSON", "WriteObjectOrArray Error writing child \"" + child.GetName(), "\", it is an unknown type");
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
