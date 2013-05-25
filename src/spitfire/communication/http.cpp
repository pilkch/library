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

// Boost headers
#include <boost/asio.hpp>

// Other libraries
#ifdef WIN32
#include <windows.h>
#endif

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>
#include <spitfire/util/log.h>
#include <spitfire/util/thread.h>

#include <spitfire/storage/filesystem.h>

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
      LOG<<"GetMimeTypeFromExtension Mime type not known for file type \""<<sExtension<<"\""<<std::endl;
      bServeInline = false;
      return "application/octet-stream";
    }


    namespace http
    {
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


      // ** cRequest

      void cRequest::AddPostFileFromPath(const std::string& _sName, const string_t& _sFilePath)
      {
        file.sName = _sName;
        file.sFilePath = _sFilePath;
      }

      std::string cRequest::CreateVariablesString() const
      {
        std::ostringstream o;

        std::map<std::string, std::string>::const_iterator iter = mValues.begin();
        const std::map<std::string, std::string>::const_iterator iterEnd = mValues.end();
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

        std::string sRelativeURIWithAnyVariables = spitfire::string::ToUTF8(sPath);
        if (method == METHOD::GET) {
          const std::string sVariables(CreateVariablesString());
          sRelativeURIWithAnyVariables += "?" + sVariables;
        }

        o<<" "<<sRelativeURIWithAnyVariables<<" HTTP/1.1"<<STR_END;

        // TODO: Use ports other than 80 when required
        //if (uri.GetPort() == 80) o<<"Host: "<<uri.GetServer()<<STR_END;
        //else o<<"Host: "<<uri.GetServer()<<":"<<uri.GetPort()<<STR_END;
        o<<"Host: "<<spitfire::string::ToUTF8(sHost)<<STR_END;

        if (nOffsetBytes != 0) o<<"Range: bytes="<<nOffsetBytes<<"-"<<STR_END;

        o<<"User-Agent: Mozilla/4.0 (compatible; Spitfire 1.0; Linux)"<<STR_END;
        o<<"Accept: */*"<<STR_END;
        o<<"Accept-Language: en-us"<<STR_END;

        //o<<"Connection: Keep-Alive"<<STR_END;
        //OR
        o<<"Connection: close"<<STR_END;

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


      size_t cConnectionHTTP::ReadHeader(network::cConnectionTCP& connection)
      {
        LOG<<"cConnectionHTTP::ReadHeader"<<std::endl;
        ASSERT(connection.IsOpen());

        // If this fails we have already read the header
        ASSERT(content.empty());

        size_t len = 0;
        char szHeaderBuffer[nBufferLength + 1];
        //LOG<<"cConnectionHTTP::ReadHeader About to start reading stuff"<<std::endl;
        while (connection.IsOpen()) {
          //LOG<<"cConnectionHTTP::ReadHeader Reading"<<std::endl;
          len = connection.Recv(szHeaderBuffer, nBufferLength, 5000);
          //LOG<<"cConnectionHTTP::ReadHeader Recv has finished"<<std::endl;
          if (len == 0) {
            LOG<<"cConnectionHTTP::ReadHeader Read 0 bytes, breaking"<<std::endl;
            break;
          }

          //LOG<<"cConnectionHTTP::ReadHeader Terminating string"<<std::endl;
          szHeaderBuffer[len] = 0;
          //LOG<<"cConnectionHTTP::ReadHeader Read "<<len<<" bytes into buffer szHeaderBuffer=\""<<szHeaderBuffer<<"\""<<std::endl;

          std::string sBuffer(szHeaderBuffer);
          std::string::size_type i = sBuffer.find("\r\n\r\n");

          if (i == std::string::npos) {
            header += sBuffer;
          } else {
            header += sBuffer.substr(0, i);
            //LOG<<"cConnectionHTTP::ReadHeader Read into buffer header=\""<<header<<"\""<<std::endl;

            // Skip "\r\n\r\n"
            i += 4;

            ASSERT(len >= i);
            const size_t nBufferRead = len - i;

            if (nBufferRead != 0) {
              // There is something to put in the content buffer so fill it up and return

              // Make space to append the next chunk to the end of our current header buffer
              //LOG<<"cConnectionHTTP::ReadHeader Content reserving  "<<nBufferRead<<" bytes"<<std::endl;
              content.reserve(nBufferRead);

              // Append to the header
              //LOG<<"cConnectionHTTP::ReadHeader Content inserting  "<<nBufferRead<<" bytes"<<std::endl;
              content.insert(content.begin(), nBufferRead, '\0');
              //LOG<<"cConnectionHTTP::ReadHeader Content copying "<<nBufferRead<<" bytes"<<std::endl;
              memcpy(&content[0], &szHeaderBuffer[i], nBufferRead);
              break;
            }
          }
        }

        // Actually parse the header into status, key value pairs etc.
        ParseHeader();

        LOG<<"cConnectionHTTP::ReadHeader returning"<<std::endl;
        return header.length();
      }

      size_t cConnectionHTTP::ReadContent(network::cConnectionTCP& connection, void* pOutContent, size_t len)
      {
        LOG<<"cConnectionHTTP::ReadContent len="<<len<<std::endl;

        size_t nContentReadThisTimeAround = 0;

        // If we already have content data then read from there first
        const size_t nBufferRead = content.size();
        //LOG<<"cConnectionHTTP::ReadContent nBufferRead="<<nBufferRead<<std::endl;
        if (nBufferRead != 0) {
          //LOG<<"cConnectionHTTP::ReadContent Reading "<<nBufferRead<<" previous bytes"<<std::endl;
          const size_t smaller = min(nBufferRead, len);
          //LOG<<"cConnectionHTTP::ReadContent Actually reading "<<smaller<<" previous bytes"<<std::endl;
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
            LOG<<"cConnectionHTTP::ReadContent Connection is closed, returning "<<nContentReadThisTimeAround<<std::endl;
            return nContentReadThisTimeAround;
          }

          // Now read the rest from the connection
          const size_t n = connection.Recv(pOutContent, len, 2000);
          //LOG<<"cConnectionHTTP::ReadContent nBufferRead="<<nBufferRead<<" n="<<n<<std::endl;
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

        return "";
      }

      size_t cConnectionHTTP::GetContentLength() const
      {
        std::map<std::string, std::string>::const_iterator iter = headerValues.find("Content-Length");
        if (iter != headerValues.end()) {
          return string::ToUnsignedInt(string::ToString_t(iter->second));
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




      // *** cHTTP

      void cHTTP::SendRequest(const cRequest& request, cRequestListener& listener) const
      {
        LOG<<"cHTTP::SendRequest"<<std::endl;

        // Start downloading at the beginning
        //uint32_t progress = 0;

        //METHOD method = request.IsMethodGet() ? METHOD::GET : METHOD::POST;

        STATE state = STATE::BEFORE_DOWNLOADING;
        (void)state; // Avoid warning about unused variable

        breathe::network::cConnectionTCP connection;

        state = STATE::CONNECTING;
        connection.Open(spitfire::string::ToUTF8(request.GetHost()), 80);

        if (!connection.IsOpen()) {
          LOG<<"cHTTP::SendRequest Connection failed, returning"<<std::endl;
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
            const size_t sent = connection.Send(sRequestHeader.c_str(), len);
            if (sent != len) {
              LOG<<"cHTTP::SendRequest Sending request, Send FAILED"<<std::endl;
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
              const size_t sent = connection.Send(sContent.c_str(), len);
              if (sent != len) {
                LOG<<"cHTTP::SendRequest Sending content, Send FAILED"<<std::endl;
                return;
              }
            } else {

              {
                // Send the start of the content
                std::ostringstream o;
                o<<"Content-Type: multipart/form-data; boundary=AaB03x"<<STR_END;

                // Content length goes here when we send the request

                std::ostringstream contentBegin;
                const std::map<std::string, std::string>& mValues = request.GetValues();
                if (!mValues.empty()) {
                  // Add the values for this request
                  std::map<std::string, std::string>::const_iterator iter = mValues.begin();
                  const std::map<std::string, std::string>::const_iterator iterEnd = mValues.end();
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
                  const size_t sent = connection.Send(sContentLength.c_str(), len);
                  if (sent != len) {
                    LOG<<"cHTTP::SendRequest Sending content length, Send FAILED"<<std::endl;
                    return;
                  }
                }


                {
                  // Send the start of the content
                  const size_t len = sContentBegin.length();
                  const size_t sent = connection.Send(sContentBegin.c_str(), len);
                  if (sent != len) {
                    LOG<<"cHTTP::SendRequest Sending begin of content, Send FAILED"<<std::endl;
                    return;
                  }
                }

                {
                  // Send the contents of the file
                  LOG<<"cHTTP::SendRequest Sending content of file"<<std::endl;
                  std::ifstream file;

                  file.open(spitfire::string::ToUTF8(request.file.sFilePath).c_str(), std::ios::in | std::ios::binary);

                  if (!file.good()) {
                      LOG<<"cHTTP::SendRequest File not opened \""<<request.file.sFilePath<<"\""<<std::endl;
                      return;
                  }

                  uint8_t buffer[1024];

                  while (!file.eof()) {
                    LOG<<"cHTTP::SendRequest Reading into buffer"<<std::endl;
                    const size_t nRead = file.readsome((char*)buffer, 1024);
                    if (nRead == 0) {
                      LOG<<"cHTTP::SendRequest Finished reading file"<<std::endl;
                      break;
                    }
                    assert(nRead <= 1024);
                    const size_t len = nRead;
                    const size_t sent = connection.Send((char*)buffer, len);
                    if (sent != len) {
                      LOG<<"cHTTP::SendRequest Sending file contents, Send FAILED"<<std::endl;
                      return;
                    }
                  }
                }

                {
                  // Send the end of the content
                  const size_t len = sContentEnd.length();
                  const size_t sent = connection.Send(sContentEnd.c_str(), len);
                  if (sent != len) {
                    LOG<<"cHTTP::SendRequest Sending end of content, Send FAILED"<<std::endl;
                    return;
                  }
                }
              }
            }
          }
        }


        state = STATE::RECEIVING_HEADER;
        std::string sHeader;
        cConnectionHTTP reader;
        size_t len = reader.ReadHeader(connection);
        if (len == 0) {
          LOG<<"cHTTP::SendRequest ReadHeader FAILED"<<std::endl;
          return;
        }


        LOG<<"cHTTP::SendRequest About to read content"<<std::endl;
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
              //LOG<<"Content: "<<sContent<<std::endl;
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
        LOG<<"cHTTP::SendRequest Finished, returning"<<std::endl;
      }
    }
  }
}
