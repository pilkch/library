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

#include <spitfire/communication/network.h>
#include <spitfire/communication/uri.h>
#include <spitfire/communication/http.h>

// Breathe headers
#include <breathe/breathe.h>


const size_t STR_LEN = 512;
const std::string STR_END = "\r\n";

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


    namespace http
    {
      class cRequest
      {
      public:
        cRequest();

        void SetMethod(METHOD method);
        void SetPath(const string_t& sPath);
        void SetOffsetBytes(size_t nOffsetBytes);

      private:
        METHOD method;
        string_t sPath;
        size_t nOffsetBytes;
      };

      cRequest::cRequest() :
        method(METHOD::GET),
        sPath(TEXT("/")),
        nOffsetBytes(0)
      {
      }

      class cResponse
      {
      };

      class cHTTP
      {
      public:
        cHTTP();

        void SetHostName(const string_t& sHostName);

        void SetRequest(const cRequest& request, cResponse& response);

        void Run();
      };




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

      private:
        void ParseHeader();

        std::string Decode(const std::string& encodedString);
        std::string Encode(const std::string& rawString);

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
        headerValues.clear();


        std::vector<std::string> vHeader;
        string::SplitOnNewLines(header, vHeader);

        if (vHeader.empty()) return;

        // HTTP/1.1 200 OK
        {
          // Split into "HTTP/1.1", "200", "OK"
          std::vector<std::string> vParts;
          string::SplitOnNewLines(vHeader[0], vParts);

          if (vParts.size() >= 3) {
            status = string::ToInt(string::ToString_t(vParts[1]));
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

      std::string cConnectionHTTP::Decode(const std::string& encodedString)
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

      std::string cConnectionHTTP::Encode(const std::string& rawString)
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



      size_t cConnectionHTTP::ReadHeader(network::cConnectionTCP& connection)
      {
        LOG<<"cConnectionHTTP::ReadHeader"<<std::endl;
        ASSERT(connection.IsOpen());

        // If this fails we have already read the header
        ASSERT(content.empty());

        size_t len = 0;
        char szHeaderBuffer[nBufferLength + 1];
        LOG<<"cConnectionHTTP::ReadHeader About to start reading stuff"<<std::endl;
        while (connection.IsOpen()) {
          LOG<<"cConnectionHTTP::ReadHeader Reading"<<std::endl;
          len = connection.Recv(szHeaderBuffer, nBufferLength, 5000);
          LOG<<"cConnectionHTTP::ReadHeader Recv has finished"<<std::endl;
          if (len == 0) {
            LOG<<"cConnectionHTTP::ReadHeader Read 0 bytes, breaking"<<std::endl;
            break;
          }

          LOG<<"cConnectionHTTP::ReadHeader Terminating string"<<std::endl;
          szHeaderBuffer[len] = 0;
          LOG<<"cConnectionHTTP::ReadHeader Read "<<len<<" bytes into buffer szHeaderBuffer=\""<<szHeaderBuffer<<"\""<<std::endl;

          std::string sBuffer(szHeaderBuffer);
          std::string::size_type i = sBuffer.find("\r\n\r\n");

          if (i == std::string::npos) {
            header += sBuffer;
          } else {
            header += sBuffer.substr(0, i);
            LOG<<"cConnectionHTTP::ReadHeader Read into buffer header=\""<<header<<"\""<<std::endl;

            // Skip "\r\n\r\n"
            i += 4;

            ASSERT(len >= i);
            const size_t nBufferRead = len - i;

            if (nBufferRead != 0) {
              // There is something to put in the content buffer so fill it up and return

              // Make space to append the next chunk to the end of our current header buffer
              LOG<<"cConnectionHTTP::ReadHeader Content reserving  "<<nBufferRead<<" bytes"<<std::endl;
              content.reserve(nBufferRead);

              // Append to the header
              LOG<<"cConnectionHTTP::ReadHeader Content inserting  "<<nBufferRead<<" bytes"<<std::endl;
              content.insert(content.begin(), nBufferRead, '\0');
              LOG<<"cConnectionHTTP::ReadHeader Content copying "<<nBufferRead<<" bytes"<<std::endl;
              memcpy(&content[0], &szHeaderBuffer[i], nBufferRead);
              break;
            }
          }
        }

        // Actually parse the header into status, key value pairs etc.
        ParseHeader();

        LOG<<"cConnectionHTTP::ReadHeader header=\""<<header<<"\", returning"<<std::endl;
        return header.length();
      }

      size_t cConnectionHTTP::ReadContent(network::cConnectionTCP& connection, void* pOutContent, size_t len)
      {
        LOG<<"cConnectionHTTP::ReadContent len="<<len<<std::endl;
        ASSERT(connection.IsOpen());

        size_t nContentReadThisTimeAround = 0;

        // If we already have content data then read from there first
        const size_t nBufferRead = content.size();
        LOG<<"cConnectionHTTP::ReadContent nBufferRead="<<nBufferRead<<std::endl;
        if (nBufferRead != 0) {
          LOG<<"cConnectionHTTP::ReadContent Reading "<<nBufferRead<<" previous bytes"<<std::endl;
          const size_t smaller = min(nBufferRead, len);
          LOG<<"cConnectionHTTP::ReadContent Actually reading "<<smaller<<" previous bytes"<<std::endl;
          memcpy(pOutContent, &content[0], smaller);

          // Increment our buffer
          pOutContent = (char*)pOutContent + smaller;
          len -= smaller;


          std::vector<char> temp(content.begin() + smaller, content.end());
          content = temp;
          ASSERT(content.size() + smaller == nBufferRead);

          nContentReadThisTimeAround += smaller;
        }

        if (len != 0) {
          // Now read the rest from the connection
          const size_t n = connection.Recv(pOutContent, len, 2000);
          LOG<<"cConnectionHTTP::ReadContent nBufferRead="<<nBufferRead<<" n="<<n<<std::endl;
          nContentReadThisTimeAround += n;
        }

        // TODO:
        // if (IsTransferEncodingChunked()) {
        //   ... begins with "\r\n25\r\n"
        //
        //   ... ends in "\r\n0\r\n"
        // }

        LOG<<"cConnectionHTTP::ReadContent nContentReadThisTimeAround="<<nContentReadThisTimeAround<<std::endl;
        return nContentReadThisTimeAround;
      }



      std::string cConnectionHTTP::GetContentType() const
      {
        std::map<std::string, std::string>::const_iterator iter = headerValues.find("Content-Type");
        if (iter != headerValues.end()) {
          return iter->second;
        }

        return false;
      }

      size_t cConnectionHTTP::GetContentLength() const
      {
        std::map<std::string, std::string>::const_iterator iter = headerValues.find("Content-Length");
        if (iter != headerValues.end()) {
          return string::ToUnsignedInt(string::ToString_t(iter->second));
        }

        return 0;
      }


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
        std::map<std::string, std::string>::const_iterator iter = headerValues.find("Transfer-Encoding");
        if (iter != headerValues.end()) {
          return (iter->second == "chunked");
        }

        return false;
      }






      // *** cDownloadHTTP

      std::string cDownloadHTTP::CreateRequest() const
      {
        std::ostringstream o;

        if (method == METHOD::GET) o<<"GET";
        else o<<"POST";

        o<<" /"<<uri.GetRelativePath()<<" HTTP/1.1"<<STR_END;

        if (uri.GetPort() == 80) o<<"Host: "<<uri.GetServer()<<STR_END;
        else o<<"Host: "<<uri.GetServer()<<":"<<uri.GetPort()<<STR_END;

        if (progress != 0) o<<"Range: bytes="<<progress<<"-"<<STR_END;

        o<<"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)"<<STR_END;
        o<<"Accept: */*"<<STR_END;
        o<<"Accept-Language: en-us"<<STR_END;

        //o<<"Connection: Keep-Alive"<<STR_END;
        //OR
        o<<"Connection: close"<<STR_END;

        o<<STR_END;

        if (method == METHOD::POST) {
          // TODO: Obviously this is incorrect we should actually get this value from somewhere
          const size_t content_length = 10;
          LOG<<"cDownloadHTTP::CreateRequest POST is not complete"<<std::endl;
          ASSERT(false);

          o<<"Content-Type: application/x-www-form-urlencoded"<<STR_END;
          o<<"Content-Length: "<<content_length<<STR_END;
          o<<STR_END;
        }

        o<<STR_END;

        return o.str();
      }


      void cDownloadHTTP::Download(const std::string& full_uri, METHOD _method, cDownloadListener& listener)
      {
        LOG<<"cDownloadHTTP::Download \""<<full_uri<<"\""<<std::endl;

        // Parse the uri
        uri.Parse(full_uri);

        // Start downloading at the beginning
        progress = 0;

        method = _method;


        state = STATE::BEFORE_DOWNLOADING;

        if (!uri.IsValidServer()) {
          LOG<<"cDownloadHTTP::Download Invalid server in uri, returning"<<""<<std::endl;
          state = STATE::INVALID_URI;
          return;
        }

        breathe::network::cConnectionTCP connection;

        state = STATE::CONNECTING;
        connection.Open(uri.GetServer(), 80);

        if (!connection.IsOpen()) {
          LOG<<"cDownloadHTTP::Download Connection failed, returning"<<""<<std::endl;
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
            LOG<<"cDownloadHTTP::Download SDLNet_TCP_Send FAILED "<<SDLNet_GetError()<<std::endl;
            return;
          }
        }


        state = STATE::RECEIVING_HEADER;
        std::string sHeader;
        cConnectionHTTP reader;
        size_t len = reader.ReadHeader(connection);
        if (len == 0) {
          LOG<<"cDownloadHTTP::Download ReadHeader FAILED "<<SDLNet_GetError()<<std::endl;
          return;
        }


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
              listener.OnTextContentReceived(sContent);
            }
          } while (len != 0);
        } else {
          assert(false);
          char buffer[STR_LEN - 1];
          do {
            std::string sContent;
            len = reader.ReadContent(connection, buffer, STR_LEN - 1);
            if (len != 0) {
              buffer[len] = 0;
              sContent = buffer;
              //listener.OnBinaryContentReceived(sContent);
            }
          } while (len != 0);
        }

        connection.Close();

        state = STATE::FINISHED;
        LOG<<"cDownloadHTTP::Download Finished, returning"<<std::endl;
      }
    }
  }
}
