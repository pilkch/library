// Standard libraries
#include <cassert>
#include <cmath>

#include <string>

#include <iostream>
#include <fstream>

#include <vector>
#include <list>
#include <map>
#include <sstream>

#include <chrono>
#include <mutex>
#include <thread>

// Other libraries
#ifdef WIN32
#include <windows.h>
#endif

#ifdef BUILD_NETWORK_TLS
#include <gnutls/gnutls.h>

#include <libgnutlsmm/gnutlsmm.h>
#endif // BUILD_NETWORK_TLS

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>
#include <spitfire/util/log.h>
#include <spitfire/util/poll.h>
#include <spitfire/util/thread.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/math/math.h>

#include <spitfire/communication/network.h>
#include <spitfire/communication/uri.h>
#include <spitfire/communication/http.h>

// Breathe headers
#include <breathe/breathe.h>


const size_t STR_LEN = 8 * 1024;
const std::string STR_END = "\r\n";

template <class V>
typename std::map<std::string, V>::const_iterator MapFindCaseInsensitive(const std::map<std::string, V>& values, const std::string& sFind)
{
  typedef typename std::map<std::string, V>::const_iterator const_iterator;

  const_iterator iter = values.begin();
  const const_iterator iterEnd = values.end();
  while (iter != iterEnd) {
    if (spitfire::string::IsEqualInsensitive(iter->first, sFind)) break;

    iter++;
  }

  return iter;
}

namespace spitfire
{
  namespace network
  {
    const string_t sWebSiteBaseURL = TEXT("http://chris.iluo.net/");

    const string_t sWebSiteProgramSubFolder = TEXT("drive/");

    void BuildWebSiteURL(string_t& sURL)
    {
      sURL = sWebSiteBaseURL;
    }

    void BuildWebSiteProgramURL(string_t& sURL)
    {
      sURL = sWebSiteBaseURL + sWebSiteProgramSubFolder;
    }

    void BuildWebSiteProgramURLWithSubFolderOrFile(string_t& sURL, const string_t& sSubFolderOrFile)
    {
      sURL = sWebSiteBaseURL + sWebSiteProgramSubFolder + sSubFolderOrFile;
    }


    struct cExtensionToMime
    {
    public:
      const string_t sExtension;
      const std::string sMimeType;
      const bool bServeInline;
    };

    const cExtensionToMime extensionToMime[] = {
      { TEXT("css"),  "text/css",                      true },
      { TEXT("html"), "text/html",                     true },
      { TEXT("txt"),  "text/plain",                    true },
      { TEXT("js"),   "text/javascript",               true },

      { TEXT("jpg"),  "image/jpeg",                    true },
      { TEXT("png"),  "image/png",                     true },
      { TEXT("ico"),  "image/x-icon",                  true },
      { TEXT("gif"),  "image/gif",                     true },
      { TEXT("bmp"),  "image/bmp",                     true },

      { TEXT("jar"),  "application/java-archive",      true },
      { TEXT("swf"),  "application/x-shockwave-flash", true },

      { TEXT("wav"),  "audio/wav",                     true },
      { TEXT("mp3"),  "audio/mpeg",                    true },
      { TEXT("exe"),  "application/octet-stream",      false },
      { TEXT("zip"),  "application/zip",               false },
      { TEXT("doc"),  "application/msword",            false },
      { TEXT("pdf"),  "application/pdf",               true },
      { TEXT("rtf"),  "application/rtf",               false },
    };

    string_t GetMimeTypeFromExtension(const string_t& sExtension, bool& bServeInline)
    {
      const string_t sExtensionLower = string::ToLower(sExtension);

      const size_t n = countof(extensionToMime);
      for (size_t i = 0; i < n; i++) {
        if (sExtensionLower == extensionToMime[i].sExtension) {
          bServeInline = extensionToMime[i].bServeInline;
          return extensionToMime[i].sMimeType;
        }
      }

      // We didn't find the extension, so return the default mime
      gLog<<"GetMimeTypeFromExtension Mime type not known for file type \""<<sExtension<<"\""<<std::endl;
      bServeInline = false;
      return "application/octet-stream";
    }

    bool IsCachePublicForExtension(const string_t& sExtension)
    {
      return (
        (sExtension == TEXT("jpg")) ||
        (sExtension == TEXT("png")) ||
        (sExtension == TEXT("ico")) ||
        (sExtension == TEXT("gif")) ||
        (sExtension == TEXT("bmp"))
      );
    }


    bool IsSpecialCharacter(char c)
    {
      switch (c) {
        case '*':
        case '-':
        case '.':
        case '_': {
          return true;
        }
      }

      return false;
    }

    std::string Decode(const std::string& encodedString)
    {
      const char* encStr = encodedString.c_str();
      std::string decodedString;
      const char* tmpStr = nullptr;
      std::size_t cnt = 0;

      // Reserve enough space for the worst case.
      const std::size_t encodedLen = encodedString.size();
      decodedString.reserve(encodedLen);

      // Run down the length of the encoded string, examining each
      // character.  If it's a %, we discard it, read in the next two
      // characters, convert their hex value to a char, and write
      // that to the decoded string.  Anything else, we just copy over.
      for (std::size_t i = 0; i < encodedLen; ++i) {
        char curChar = encStr[i];

        if ('+' == curChar) {
          if (tmpStr != nullptr) {
            decodedString.append(tmpStr, cnt);
            tmpStr = nullptr;
            cnt = 0;
          }
          decodedString += ' ';
        } else if ('%' == curChar) {
          if (tmpStr != nullptr) {
            decodedString.append(tmpStr, cnt);
            tmpStr = nullptr;
            cnt = 0;
          }

          if (((i + 2) < encodedLen) && string::IsHexDigit(encStr[i + 1]) && string::IsHexDigit(encStr[i + 2])) {
            // Parse the hex digits and add the new character to the string
            char s[3];
            s[0] = encStr[i + 1];
            s[1] = encStr[i + 2];
            s[2] = 0;
            uint32_t value = breathe::string::FromHexStringToUint32_t(s);
            decodedString += static_cast<char>(value);

            // Skip because we have read two extra characters
            i += 2;
          } else {
            gLog<<"cHTTP::Decode Invalid %-escapes in \""<<encodedString<<"\""<<std::endl;
            return "";
          }
        } else {
          if (cnt == 0) tmpStr = encStr + i;
          ++cnt;
        }
      }
      if (tmpStr != nullptr) {
        decodedString.append(tmpStr, cnt);
        cnt = 0;
        tmpStr = nullptr;
      }

      return decodedString;
    }

    std::string Encode(const std::string& rawString)
    {
      char encodingBuffer[4] = { '%', '\0', '\0', '\0' };

      std::size_t rawLen = rawString.size();

      std::string encodedString;
      encodedString.reserve(rawLen);

      for (std::size_t i = 0; i < rawLen; ++i) {
        char curChar = rawString[i];

        if (curChar == ' ') encodedString += '+';
        else if (isalpha(curChar) || isdigit(curChar) || IsSpecialCharacter(curChar)) encodedString += curChar;
        else {
          unsigned int temp = static_cast<unsigned int>(curChar);

          encodingBuffer[1] = breathe::string::ConvertToHexDigit(temp / 0x10);
          encodingBuffer[2] = breathe::string::ConvertToHexDigit(temp % 0x10);
          encodedString += encodingBuffer;
        }
      }

      return encodedString;
    }



    namespace http
    {
      std::string URLEncode(const std::string& _url)
      {
        std::ostringstream o;

        for (auto& c : _url) {
          switch (c) {
            case ' ': {
              o<<'+';
              break;
            }
            default:
              o<<c;
          }
        }

        return o.str();
      }

      string_t GetStatusAsString(STATUS status)
      {
        return spitfire::string::ToString(int(status));
      }

      string_t GetStatusDescription(STATUS status)
      {
        switch (status) {
          case STATUS::OK: return TEXT("Ok");

          case STATUS::BAD_REQUEST: return TEXT("Bad Request");
          case STATUS::FORBIDDEN: return TEXT("Forbidden");
          case STATUS::NOT_FOUND: return TEXT("Not Found");
          case STATUS::METHOD_NOT_ALLOWED: return TEXT("Method Not Allowed");
          case STATUS::INTERNAL_SERVER_ERROR: return TEXT("Internal Server Error");
          case STATUS::NOT_IMPLEMENTED: return TEXT("Not Implemented");
          case STATUS::HTTP_VERSION_NOT_SUPPORTED: return TEXT("HTTP Version Not Supported");
        };

        return TEXT("Unknown");
      }



      bool ParseRequestPair(string::cStringParserUTF8& sp, std::string& sKey, std::string& sValue)
      {
        // If we are at the end of the request then return false
        if (sp.IsEnd()) return false;

        // If we have an empty line (Like at the end of the request) then return false
        char c = sp.GetCharacter();
        if ((c == '\r') || (c == '\n')) return false;

        // Parse path
        if (!sp.GetToStringAndSkip(": ", sKey)) {
          gLog<<"ParseRequestPair Couldn't parse key, returning false"<<std::endl;
          return false;
        }

        // Get to the end of the line
        if (!sp.GetToOneOfTheseCharacters("\r\n", sValue)) {
          gLog<<"ParseRequestPair Couldn't parse value, returning false"<<std::endl;
          return false;
        }

        // Skip the value
        sp.SkipCharacters(sValue.length());

        // Skip \r\n and \n
        c = sp.GetCharacter();
        if (c == '\r') {
          sp.SkipCharacter();
          if (sp.GetCharacter() == '\n') sp.SkipCharacter();
        } else if (c == '\n') sp.SkipCharacter();

        //gLog<<"ParseRequestPair returning \""<<sKey<<"\"=\""<<sValue<<"\""<<std::endl;
        return true;
      }

      bool ParseRequest(cRequest& request, const std::string& sRequest)
      {
        gLog<<"ParseRequest sRequest=\""<<sRequest<<"\""<<std::endl;

        request.Clear();

        string::cStringParserUTF8 sp(sRequest);

        if (sp.IsEnd()) {
          gLog<<"ParseRequest Empty string, returning false"<<std::endl;
          return false;
        }

        std::string sValue;

        // Parse method
        if (!sp.GetToWhiteSpaceAndSkip(sValue)) {
          gLog<<"ParseRequest Couldn't parse method, returning false"<<std::endl;
          return false;
        }

        if (sValue == "POST") request.SetMethodPost();
        else request.SetMethodGet();

        // Parse path
        if (!sp.GetToWhiteSpaceAndSkip(sValue)) {
          gLog<<"ParseRequest Couldn't parse path, returning false"<<std::endl;
          return false;
        }

        request.SetPath(Decode(sValue));

        // Skip to the end of the line
        sp.SkipToStringAndSkip("\n");

        std::string sKey;

        // Parse key value pairs
        while (ParseRequestPair(sp, sKey, sValue)) {
          if (sKey == "Host") { // Host: 127.0.0.1:12345
            size_t found = sValue.find(":");
            if (found != std::string::npos) {
              // Split the host and port up
              sValue = sValue.substr(0, found);
              //std::string sPort = sValue.substr(found + 1);

              request.SetHost(sValue);
              // TODO: Do something with the port
            } else request.SetHost(sValue);
          } else {
            request.AddOtherHeader(sKey, sValue);
          }
        }

        // Decode any form url encoded data
        gLog<<"request.GetContentType()=\""<<request.GetContentType()<<"\""<<std::endl;
        if (spitfire::string::StartsWith(request.GetContentType(), "application/x-www-form-urlencoded")) {
          const size_t nContentLengthBytes = request.GetContentLengthBytes();
          if (nContentLengthBytes != 0) {
            if (sp.IsEnd()) {
              gLog<<"ParseRequest Could not read the url encoded content, returning false"<<std::endl;
              return false;
            }

            // Read an empty line
            sp.SkipToStringAndSkip("\n");

            if (sp.IsEnd()) {
              gLog<<"ParseRequest Could not read the url encoded content, returning false"<<std::endl;
              return false;
            }

            // Read the url encoded string
            // action=delete&track=140736481787360&x=14&y=14
            std::string sLine = sp.GetToEndAndSkip();
            if (sLine.length() < nContentLengthBytes) {
              gLog<<"ParseRequest URL encoded content length was less than the expected "<<nContentLengthBytes<<", actual "<<sLine.length()<<", returning false"<<std::endl;
              return false;
            }

            // Make sure that we ignore bytes after the content length
            sLine.resize(nContentLengthBytes);

            gLog<<"ParseRequest url encoded string \""<<sLine<<"\""<<std::endl;

            // Decode our url encoded string;
            std::vector<std::string> pairs;
            spitfire::string::Split(sLine, '&', pairs);

            const size_t n = pairs.size();
            for (size_t i = 0; i < n; i++) {
              gLog<<"ParseRequest Pair \""<<pairs[i]<<"\""<<std::endl;
              size_t found = 0;
              if (!spitfire::string::Find(pairs[i], "=", found)) {
                gLog<<"ParseRequest Invalid pair \""<<pairs[i]<<"\", returning false"<<std::endl;
                return false;
              }

              const std::string sKey = pairs[i].substr(0, found);
              const std::string sValue = spitfire::network::Decode(pairs[i].substr(found + 1));
              gLog<<"ParseRequest Split \""<<sKey<<"\"=\""<<sValue<<"\""<<std::endl;
              request.AddFormData(sKey, sValue);
            }
          }
        }

        gLog<<"ParseRequest returning true"<<std::endl;
        return true;
      }



      // ** cRequest

      cResponse::cResponse() :
        status(STATUS::OK),
        bContentLengthSet(false),
        nContentLengthBytes(0),
        bContentDispositionServeInline(true),
        expires(EXPIRES::ONE_YEAR),
        cacheControl(CACHE_CONTROL::NOT_SPECIFIED)
      {
        SetContentTypeTextHTMLUTF8();
        SetConnectionClose();
      }

      void cResponse::SetStatus(STATUS _status)
      {
        status = _status;
      }

      void cResponse::SetContentLengthBytes(size_t _nContentLengthBytes)
      {
        if (_nContentLengthBytes != 0) bContentLengthSet = true;
        nContentLengthBytes = _nContentLengthBytes;
      }

      void cResponse::SetContentMimeType(const std::string& _sMimeType)
      {
        sMimeType = _sMimeType;
      }

      void cResponse::SetContentTypeTextPlainUTF8()
      {
        sMimeType = "text/plain";
      }

      void cResponse::SetContentTypeTextHTMLUTF8()
      {
        sMimeType = "text/html";
      }

      void cResponse::SetContentTypeTextEventStream()
      {
        sMimeType = "text/event-stream";
      }

      void cResponse::SetContentDispositionInline(const std::string& sFile)
      {
        // Content-Disposition: inline; filename="lilly.mp3"
        bContentDispositionServeInline = true;
        sContentDispositionFile = sFile;
      }

      void cResponse::SetExpiresMinusOne()
      {
        expires = EXPIRES::MINUS_ONE;
      }

      void cResponse::SetExpiresOneMonth()
      {
        expires = EXPIRES::ONE_MONTH;
      }

      void cResponse::SetExpiresOneYear()
      {
        expires = EXPIRES::ONE_YEAR;
      }

      void cResponse::SetCacheControlNoCache()
      {
        cacheControl = CACHE_CONTROL::NO_CACHE;
      }

      void cResponse::SetCacheControlPrivateMaxAgeZero()
      {
        cacheControl = CACHE_CONTROL::PRIVATE_MAX_AGE_ZERO;
      }

      void cResponse::SetCacheControlPublic()
      {
        cacheControl = CACHE_CONTROL::PUBLIC;
      }

      void cResponse::SetConnectionClose()
      {
        bConnectionKeepAlive = false;
      }

      void cResponse::SetConnectionKeepAlive()
      {
        bConnectionKeepAlive = true;
      }

      std::string cResponse::ToString() const
      {
        gLog<<"cResponse::ToString mime=\""<<sMimeType<<"\""<<std::endl;

        std::ostringstream o;
        o<<"HTTP/1.1 "<<GetStatusAsString(status)<<" "<<GetStatusDescription(status)<<"\n";
        o<<"Status: "<<GetStatusAsString(status)<<" "<<GetStatusDescription(status)<<"\n";
        o<<"Version: HTTP/1.1\n";

        // Set the content type
        if (spitfire::string::StartsWith(sMimeType, "text/")) o<<"Content-type: "<<sMimeType<<"; charset=UTF-8\n";
        else o<<"Content-type: "<<sMimeType<<"\n";

        // Set the content disposition
        // Content-Disposition: inline; filename="lilly.mp3"
        if (bContentDispositionServeInline && !sContentDispositionFile.empty()) o<<"Content-Disposition: inline; filename=\""<<sContentDispositionFile<<"\"\n";

        if (cacheControl == CACHE_CONTROL::NO_CACHE) o<<"Cache-control: no-cache\n";
        else if (cacheControl == CACHE_CONTROL::PRIVATE_MAX_AGE_ZERO) o<<"Cache-control: private, max-age=0\n";
        else if (cacheControl == CACHE_CONTROL::PUBLIC) o<<"Cache-Control: public\n";

        spitfire::util::cDateTime dateTime;
        dateTime.SetFromUniversalTimeNow();
        o<<"Date: "<<dateTime.GetRFC1123Format()<<"\n";

        if (expires == EXPIRES::MINUS_ONE) o<<"Expires: -1\n";
        else if ((expires == EXPIRES::ONE_MONTH) || (expires == EXPIRES::ONE_YEAR)) {
          spitfire::util::cDateTime dateTime;
          dateTime.SetFromLocalTimeNow();
          const int days = (expires == EXPIRES::ONE_MONTH) ? 30 : 365;
          dateTime.AddDays(days);
          o<<"Expires: "<<dateTime.GetRFC1123Format()<<"\n";
        }

        //o<<"Server: Apache 1.0 (Unix)\n";
        //o<<"x-frame-options: SAMEORIGIN\n";
        //o<<"x-xss-protection: 1; mode=block\n";
        if (bContentLengthSet) o<<"Content-Length: "<<nContentLengthBytes<<"\n";
        o<<"Connection: "<<(bConnectionKeepAlive ? "Keep-Alive" : "Close")<<"\n\n";

        return o.str();
      }


      // ** cConnectionHTTP

      class cConnectionHTTP
      {
      public:
        cConnectionHTTP();

        size_t ReadHeader(network::cConnectionTCP& connection);
        size_t ReadContent(network::cConnectionTCP& connection, void* pBuffer, size_t len);


        int GetStatus() const { return status; }
        std::string GetContentType() const;
        size_t GetContentLength() const;
        bool IsTransferEncodingChunked() const;

        std::string GetContentAsText() const;

      private:
        void ParseHeader();

        std::string header;
        int status;
        std::map<std::string, std::string> headerValues;

        static const size_t nBufferLength = 1024;
        std::vector<char> content;
      };

      cConnectionHTTP::cConnectionHTTP() :
        status(0)
      {
      }

      void cConnectionHTTP::ParseHeader()
      {
        gLog<<"cConnectionHTTP::ParseHeader"<<std::endl;

        headerValues.clear();


        std::vector<std::string> vHeader;
        string::SplitOnNewLines(header, vHeader);

        if (vHeader.empty()) {
          gLog<<"cConnectionHTTP::ParseHeader No lines to parse, returning"<<std::endl;
          return;
        }

        // HTTP/1.1 200 OK
        {
          // Split into "HTTP/1.1", "200", "OK"
          std::vector<std::string> vParts;
          string::SplitOnNewLines(vHeader[0], vParts);

          if (vParts.size() >= 3) {
            status = string::ToInt(string::ToString(vParts[1]));
          }
        }

        std::string before;
        std::string after;

        const size_t n = vHeader.size();
        for (size_t i = 0; i < n; i++) {
          bool bIsFound = string::Split(vHeader[i], ": ", before, after);
          if (!bIsFound) continue;

          // Add it to the key value pairs
          headerValues[before] = after;
        }
      }




      // ** cRequest

      std::string cRequest::GetAccept() const
      {
        std::map<std::string, std::string>::const_iterator iter = MapFindCaseInsensitive(mOtherHeaders, "Accept");
        if (iter != mOtherHeaders.end()) return iter->second;

        // Return an empty string if we haven't specified an accept
        return "";
      }

      std::string cRequest::GetContentType() const
      {
        const std::map<std::string, std::string>::const_iterator iter = MapFindCaseInsensitive(mOtherHeaders, "Content-Type");
        if (iter != mOtherHeaders.end()) return iter->second;

        // Return an empty string if we haven't specified a content type
        return "";
      }

      size_t cRequest::GetContentLengthBytes() const
      {
        const std::map<std::string, std::string>::const_iterator iter = MapFindCaseInsensitive(mOtherHeaders, "Content-Length");
        if (iter != mOtherHeaders.end()) return spitfire::string::ToUnsignedInt(iter->second);

        // Return a default content type
        return 0;
      }

      void cRequest::AddPostFileFromPath(const std::string& _sName, const string_t& _sFilePath)
      {
        file.sName = _sName;
        file.sFilePath = _sFilePath;
      }

      bool cRequest::IsConnectionClose() const
      {
        const std::map<std::string, std::string>::const_iterator iter = MapFindCaseInsensitive(mOtherHeaders, "Connection");
        if (iter != mOtherHeaders.end()) return (spitfire::string::IsEqualInsensitive(iter->second, "Close"));

        // Return the default value
        return TEXT("Close");
      }

      void cRequest::SetConnectionClose()
      {
        mOtherHeaders["Connection"] = "Close";
      }

      void cRequest::SetConnectionKeepAlive()
      {
        mOtherHeaders["Connection"] = "Keep-Alive";
      }

      std::string cRequest::CreateVariablesString() const
      {
        std::ostringstream o;

        std::map<std::string, std::string>::const_iterator iter = mVariables.begin();
        const std::map<std::string, std::string>::const_iterator iterEnd = mVariables.end();
        // Write the first item so that we can safely add "&" between every other item
        if (iter != iterEnd) {
          o<<Encode(iter->first)<<"="<<Encode(iter->second);
          iter++;
        }
        while (iter != iterEnd) {
          o<<"&"<<Encode(iter->first)<<"="<<Encode(iter->second);
          iter++;
        }

        return o.str();
      }

      std::string cRequest::CreateRequestHeader() const
      {
        std::ostringstream o;

        if (method == METHOD::GET) o<<"GET";
        else o<<"POST";

        std::string sRelativeURIWithAnyVariables = "/" + spitfire::string::ToUTF8(sPath);
        if ((method == METHOD::GET) && !mVariables.empty()) {
          // Append the encoded variables to the path
          sRelativeURIWithAnyVariables += "?" + CreateVariablesString();
        }

        o<<" "<<sRelativeURIWithAnyVariables<<" HTTP/1.1"<<STR_END;

        // TODO: Use ports other than 80 when required
        //if (uri.GetPort() == 80) o<<"Host: "<<uri.GetServer()<<STR_END;
        //else o<<"Host: "<<uri.GetServer()<<":"<<uri.GetPort()<<STR_END;
        o<<"Host: "<<spitfire::string::ToUTF8(sHost)<<STR_END;

        if (nOffsetBytes != 0) o<<"Range: bytes="<<nOffsetBytes<<"-"<<STR_END;

        o<<"User-Agent: Mozilla/4.0 (compatible; Spitfire 1.0; Linux)"<<STR_END;
        o<<"Accept: */*"<<STR_END;
        o<<"Connection: "<<(IsConnectionKeepAlive() ? "Keep-Alive" : "Close")<<STR_END;

        return o.str();
      }

/*
Content-type: multipart/form-data, boundary=AaB03x

--AaB03x
content-disposition: form-data; name="field1"

Joe Blow
--AaB03x
content-disposition: form-data; name="pics"; filename="file1.txt"
Content-Type: text/plain

... contents of file1.txt ...
--AaB03x--



Content-Type: text/plain

OR

Content-type: image/gif
Content-Transfer-Encoding: binary
*/



      // ** cServerUtil

      void cServerUtil::SendResponse(cConnectedClient& connection, const cResponse& response) const
      {
        connection.Write(response.ToString());
      }

      void cServerUtil::SendContent(cConnectedClient& connection, const std::string& sContentUTF8) const
      {
        connection.Write(sContentUTF8);
      }

      void cServerUtil::ServeError404(cConnectedClient& connection, const cRequest& request) const
      {
        string_t sContentUTF8(
          "<!DOCTYPE html>"
          "<html lang=\"en\">"
          "  <meta charset=\"utf-8\">"
          "  <meta name=\"viewport\" content=\"initial-scale=1, minimum-scale=1, width=device-width\">"
          "  <title>Error 404 (Not Found)</title>"
          "  <style>"
          "    *{margin:0;padding:0}html,code{font:15px/22px arial,sans-serif}html{background:#fff;color:#222;padding:15px}body{margin:7% auto 0;max-width:390px;min-height:180px;padding:30px 0 15px}* > body{background:url(http://www.google.com/images/errors/robot.png) 100% 5px no-repeat;padding-right:205px}p{margin:11px 0 22px;overflow:hidden}ins{color:#777;text-decoration:none}a img{border:0}@media screen and (max-width:772px){body{background:none;margin-top:0;max-width:none;padding-right:0}}"
          "  </style>"
          "  <a href=\"http://www.google.com/\"><img src=\"http://www.google.com/images/errors/logo_sm.gif\" alt=\"Google\"></a>"
          "  <p><b>404.</b> <ins>That’s an error.</ins>"
        );

        // Add the message
        sContentUTF8 +=
          "  <p>"
          "    The requested URL <code>" + request.GetPath() + "</code> was not found on this server.  <ins>That’s all we know.</ins>"
          "</p>"
          "</html>"
        ;

        cResponse response;
        response.SetStatus(STATUS::NOT_FOUND);
        response.SetContentLengthBytes(sContentUTF8.length());
        response.SetContentTypeTextHTMLUTF8();

        SendResponse(connection, response);
        SendContent(connection, sContentUTF8);
      }

      void cServerUtil::ServeError(cConnectedClient& connection, const cRequest& request, STATUS status) const
      {
        string_t sContentUTF8(
          "<!DOCTYPE html>"
          "<html lang=\"en\">"
          "  <meta charset=\"utf-8\">"
          "  <meta name=\"viewport\" content=\"initial-scale=1, minimum-scale=1, width=device-width\">"
          "  <title>Error " + GetStatusAsString(status) + " (" + GetStatusDescription(status) + ")</title>"
          "  <style>"
          "    *{margin:0;padding:0}html,code{font:15px/22px arial,sans-serif}html{background:#fff;color:#222;padding:15px}body{margin:7% auto 0;max-width:390px;min-height:180px;padding:30px 0 15px}* > body{background:url(//www.google.com/images/errors/robot.png) 100% 5px no-repeat;padding-right:205px}p{margin:11px 0 22px;overflow:hidden}ins{color:#777;text-decoration:none}a img{border:0}@media screen and (max-width:772px){body{background:none;margin-top:0;max-width:none;padding-right:0}}"
          "  </style>"
          "  <a href=\"http://www.google.com/\"><img src=\"http://www.google.com/images/errors/logo_sm.gif\" alt=\"Google\"></a>"
          "  <p><b>404.</b> <ins>That’s an error.</ins>"
        );

        // Add the message
        sContentUTF8 +=
          "  <p>"
          "    The requested URL <code>" + request.GetPath() + "</code> was not found on this server.  <ins>That’s all we know.</ins>"
          "</p>"
          "</html>"
        ;

        cResponse response;
        response.SetStatus(status);
        response.SetContentLengthBytes(sContentUTF8.length());
        response.SetContentTypeTextHTMLUTF8();

        SendResponse(connection, response);
        SendContent(connection, sContentUTF8);
        SendContent(connection, "\n\n");
      }

      void cServerUtil::ServeFile(cConnectedClient& connection, const cRequest& request, const string_t& sMimeTypeUTF8, const string_t& sRelativeFilePath) const
      {
        if (!request.IsMethodGet()) {
          ServeError(connection, request, STATUS::NOT_IMPLEMENTED);
          return;
        }

        if (!filesystem::FileExists(sRelativeFilePath)) {
          ServeError404(connection, request);
          return;
        }

        if (filesystem::IsFolder(sRelativeFilePath)) {
          ServeError(connection, request, STATUS::INTERNAL_SERVER_ERROR);
          return;
        }

        storage::cReadFile file(sRelativeFilePath);
        if (!file.IsOpen()) {
          ServeError(connection, request, STATUS::INTERNAL_SERVER_ERROR);
          return;
        }

        const size_t nFileSizeBytes = filesystem::GetFileSizeBytes(sRelativeFilePath);

        cResponse response;
        response.SetStatus(STATUS::OK);
        response.SetContentLengthBytes(nFileSizeBytes);
        response.SetContentMimeType(sMimeTypeUTF8);
        if (IsCachePublicForExtension(filesystem::GetExtensionNoDot(sRelativeFilePath))) response.SetCacheControlPublic();
        response.SetConnectionClose();

        SendResponse(connection, response);

        const size_t nBufferSizeBytes = 1024;

        uint8_t buffer[nBufferSizeBytes];

        while (file.IsOpen()) {
          const size_t nRead = file.Read(buffer, nBufferSizeBytes);
          if (nRead == 0) break;

          connection.Write(buffer, nRead);
        }

        connection.Write("\n\n");
      }

      void cServerUtil::ServeFileWithResolvedFilePath(cConnectedClient& connection, const cRequest& request, const string_t& sFilePath) const
      {
        if (!request.IsMethodGet()) {
          ServeError(connection, request, STATUS::NOT_IMPLEMENTED);
          return;
        }

        if (!filesystem::FileExists(sFilePath)) {
          ServeError404(connection, request);
          return;
        }

        if (filesystem::IsFolder(sFilePath)) {
          ServeError(connection, request, STATUS::INTERNAL_SERVER_ERROR);
          return;
        }

        storage::cReadFile file(sFilePath);
        if (!file.IsOpen()) {
          ServeError(connection, request, STATUS::INTERNAL_SERVER_ERROR);
          return;
        }

        const size_t nFileSizeBytes = filesystem::GetFileSizeBytes(sFilePath);

        bool bServeInline = false;
        const std::string sMimeTypeUTF8 = GetMimeTypeFromExtension(filesystem::GetExtensionNoDot(sFilePath), bServeInline);

        cResponse response;
        response.SetStatus(STATUS::OK);
        response.SetContentLengthBytes(nFileSizeBytes);
        response.SetContentMimeType(sMimeTypeUTF8);
        if (bServeInline) response.SetContentDispositionInline(filesystem::GetFile(sFilePath));
        response.SetConnectionClose();

        SendResponse(connection, response);

        const size_t nBufferSizeBytes = 1024;

        uint8_t buffer[nBufferSizeBytes];

        while (file.IsOpen()) {
          const size_t nRead = file.Read(buffer, nBufferSizeBytes);
          if (nRead == 0) break;

          connection.Write(buffer, nRead);
        }

        connection.Write("\n\n");
      }

      bool cServerUtil::GetLocalFilePathInWebDirectory(std::string& sResolvedLocalFilePath, const std::string sRelativeFilePath) const
      {
        // Build the path
        sResolvedLocalFilePath = "data/web/" + string::StripLeading(sRelativeFilePath, "/");

        // Check if the path is trying to be tricky
        if ((string::CountOccurrences(sResolvedLocalFilePath, "./") != 0) || (string::CountOccurrences(sResolvedLocalFilePath, "..") != 0)) {
          gLog<<"cServer::GetLocalFilePathInWebDirectory Request path \""<<sResolvedLocalFilePath<<"\" is a compressed path, returning false"<<std::endl;
          return false;
        } else if (!filesystem::FileExists(sResolvedLocalFilePath)) {
          gLog<<"cServer::GetLocalFilePathInWebDirectory Request path \""<<sResolvedLocalFilePath<<"\" was not found, returning false"<<std::endl;
          return false;
        }

        // Look for index.html if it is a folder
        if (filesystem::IsFolder(sResolvedLocalFilePath)) {
          if (!filesystem::FileExists(sResolvedLocalFilePath + "index.html")) {
            gLog<<"cServer::GetLocalFilePathInWebDirectory Request path \""<<sResolvedLocalFilePath<<"\" is a folder, but it doesn't contain an index.html, returning false"<<std::endl;
            return false;
          }

          sResolvedLocalFilePath += "index.html";
        }

        return true;
      }

      bool cServerUtil::IsFileInWebDirectory(const std::string sRelativeFilePath) const
      {
        std::string sResolvedLocalFilePath;
        return GetLocalFilePathInWebDirectory(sResolvedLocalFilePath, sRelativeFilePath);
      }

      void cServerUtil::ServeFile(cConnectedClient& connection, const cRequest& request) const
      {
        std::string sResolvedLocalFilePath;
        if (!GetLocalFilePathInWebDirectory(sResolvedLocalFilePath, request.GetPath())) {
          ServeError404(connection, request);
        } else {
          bool bServeInline = false;
          const std::string sMimeTypeUTF8 = GetMimeTypeFromExtension(filesystem::GetExtensionNoDot(sResolvedLocalFilePath), bServeInline);
          ServeFile(connection, request, sMimeTypeUTF8, sResolvedLocalFilePath);
        }
      }



      // ** cConnectionHTTP

      size_t cConnectionHTTP::ReadHeader(network::cConnectionTCP& connection)
      {
        gLog<<"cConnectionHTTP::ReadHeader"<<std::endl;
        ASSERT(connection.IsOpen());

        // If this fails we have already read the header
        ASSERT(content.empty());


        // Wait for the request to be sent
        spitfire::util::SleepThisThreadMS(1000);

        //const size_t bytes_readable = connection.GetBytesToRead();

        //gLog<<"cConnectionHTTP::ReadHeader bytes_readable="<<bytes_readable<<std::endl;

        size_t len = 0;
        char szHeaderBuffer[nBufferLength + 1];
        //gLog<<"cConnectionHTTP::ReadHeader About to start reading stuff"<<std::endl;
        while (connection.IsOpen()) {
          //gLog<<"cConnectionHTTP::ReadHeader Reading"<<std::endl;
          len = connection.Read(szHeaderBuffer, nBufferLength, 5000);
          //gLog<<"cConnectionHTTP::ReadHeader Read has finished"<<std::endl;
          if (len == 0) {
            gLog<<"cConnectionHTTP::ReadHeader Read 0 bytes, breaking"<<std::endl;
            break;
          }

          gLog<<"cConnectionHTTP::ReadHeader Terminating string"<<std::endl;
          szHeaderBuffer[len] = 0;
          gLog<<"cConnectionHTTP::ReadHeader Read "<<len<<" bytes into buffer szHeaderBuffer=\""<<szHeaderBuffer<<"\""<<std::endl;

          std::string sBuffer(szHeaderBuffer);
          std::string::size_type i = sBuffer.find("\r\n\r\n");

          if (i == std::string::npos) {
            header += sBuffer;
          } else {
            header += sBuffer.substr(0, i);
            gLog<<"cConnectionHTTP::ReadHeader Read into buffer header=\""<<header<<"\""<<std::endl;

            // Skip "\r\n\r\n"
            i += 4;

            ASSERT(len >= i);
            const size_t nBufferRead = len - i;
            std::cout<<"cConnectionHTTP::ReadHeader nBufferRead="<<nBufferRead<<std::endl;

            if (nBufferRead != 0) {
              // There is something to put in the content buffer so fill it up and return

              // Make space to append the next chunk to the end of our current header buffer
              //gLog<<"cConnectionHTTP::ReadHeader Content reserving  "<<nBufferRead<<" bytes"<<std::endl;
              content.reserve(nBufferRead);

              // Append to the header
              gLog<<"cConnectionHTTP::ReadHeader Content inserting  "<<nBufferRead<<" bytes"<<std::endl;
              content.insert(content.begin(), nBufferRead, '\0');
              //gLog<<"cConnectionHTTP::ReadHeader Content copying "<<nBufferRead<<" bytes"<<std::endl;
              memcpy(&content[0], &szHeaderBuffer[i], nBufferRead);
              break;
            }
          }
        }

        //gLog<<"cConnectionHTTP::ReadHeader Parsing the header \""<<header<<"\""<<std::endl;
        // Actually parse the header into status, key value pairs etc.
        ParseHeader();

        gLog<<"cConnectionHTTP::ReadHeader returning "<<header.length()<<std::endl;
        return header.length();
      }

      size_t cConnectionHTTP::ReadContent(network::cConnectionTCP& connection, void* pOutContent, size_t len)
      {
        gLog<<"cConnectionHTTP::ReadContent len="<<len<<std::endl;

        size_t nContentReadThisTimeAround = 0;

        // If we already have content data then read from there first
        const size_t nBufferRead = content.size();
        //gLog<<"cConnectionHTTP::ReadContent nBufferRead="<<nBufferRead<<std::endl;
        if (nBufferRead != 0) {
          //gLog<<"cConnectionHTTP::ReadContent Reading "<<nBufferRead<<" previous bytes"<<std::endl;
          const size_t smaller = min(nBufferRead, len);
          //gLog<<"cConnectionHTTP::ReadContent Actually reading "<<smaller<<" previous bytes"<<std::endl;
          memcpy(pOutContent, &content[0], smaller);

          // Increment our buffer
          pOutContent = static_cast<char*>(pOutContent) + smaller;
          len -= smaller;


          std::vector<char> temp(content.begin() + smaller, content.end());
          content = temp;
          ASSERT(content.size() + smaller == nBufferRead);

          nContentReadThisTimeAround += smaller;
        }

        if (len != 0) {
          if (!connection.IsOpen()) {
            gLog<<"cConnectionHTTP::ReadContent Connection is closed, returning "<<nContentReadThisTimeAround<<std::endl;
            return nContentReadThisTimeAround;
          }

          // Now read the rest from the connection
          const size_t n = connection.Read(pOutContent, len, 2000);
          //gLog<<"cConnectionHTTP::ReadContent nBufferRead="<<nBufferRead<<" n="<<n<<std::endl;
          nContentReadThisTimeAround += n;
        }

        // TODO:
        // if (IsTransferEncodingChunked()) {
        //   ... begins with "\r\n25\r\n"
        //
        //   ... ends in "\r\n0\r\n"
        // }

        gLog<<"cConnectionHTTP::ReadContent returning "<<nContentReadThisTimeAround<<std::endl;
        return nContentReadThisTimeAround;
      }



      std::string cConnectionHTTP::GetContentType() const
      {
        std::map<std::string, std::string>::const_iterator iter = MapFindCaseInsensitive(headerValues, "Content-Type");
        if (iter != headerValues.end()) {
          return iter->second;
        }

        return "";
      }

      size_t cConnectionHTTP::GetContentLength() const
      {
        std::map<std::string, std::string>::const_iterator iter = MapFindCaseInsensitive(headerValues, "Content-Length");
        if (iter != headerValues.end()) {
          return string::ToUnsignedInt(string::ToString(iter->second));
        }

        return 0;
      }

      std::string cConnectionHTTP::GetContentAsText() const
      {
        std::string sText;
        const size_t n = content.size();
        sText.resize(n + 1);
        for (size_t i = 0; i < n; i++) sText[i] = content[i];

        // Terminate the string
        sText[n + 1] = 0;

        return sText;
      }


      // ** cConnectionHTTP
      //
      // HTTP/1.1 200 OK
      // Content-Type: text/plain
      // Transfer-Encoding: chunked
      //
      // 25
      // This is the data in the first chunk
      //
      // 1C
      // and this is the second one
      //
      // 0
      //

      bool cConnectionHTTP::IsTransferEncodingChunked() const
      {
        std::map<std::string, std::string>::const_iterator iter = MapFindCaseInsensitive(headerValues, "Transfer-Encoding");
        if (iter != headerValues.end()) {
          return (spitfire::string::IsEqualInsensitive(iter->second, "chunked"));
        }

        return false;
      }




      // *** cHTTP

      void cHTTP::Download(const std::string& path, METHOD method, cRequestListener& listener) const
      {
        gLog<<"cHTTP::Download"<<std::endl;

        cURI uri(spitfire::network::http::URLEncode(path));

        cRequest request;
        request.SetHost(uri.GetServer());
        request.SetPath(uri.GetRelativePath());
        request.SetMethod(method);

        breathe::network::cConnectionTCP connection;

        SendRequest(connection, request, listener);
      }

      void cHTTP::SendRequest(const cRequest& request, cRequestListener& listener) const
      {
        gLog<<"cHTTP::SendRequest"<<std::endl;

        breathe::network::cConnectionTCP connection;

        SendRequest(connection, request, listener);
      }

      void cHTTP::SendRequest(cConnectionTCP& connection, const cRequest& request, cRequestListener& listener) const
      {
        gLog<<"cHTTP::SendRequest"<<std::endl;

        // Start downloading at the beginning
        //uint32_t progress = 0;

        //METHOD method = request.IsMethodGet() ? METHOD::GET : METHOD::POST;

        STATE state = STATE::BEFORE_DOWNLOADING;
        (void)state; // Avoid warning about unused variable

        state = STATE::CONNECTING;
        connection.Open(spitfire::string::ToUTF8(request.GetHost()), 80);

        if (!connection.IsOpen()) {
          gLog<<"cHTTP::SendRequest Connection failed, returning"<<std::endl;
          state = STATE::CONNECTION_FAILED;
          return;
        }

        // Send request and content
        {
          // Send request
          state = STATE::SENDING_REQUEST;
          {
            const std::string sRequestHeader(request.CreateRequestHeader());
            const size_t len = sRequestHeader.length();
            const size_t sent = connection.Write(sRequestHeader.c_str(), len);
            if (sent != len) {
              gLog<<"cHTTP::SendRequest Sending request, Write FAILED"<<std::endl;
              return;
            }
          }

          // Send content
          state = STATE::SENDING_CONTENT;

          // Create parameters string
          const std::string sVariables(request.CreateVariablesString());

          if (request.IsMethodPost()) {
            if (request.file.sFilePath.empty()) {
              std::ostringstream o;
              // As the variables as an encoded string
              const size_t content_length = sVariables.length();

              o<<"Content-Type: application/x-www-form-urlencoded"<<STR_END;
              o<<"Content-Length: "<<content_length<<STR_END;
              o<<STR_END;
              o<<sVariables<<STR_END;
              o<<STR_END;

              // Send the content
              const std::string sContent(o.str());
              const size_t len = sContent.length();
              const size_t sent = connection.Write(sContent.c_str(), len);
              if (sent != len) {
                gLog<<"cHTTP::SendRequest Sending content, Write FAILED"<<std::endl;
                return;
              }
            } else {

              {
                // Send the start of the content
                std::ostringstream o;
                o<<"Content-Type: multipart/form-data; boundary=AaB03x"<<STR_END;

                // Content length goes here when we send the request

                std::ostringstream contentBegin;
                const std::map<std::string, std::string>& mVariables = request.GetVariables();
                if (!mVariables.empty()) {
                  // Add the values for this request
                  std::map<std::string, std::string>::const_iterator iter = mVariables.begin();
                  const std::map<std::string, std::string>::const_iterator iterEnd = mVariables.end();
                  while (iter != iterEnd) {
                    contentBegin<<"--AaB03x"<<STR_END;
                    contentBegin<<"content-disposition: form-data; name=\""<<iter->first<<"\""<<STR_END;
                    contentBegin<<STR_END;
                    contentBegin<<iter->second<<STR_END;

                    iter++;
                  }
                }
                contentBegin<<"--AaB03x"<<STR_END;
                contentBegin<<"Content-Disposition: form-data; name=\"file\"; filename=\"results.json\""<<STR_END;
                contentBegin<<"Content-Type: application/json"<<STR_END;
                contentBegin<<STR_END;

                // Content goes here when we send the request

                std::ostringstream contentEnd;
                contentEnd<<"--AaB03x--"<<STR_END;
                contentEnd<<STR_END;


                const std::string sContentBegin(contentBegin.str());
                const std::string sContentEnd(contentEnd.str());

                {
                  const size_t nContentLength = sContentBegin.length() + spitfire::filesystem::GetFileSizeBytes(request.file.sFilePath) + sContentEnd.length();

                  o<<"Content-Length: "<<nContentLength<<STR_END;
                  o<<STR_END;

                  // Send the content length
                  const std::string sContentLength(o.str());
                  const size_t len = sContentLength.length();
                  const size_t sent = connection.Write(sContentLength.c_str(), len);
                  if (sent != len) {
                    gLog<<"cHTTP::SendRequest Sending content length, Write FAILED"<<std::endl;
                    return;
                  }
                }


                {
                  // Send the start of the content
                  const size_t len = sContentBegin.length();
                  const size_t sent = connection.Write(sContentBegin.c_str(), len);
                  if (sent != len) {
                    gLog<<"cHTTP::SendRequest Sending begin of content, Write FAILED"<<std::endl;
                    return;
                  }
                }

                {
                  // Send the contents of the file
                  gLog<<"cHTTP::SendRequest Sending content of file"<<std::endl;
                  std::ifstream file;

                  file.open(spitfire::string::ToUTF8(request.file.sFilePath).c_str(), std::ios::in | std::ios::binary);

                  if (!file.good()) {
                      gLog<<"cHTTP::SendRequest File not opened \""<<request.file.sFilePath<<"\""<<std::endl;
                      return;
                  }

                  uint8_t buffer[1024];

                  while (!file.eof()) {
                    gLog<<"cHTTP::SendRequest Reading into buffer"<<std::endl;
                    const size_t nRead = file.readsome((char*)buffer, 1024);
                    if (nRead == 0) {
                      gLog<<"cHTTP::SendRequest Finished reading file"<<std::endl;
                      break;
                    }
                    assert(nRead <= 1024);
                    const size_t len = nRead;
                    const size_t sent = connection.Write((char*)buffer, len);
                    if (sent != len) {
                      gLog<<"cHTTP::SendRequest Sending file contents, Write FAILED"<<std::endl;
                      return;
                    }
                  }
                }

                {
                  // Send the end of the content
                  const size_t len = sContentEnd.length();
                  const size_t sent = connection.Write(sContentEnd.c_str(), len);
                  if (sent != len) {
                    gLog<<"cHTTP::SendRequest Sending end of content, Write FAILED"<<std::endl;
                    return;
                  }
                }
              }
            }
          } else {
            // Write an extra new line so that the server knows this is the end of the request
            connection.Write(STR_END.c_str(), STR_END.length());
            connection.Write(STR_END.c_str(), STR_END.length());
          }
        }


        state = STATE::RECEIVING_HEADER;
        std::string sHeader;
        cConnectionHTTP reader;
        size_t len = reader.ReadHeader(connection);
        if (len == 0) {
          gLog<<"cHTTP::SendRequest ReadHeader FAILED"<<std::endl;
          return;
        }


        gLog<<"cHTTP::SendRequest About to read content"<<std::endl;
        state = STATE::RECEIVING_CONTENT;

        bool bIsText = true;
        if (bIsText) {
          char buffer[STR_LEN - 1];
          std::string sContent;
          do {
            len = reader.ReadContent(connection, buffer, STR_LEN - 1);
            if (len != 0) {
              buffer[len] = 0;
              sContent = buffer;
              gLog<<"Content: "<<sContent<<std::endl;
              listener.OnTextContentReceived(sContent);
            }
          } while (len != 0);
        } else {
          char buffer[STR_LEN - 1];
          do {
            len = reader.ReadContent(connection, buffer, STR_LEN - 1);
            if (len != 0) listener.OnBinaryContentReceived(buffer, len);
          } while (len != 0);
        }

        connection.Close();

        state = STATE::FINISHED;
        gLog<<"cHTTP::SendRequest Finished, returning"<<std::endl;
      }


      #ifdef BUILD_NETWORK_TLS
      // Use the system ca-certificates file
      // Unfortunately this can differ in each distribution.  This worked on Ubuntu
      const std::string ca_certificates_file_path = "/etc/ssl/certs/ca-certificates.crt";

      const size_t MAX_BUF = 4 * 1024; // 4k read buffer

      void DownloadHTTPS(const std::string& url, spitfire::network::http::cRequestListener& listener)
      {
        std::cout<<"DownloadHTTPS \""<<url<<"\""<<std::endl;

        const spitfire::network::cURI uri(spitfire::network::http::URLEncode(url));

        const std::string ip = hostname_lookup_ip(uri.GetHost());

        tcp_connection connection;

        gnutlsmm::client_session session;

        session.init();

        // X509 stuff
        gnutlsmm::certificate_credentials credentials;

        credentials.init();

        // Set the trusted ca file
        credentials.set_x509_trust_file(ca_certificates_file_path.c_str(), GNUTLS_X509_FMT_PEM);

        // Put the x509 credentials to the current session
        session.set_credentials(credentials);

        // Set TLS version and cypher priorities
        // https://gnutls.org/manual/html_node/Priority-Strings.html
        // NOTE: No SSL, only TLS1.2
        // TODO: TLS1.3 didn't seem to work, server dependent?
        //session.set_priority ("NORMAL:+SRP:+SRP-RSA:+SRP-DSS:-DHE-RSA:-VERS-SSL3.0:%SAFE_RENEGOTIATION:%LATEST_RECORD_VERSION", nullptr);
        session.set_priority("SECURE128:+SECURE192:-VERS-ALL:+VERS-TLS1.2:%SAFE_RENEGOTIATION", nullptr);

        // connect to the peer
        connection.connect(ip, uri.GetPort());
        session.set_transport_ptr((gnutls_transport_ptr_t)(ptrdiff_t)connection.get_sd());

        // Perform the TLS handshake
        const int result = session.handshake();
        if (result < 0) {
            throw std::runtime_error("DownloadHTTPS Handshake failed, error " + std::to_string(result));
        }

        std::cout<<"DownloadHTTPS Handshake completed"<<std::endl;

        std::cout<<"DownloadHTTPS Sending HTTP request"<<std::endl;
        const std::string request =
          "GET /" + uri.GetRelativePath() + " HTTP/1.0\r\n"
          "Host: " + uri.GetHost() + "\r\n"
          "\r\n";
        std::cout<<"DownloadHTTPS Request: \""<<request<<"\""<<std::endl;
        session.send(request.c_str(), request.length());

        std::cout<<"DownloadHTTPS Reading response"<<std::endl;

        char buffer[MAX_BUF + 1];

        util::poll_read p(connection.get_sd());

        const int timeout_ms = 2000;

        // Once we start not receiving data we retry 10 times in 100ms and then exit
        size_t no_bytes_retries = 0;
        const size_t max_no_bytes_retries = 10;
        const size_t retries_timeout_ms = 10;

        std::string received_so_far;

        bool reading_headers = true;

        // NOTE: gnutls_record_recv may return GNUTLS_E_PREMATURE_TERMINATION
        // https://lists.gnupg.org/pipermail/gnutls-help/2014-May/003455.html
        // This means the peer has terminated the TLS session using a TCP RST (i.e., called close()).
        // Since gnutls cannot distinguish that termination from an attacker terminating the session it warns you with this error code.

        while (no_bytes_retries < max_no_bytes_retries) {
            // Check if there is already something in the gnutls buffers
            if (session.check_pending() == 0) {
                // There was no gnutls data ready, check the socket
                switch (p.poll(timeout_ms)) {
                    case util::POLL_READ_RESULT::DATA_READY: {
                        // Check if bytes are actually available (Otherwise if we try to read again the gnutls session object goes into a bad state and gnutlsxx throws an exception)
                        if (connection.get_bytes_available() == 0) {
                            //std::cout<<"but no bytes available"<<std::endl;
                            no_bytes_retries++;

                            // Don't hog the CPU
                            spitfire::util::SleepThisThreadMS(retries_timeout_ms);

                            continue;
                        }
                    }
                    case util::POLL_READ_RESULT::ERROR: {
                        break;
                    }
                    case util::POLL_READ_RESULT::TIMED_OUT: {
                        // We hit the 2 second timeout, we are probably done
                        break;
                    }
                }
            }

            const ssize_t result = session.recv(buffer, MAX_BUF);
            if (result == 0) {
                std::cout<<"DownloadHTTPS Peer has closed the TLS connection"<<std::endl;
                break;
            } else if (result < 0) {
                std::cerr<<"DownloadHTTPS Read error: "<<gnutls_strerror_name(result)<<" "<<gnutls_strerror(result)<<std::endl;
                break;
            }

            const size_t bytes_read = result;
            //std::cout << "Received " << bytes_read << " bytes" << std::endl;
            if (reading_headers) {
                received_so_far.append(buffer, bytes_read);

                size_t i = received_so_far.find("\r\n\r\n");
                if (i != std::string::npos) {
                    std::cout<<"DownloadHTTPS Headers received"<<std::endl;

                    // Anything after this was file content
                    i += strlen("\r\n\r\n");

                    // We are now up to the content
                    reading_headers = false;

                    std::cout<<"DownloadHTTPS Reading content"<<std::endl;

                    // Add to the file content
                    listener.OnTextContentReceived(std::string(&received_so_far[i], received_so_far.length() - i));
                }
            } else {
                // Everything else is content
                listener.OnTextContentReceived(std::string(buffer, bytes_read));
            }
        }

        session.bye(GNUTLS_SHUT_RDWR);

        std::cout<<"DownloadHTTPS Finished"<<std::endl;
      }
      #endif // BUILD_NETWORK_TLS
    }
  }
}
