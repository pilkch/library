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


    namespace http
    {
      class cConnectionHTTP
      {
      public:
        cConnectionHTTP();

        size_t ReadHeader(network::cConnectionTCP& connection, std::string& header);
        size_t ReadContent(network::cConnectionTCP& connection, const void* pBuffer, size_t len);

      private:
        void ParseHeader(const char* header);

        static const size_t nBufferLength = 1024;
        char buffer[nBufferLength];
        size_t nBufferRead; // How much of buffer is valid?
      };

      cConnectionHTTP::cConnectionHTTP() :
        nBufferRead(0)
      {
      }

      size_t cConnectionHTTP::ReadHeader(network::cConnectionTCP& connection, std::string& header)
      {
        std::cout<<"cConnectionHTTP::ReadHeader"<<std::endl;
        ASSERT(connection.IsOpen());

        // If this fails we have already read the header
        ASSERT(nBufferRead == 0);

        header = "";

        bool bIsHeaderRead = false;
        size_t len = 0;
        char szHeaderBuffer[nBufferLength];
        while (!bIsHeaderRead) {
          if (!connection.IsOpen()) {
            std::cout<<"cConnectionHTTP::ReadHeader Connection is closed, breaking"<<std::endl;
            break;
          }

          std::cout<<"cConnectionHTTP::ReadHeader Reading"<<std::endl;
          len = connection.Recv(szHeaderBuffer, nBufferLength);
          if (len == 0) {
            std::cout<<"cConnectionHTTP::ReadHeader Read 0 bytes, breaking"<<std::endl;
            break;
          }

          szHeaderBuffer[len] = 0;
          std::cout<<"cConnectionHTTP::ReadHeader Read into buffer szHeaderBuffer=\""<<szHeaderBuffer<<"\""<<std::endl;

          header += string::StripAfterInclusive(szHeaderBuffer, "\n\n");
          std::cout<<"cConnectionHTTP::ReadHeader Read into buffer header=\""<<header<<"\""<<std::endl;
        }

        std::cout<<"cConnectionHTTP::ReadHeader header=\""<<header<<"\", returning"<<std::endl;
        return header.length();
      }

      size_t cConnectionHTTP::ReadContent(network::cConnectionTCP& connection, const void* pBuffer, size_t len)
      {
        ASSERT(connection.IsOpen());

        // TODO: Read from buffer first and then read from socket

        return 0;
      }






      // *** cDownloadHTTP

      std::string cDownloadHTTP::CreateRequest() const
      {
        std::ostringstream o;

        if (method == METHOD::GET) o<<"GET";
        else o<<"POST";

        std::cout<<"path=\""<<uri.GetRelativePath()<<"\""<<std::endl;
        o<<" /"<<uri.GetRelativePath()<<" HTTP/1.1" STR_END;
        o<<"Host: "<<uri.GetServer()<< STR_END;
        if (progress != 0) o<<"Range: bytes="<<progress<<"-" STR_END;
        o<<"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)" STR_END;
        o<<"Accept: */*" STR_END;
        o<<"Accept-Language: en-us" STR_END;
        o<<"Connection: Keep-Alive" STR_END;
        o<<STR_END;

        if (method == METHOD::POST) {
          const size_t content_length = 10;

          o<<"Content-Type: application/x-www-form-urlencoded" STR_END;
          o<<"Content-Length: "<<content_length<<STR_END;
          o<<STR_END;
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
        std::cout<<"cDownloadHTTP::ThreadFunction "<<uri.GetServer()<<" "<<uri.GetRelativePath()<<std::endl;

        state = STATE::BEFORE_DOWNLOADING;
        content = "";

        if (!uri.IsValidServer()) {
          state = STATE::INVALID_URI;
          return;
        }

        breathe::network::cConnectionTCP connection;

        state = STATE::CONNECTING;
        connection.Open(uri.GetServer(), 80);

        if (!connection.IsOpen()) {
          state = STATE::CONNECTION_FAILED;
          return;
        }

        state = STATE::SENDING_REQUEST;
        {
          // Send header
          std::string request(CreateRequest());
          size_t len = request.length() + 1;
          size_t sent = connection.Send(request.data(), len);
          if (sent != len) {
            LOG<<"cDownloadHTTP::ThreadFunction SDLNet_TCP_Send FAILED "<<SDLNet_GetError()<<std::endl;
            return;
          }
        }


        state = STATE::RECEIVING_HEADER;
        std::string sHeader;
        cConnectionHTTP reader;
        size_t len = reader.ReadHeader(connection, sHeader);
        if (len == 0) {
          LOG<<"cDownloadHTTP::ThreadFunction ReadHeader FAILED "<<SDLNet_GetError()<<std::endl;
          return;
        }


        state = STATE::RECEIVING_CONTENT;
        char buffer[STR_LEN - 1];
        do {
          len = reader.ReadContent(connection, buffer, STR_LEN - 1);
          if (len > 0) {
            buffer[len] = 0;
            content += buffer;
          }
        } while (len > 0);

        LOG<<"CONTENT"<<std::endl;
        LOG<<content<<std::endl;

        connection.Close();

        state = STATE::FINISHED;
        std::cout<<"cDownloadHTTP::ThreadFunction Finished, returning"<<std::endl;
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
}
