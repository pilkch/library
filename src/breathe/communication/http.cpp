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

// Boost headers
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

// Other libraries
#ifdef WIN32
#include <windows.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/thread.h>

#include <spitfire/math/math.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/communication/network.h>
#include <breathe/communication/uri.h>
#include <breathe/communication/http.h>


const size_t STR_LEN = 512;
#define STR_END "\r\n"

namespace breathe
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


    // *** cDownloadHTTP

    std::string cDownloadHTTP::CreateRequest() const
    {
      std::ostringstream o;

      if (method == METHOD_GET) o<<"GET";
      else o<<"POST";

      o<<" /"<<uri.GetRelativePath()<<" HTTP/1.1" STR_END;

      o<<"Host: "<<uri.GetServer()<< STR_END;
      o<<"Range: bytes="<<progress<<"-" STR_END;
      o<<"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)" STR_END;
      o<<"Accept: */*" STR_END;
      o<<"Accept-Language: en-us" STR_END;
      o<<"Connection: Keep-Alive" STR_END;
      o<<STR_END;

      if (method == METHOD_POST) {
        const size_t content_length = 10;

        o<<"Content-Type: application/x-www-form-urlencoded" STR_END;
        o<<"Content-Length: "<<content_length<<STR_END;
        o<<STR_END;
        o<<"home=Cosby&favorite+flavor=flies"<<STR_END;
      }

      o<<STR_END;

      return o.str();

   /*   char header[STR_LEN];
      header[0] = 0;

      sprintf(header,
      "GET /%s HTTP/1.1" STR_END
      "Host: %s" STR_END
      "Range: bytes=%ld-" STR_END
      "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)" STR_END*/
  //   "Accept: */*" STR_END
  /*   "Accept-Language: en-us" STR_END
      "Connection: Keep-Alive" STR_END
      "" STR_END

      , path.c_str(), server.c_str(), progress);

      request = std::string(header);*/
    }

    void cDownloadHTTP::ThreadFunction()
    {
      std::cout<<uri.GetServer()<<" "<<uri.GetRelativePath()<<std::endl;
      content = "";

      connection.Open(uri.GetServer(), 80);

      {
        // Send header
        std::string request(CreateRequest());
        size_t len = request.length() + 1;
        size_t sent = connection.Send(request.data(), len);
        if (sent != len) {
          LOG<<"SDLNet_TCP_Send: "<<SDLNet_GetError()<<std::endl;
          exit(EXIT_FAILURE);
        }
      }

      size_t len = 0;

      char buffer[STR_LEN - 1];
      buffer[0] = 0;

      do {
        len = connection.Recv(buffer, STR_LEN - 1);
        if (len == 0) break;

        buffer[len] = 0;

        ParseHeader(buffer);

        len = connection.Recv(buffer, STR_LEN - 1);
        if (len > 0) {
          buffer[len] = 0;
          content += buffer;
        }
      } while (len > 0);

      LOG<<"CONTENT"<<std::endl;
      LOG<<content<<std::endl;

      connection.Close();
    }

    std::string cDownloadHTTP::Decode(const std::string& encodedString)
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
          if (tmpStr != NULL) {
            decodedString.append(tmpStr, cnt);
            tmpStr = NULL;
            cnt = 0;
          }
          decodedString += ' ';
        } else if ('%' == curChar) {
          if (tmpStr != NULL) {
            decodedString.append(tmpStr, cnt);
            tmpStr = NULL;
            cnt = 0;
          }

          if ((i + 2 < encodedLen) && string::IsHexDigit(encStr[i + 1]) && string::IsHexDigit(encStr[i + 2])) {
            char s[3];
            s[0] = encStr[i++];
            s[1] = encStr[i++];
            s[0] = 0;
            uint32_t value = breathe::string::FromHexStringToUint32_t(s);
            decodedString += static_cast<char>(value);
          } else {
            LOG<<"cHTTP::Decode invalid %-escapes in " + encodedString;
            // TODO: What do we do now?
          }
        } else {
          if (cnt == 0) tmpStr = encStr + i;
          ++cnt;
        }
      }
      if (tmpStr != NULL) {
        decodedString.append(tmpStr, cnt);
        cnt = 0;
        tmpStr = NULL;
      }

      return decodedString;
    }

    bool IsSpecialCharacter(char c)
    {
      return false;
    }

    std::string cDownloadHTTP::Encode(const std::string& rawString)
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

    /*void cDownloadHTTP::Download(const std::string& _path)
    {
      path = _path;
      server = breathe::string::StripAfterInclusive(breathe::string::StripLeading(path, "http://"), "/");
      path = breathe::string::StripBeforeInclusive(_path, server + "/");
      if (path.length() < 1) path = "/";

      Run();
    }*/

    void cDownloadHTTP::ParseHeader(const char* szHeader)
    {
      ASSERT(false);
    }
  }
}
