// Standard headers
#include <sstream>

// Spitfire headers
#include <spitfire/storage/csv.h>

namespace spitfire
{
  namespace csv
  {
    // ** cReader

    bool cReader::Open(const string_t& sFilePath)
    {
      return file.Open(sFilePath);
    }

    bool cReader::ReadLine(std::vector<string_t>& values)
    {
      std::cout<<"cReader::ReadLine"<<std::endl;

      values.clear();

      // Fields containing a line-break, double-quote, and/or commas must be quoted.
      // A (double) quote character in a field must be represented by two double quote characters.

      bool bReadAllValues = false;

      string_t sLine;
      file.ReadLineUTF8(sLine);

      string::cStringParser sp(sLine);
      while (!sp.IsEnd()) {
        if (sp.GetCharacter() == TEXT('\"')) {
          sp.SkipCharacter();

          ostringstream_t oValue;

          bool bFoundEndQuote = false;
          while (!bFoundEndQuote) {
            if (sp.IsEnd()) {
              std::cerr<<"cReader::ReadLine Error unexpected end of string"<<std::endl;
              break;
            }

            string_t sText;
            if (sp.GetToStringAndSkip(TEXT("\""), sText)) {
              if (sp.IsEnd()) {
                oValue<<sText;
                bFoundEndQuote = true;
              } else {
                const char_t c = sp.GetCharacterAndSkip();
                if (c == TEXT('\"')) {
                  oValue<<sText;
                  oValue<<"\"";
                } else if (c == TEXT(',')) {
                  oValue<<sText;
                  bFoundEndQuote = true;
                } else {
                  std::cerr<<"cReader::ReadLine Error found unexpected string at "<<sp.GetToEndAndSkip()<<std::endl;
                }
              }
            } else {
              oValue<<sp.GetToEndAndSkip();

              file.ReadLineUTF8(sLine);

              string::cStringParser sp2(sLine);
              sp = sp2;
            }
          }

          values.push_back(oValue.str());
        } else {
          //std::cout<<"cReader::ReadLine 0"<<std::endl;
          string_t sValue;
          if (sp.GetToStringAndSkip(TEXT(","), sValue)) {
            //std::cout<<"cReader::ReadLine 1"<<std::endl;
            values.push_back(sValue);
          } else {
            //std::cout<<"cReader::ReadLine 2"<<std::endl;
            values.push_back(sp.GetToEndAndSkip());
            bReadAllValues = true;
            break;
          }
        }
      }

      std::cout<<"cReader::ReadLine Finished reading "<<values.size()<<" values, returning "<<bReadAllValues<<std::endl;
      return bReadAllValues;
    }


    // ** cWriter

    cWriter::cWriter() :
      bIsFirstValue(true)
    {
    }

    bool cWriter::Open(const string_t& sFilePath)
    {
      o.open(string::ToUTF8(sFilePath).c_str());
      return o.is_open();
    }

    void cWriter::AddValue(const string_t& sValue)
    {
      if (!bIsFirstValue) o<<",";

      // Fields containing a line-break, double-quote, and/or commas must be quoted.
      // A (double) quote character in a field must be represented by two double quote characters.

      std::ostringstream oValue;
      string::cStringParser sp(sValue);
      bool bNeedsQuoting = false;
      while (!sp.IsEnd()) {
        const string_t s = sp.GetCharacterPossiblySurrogatePairAndSkip();
        assert(!s.empty());
        const char_t c = s[0];
        switch (c) {
          case TEXT('\r'):
          case TEXT('\n'):
          case TEXT(','): {
            bNeedsQuoting = true;
            break;
          }
          case TEXT('\"'): {
            bNeedsQuoting = true;
            oValue<<"\"";
            break;
          }
        }

        oValue<<string::ToUTF8(s);
      }

      if (bNeedsQuoting) o<<"\""<<oValue.str()<<"\"";
      else o<<oValue.str();

      bIsFirstValue = false;
    }

    void cWriter::EndRow()
    {
      o<<std::endl;
      bIsFirstValue = true;
    }
  }
}
