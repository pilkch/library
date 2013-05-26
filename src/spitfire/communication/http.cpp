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
        const char c = sp.GetCharacter();
        if ((c == '\r') || (c == '\n')) return false;

        // Parse path
        if (!sp.GetToStringAndSkip(": ", sKey)) {
          LOG<<"ParseRequest Couldn't parse key, returning false"<<std::endl;
          return false;
        }

        // Get to the end of the line
        return sp.GetToStringAndSkip("\n", sValue);
      }

      bool ParseRequest(cRequest& request, const std::string& sRequest)
      {
        request.Clear();

        string::cStringParserUTF8 sp(sRequest);

        if (sp.IsEnd()) {
          LOG<<"ParseRequest Empty string, returning false"<<std::endl;
          return false;
        }

        std::string sValue;

        // Parse method
        if (!sp.GetToWhiteSpaceAndSkip(sValue)) {
          LOG<<"ParseRequest Couldn't parse method, returning false"<<std::endl;
          return false;
        }

        if (sValue == "POST") request.SetMethodPost();
        else request.SetMethodGet();

        // Parse path
        if (!sp.GetToWhiteSpaceAndSkip(sValue)) {
          LOG<<"ParseRequest Couldn't parse path, returning false"<<std::endl;
          return false;
        }

        request.SetPath(sValue);

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
            request.AddValue(sKey, sValue);
          }
        }

        return true;
      }



      // ** cRequest

      cResponse::cResponse() :
        status(STATUS::OK),
        nContentLengthBytes(0),
        iExpires(-1),
        bCacheControlPrivateMaxAgeZero(true),
        bCloseConnection(true)
      {
        SetContentTypeTextHTMLUTF8();
      }

      void cResponse::SetStatus(STATUS _status)
      {
        status = _status;
      }

      void cResponse::SetContentLengthBytes(size_t _nContentLengthBytes)
      {
        nContentLengthBytes = _nContentLengthBytes;
      }

      void cResponse::SetContentMimeType(const std::string& _sMimeType)
      {
        sMimeType = _sMimeType;
      }

      void cResponse::SetContentTypeTextHTMLUTF8()
      {
        sMimeType = "text/html";
      }

      void cResponse::SetDateTimeNow()
      {
      }

      void cResponse::SetExpires(int _iExpires)
      {
        iExpires = _iExpires;
      }

      void cResponse::SetCacheControlPrivateMaxAgeZero()
      {
        bCacheControlPrivateMaxAgeZero = true;
      }

      void cResponse::SetCloseConnection()
      {
        bCloseConnection = true;
      }

      std::string cResponse::ToString() const
      {
        LOG<<"cResponse::ToString mime=\""<<sMimeType<<"\""<<std::endl;

        std::ostringstream o;
        o<<"HTTP/1.1 "<<GetStatusAsString(status)<<" "<<GetStatusDescription(status)<<"\n";
        o<<"Status: "<<GetStatusAsString(status)<<" "<<GetStatusDescription(status)<<"\n";
        o<<"Version: HTTP/1.1\n";

        // Set the content type
        if (spitfire::string::StartsWith(sMimeType, "text/")) o<<"Content-type: "<<sMimeType<<"; charset=UTF-8\n";
        else o<<"Content-type: "<<sMimeType<<"\n";

        if (bCacheControlPrivateMaxAgeZero) o<<"Cache-control: private, max-age=0\n";
        o<<"Date: Fri, 24 May 2013 10:15:39 GMT\n";
        o<<"Expires: "<<iExpires<<"\n";
        //o<<"Server: Apache 1.0 (Unix)\n";
        //o<<"x-frame-options: SAMEORIGIN\n";
        //o<<"x-xss-protection: 1; mode=block\n";
        o<<"Content-Length: "<<nContentLengthBytes<<"\n";
        o<<"Connection: close\n\n";

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





      // ** cServerEvent

      cServerEvent::cServerEvent() :
        type(SERVER_EVENT_TYPE::UNKNOWN),
        pConnectedClient(nullptr)
      {
      }


      // ** cConnectedClient

      cConnectedClient::cConnectedClient(boost::asio::io_service& _socket) :
        socket(_socket),
        bIsRunning(false)
      {
      }

      bool cConnectedClient::IsRunning() const
      {
        return bIsRunning;
      }

      void cConnectedClient::Run(cServer& server)
      {
        LOG<<"cConnectedClient::Run"<<std::endl;

        bIsRunning = true;

        server.RunClientConnection(*this);

        server.OnClientConnectionFinished(*this);

        bIsRunning = false;

        LOG<<"cConnectedClient::Run returning"<<std::endl;
      }

      size_t cConnectedClient::GetBytesToRead()
      {
        // Check if there are any bytes available
        boost::asio::socket_base::bytes_readable command(true);

        socket.io_control(command);

        return command.get();
      }

      size_t cConnectedClient::GetBytesAvailable()
      {
        return socket.available();
      }

      size_t cConnectedClient::Read(uint8_t* pBuffer, size_t nBufferSize)
      {
        return socket.receive(boost::asio::buffer(pBuffer, nBufferSize));
      }

      void cConnectedClient::Write(const uint8_t* pBuffer, size_t nBufferSize)
      {
        boost::asio::write(socket, boost::asio::buffer(pBuffer, nBufferSize), boost::asio::transfer_all());
      }

      void cConnectedClient::Write(const std::string& sData)
      {
        /*message = sData;

        boost::asio::async_write(socket, boost::asio::buffer(message),
            boost::bind(&cConnectedClient::WriteCallback, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));*/

        boost::asio::write(socket, boost::asio::buffer(sData), boost::asio::transfer_all());
      }

      void cConnectedClient::SendResponse(const cResponse& response)
      {
        Write(response.ToString());
      }

      void cConnectedClient::SendContent(const std::string& sContentUTF8)
      {
        Write(sContentUTF8);
      }


      // ** cServer

      cServer::cServer() :
        util::cThread(soStop, "cServer"),
        soStop("soStop"),
        pRequestHandler(nullptr),
        soEvent("soEvent"),
        mutexEventQueue("mutexEventQueue")
      {
      }

      void cServer::OnConnectedClient(cConnectedClient* pNewConnection)
      {
        LOG<<"cServer::OnConnectedClient New connection started"<<std::endl;

        ASSERT(pNewConnection != nullptr);

        clients.push_back(pNewConnection);

        // Start the connection thread
        pNewConnection->Run(*this);
      }

      void cServer::ServeError404(cConnectedClient& connection, const cRequest& request)
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
        response.SetDateTimeNow();

        connection.SendResponse(response);
        connection.SendContent(sContentUTF8);
      }

      void cServer::ServeError(cConnectedClient& connection, const cRequest& request, STATUS status)
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
        response.SetDateTimeNow();

        connection.SendResponse(response);
        connection.SendContent(sContentUTF8);
        connection.SendContent("\n\n");
      }

      void cServer::ServeFile(cConnectedClient& connection, const cRequest& request, const string_t& sMimeTypeUTF8, const string_t& sRelativeFilePath)
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
        response.SetDateTimeNow();
        response.SetExpires(-1);
        response.SetCacheControlPrivateMaxAgeZero();
        response.SetCloseConnection();

        connection.SendResponse(response);

        const size_t nBufferSizeBytes = 1024;

        uint8_t buffer[nBufferSizeBytes];

        while (file.IsOpen()) {
          const size_t nRead = file.Read(buffer, nBufferSizeBytes);
          if (nRead == 0) break;

          connection.Write(buffer, nRead);
        }

        connection.Write("\n\n");
      }


      void cServer::ServeFile(cConnectedClient& connection, const cRequest& request)
      {
        std::string sRelativeFilePath = string::StripLeading(request.GetPath(), "/");

        bool bIsValidRelativeFilePath = true;

        // Check if the path is trying to be tricky
        if ((string::CountOccurrences(sRelativeFilePath, "./") != 0) || (string::CountOccurrences(sRelativeFilePath, "..") != 0)) {
          LOG<<"cServer::ServeFile Request path is a compressed path, returning 404"<<std::endl;
          bIsValidRelativeFilePath = false;
        } else if (!filesystem::FileExists(sRelativeFilePath)) {
          LOG<<"cServer::ServeFile Request path \""<<sRelativeFilePath<<"\" was not found, returning 404"<<std::endl;
          bIsValidRelativeFilePath = false;
        }

        if (!bIsValidRelativeFilePath) {
          ServeError404(connection, request);
        } else {
          // Add index.html if it is a folder
          if (filesystem::IsFolder(sRelativeFilePath)) sRelativeFilePath += "index.html";

          bool bServeInline = false;
          const std::string sMimeTypeUTF8 = GetMimeTypeFromExtension(filesystem::GetExtensionNoDot(sRelativeFilePath), bServeInline);
          ServeFile(connection, request, sMimeTypeUTF8, sRelativeFilePath);
        }
      }

      /*void cServer::ServePage(cConnectedClient& connection, const cRequest& request, const string_t& sMimeTypeUTF8, const string_t& sPageContentUTF8)
      {
        cResponse response;
        response.SetStatus(STATUS::OK);
        response.SetCacheControl();
        response.SetContentType(sMimeTypeUTF8);
        response.SetDateTimeNow();
        response.SetContentLengthBytes(sContentUTF8.length());
        response.SetConnectionClose();

        "HTTP/1.1 " + status + " " + GetStatusDescription(status) +
        "Location: http://" + GetHostName() + "/" +
        "Expires: -1" +
        "Cache-Control: private, max-age=0" +
        "Content-Type: text/html; charset=UTF-8" +
        "Date: Wed, 18 Jul 2012 05:13:40 GMT" +
        "Content-Length: " + nContentLengthBytes +
        "Connection: close"

        connection.SendResponse(response);
        connection.SendContent(sContentUTF8);
      }*/

      void cServer::OnRequestMade(cConnectedClient& connection, const cRequest& request)
      {
        // Ask the request handler to handle this request first
        if (pRequestHandler != nullptr) {
          if (pRequestHandler->HandleRequest(*this, connection, request)) return;
        }

        //ServeFile(client, request);
      }

      bool cServer::_IsToStop() const
      {
        return false;
      }

      void cServer::RunClientConnection(cConnectedClient& connection)
      {
        // Wait for the request to be sent
        sleep(1);

        const size_t bytes_readable = connection.GetBytesToRead();

        std::cout<<"bytes_readable="<<bytes_readable<<std::endl;

        const size_t nDataSize = 512;
        uint8_t data[nDataSize];

        std::vector<uint8_t> buffer;
        buffer.reserve(1024);

        while (true) {
          const size_t nBytesAvailable = connection.GetBytesAvailable();
          if (nBytesAvailable == 0) {
            //LOG<<"cServer::RunClientConnection No bytes available, breaking"<<std::endl;
            break;
          }

          const size_t nBytesRead = connection.Read(data, nDataSize);
          if (nBytesRead == 0) {
            LOG<<"cServer::RunClientConnection No bytes read, breaking"<<std::endl;
            break;
          }

          LOG<<"cServer::RunClientConnection "<<nBytesRead<<" bytes read"<<std::endl;
          const size_t nBufferSize = buffer.size();

          // Resize the read buffer
          buffer.resize(nBufferSize + nBytesRead);

          // Append the data
          memcpy(buffer.data() + nBufferSize, data, nBytesRead);
        };

        if (buffer.empty()) {
          LOG<<"cServer::RunClientConnection No request sent, returning"<<std::endl;
          return;
        }

        const std::string sRequest((const char*)buffer.data());

        //LOG<<"cServer::RunClientConnection request \""<<(const char*)(buffer.data())<<"\""<<std::endl;
        cRequest request;
        ParseRequest(request, sRequest);
        LOG<<"Path "<<request.GetPath()<<std::endl;
        if (string::StartsWith(request.GetPath(), "/data/web/")) {
          ServeFile(connection, request);
        } else {
          static int x = 1;

          const std::string sContentUTF8 = "<html><head><title>Title</title></head><body>Body " + spitfire::string::ToString(x) + "</body></html>\n";

          x++;

          // TODO: Fill out the response from the request
          cResponse response;
          response.SetContentLengthBytes(sContentUTF8.length());
          connection.SendResponse(response);

          // TODO: Send actual content
          connection.SendContent(sContentUTF8);

          connection.Write("\n\n");
        }
      }

      void cServer::OnClientConnectionFinished(cConnectedClient& connection)
      {
        cServerEvent* pEvent = new cServerEvent;
        pEvent->type = SERVER_EVENT_TYPE::CLIENT_CONNECTION_FINISHED;
        pEvent->pConnectedClient = &connection;
        SendEvent(pEvent);
      }

      void cServer::SendEvent(cServerEvent* pEvent)
      {
        spitfire::util::cLockObject lock(mutexEventQueue);

        eventQueue.push_back(pEvent);
      }

      cServerEvent* cServer::GetNextEvent()
      {
        spitfire::util::cLockObject lock(mutexEventQueue);

        cServerEvent* pEvent = eventQueue.front();
        eventQueue.pop_front();
        return pEvent;
      }

      void cServer::ThreadFunction()
      {
        LOG<<"cServer::ThreadFunction"<<std::endl;

        cTCPServer server(*this, 12345);
        server.Run();

        while (true) {
          soEvent.WaitForever();

          cServerEvent* pEvent = GetNextEvent();
          if (pEvent != nullptr) {
            if (pEvent->type == SERVER_EVENT_TYPE::CLIENT_CONNECTION_FINISHED) {
              ASSERT(pEvent->pConnectedClient != nullptr);

              if (!pEvent->pConnectedClient->IsRunning()) {
                // Destroy the connection
                SAFE_DELETE(pEvent->pConnectedClient);
              } else {
                // Wait a little bit
                util::YieldThisThread();

                // Resend the event so that we try again later
                SendEvent(pEvent);

                // Skip deleting the event
                continue;
              }
            }

            SAFE_DELETE(pEvent);
          }
        }

        LOG<<"cServer::ThreadFunction returning"<<std::endl;
      }





      // ** cTCPServer

      cTCPServer::cTCPServer(cServer& _server, uint16_t uiPort) :
        server(_server),
        acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), uiPort)),
        pNewConnection(nullptr)
      {
      }

      void cTCPServer::Run()
      {
        // Start accepting connections
        StartAccept();

        // Run the io service
        io_service.run();

        SAFE_DELETE(pNewConnection);
      }

      void cTCPServer::StartAccept()
      {
        LOG<<"cTCPServer::StartAccept"<<std::endl;

        // Make sure that there isn't a current connection in progress
        ASSERT(pNewConnection == nullptr);

        // Create a new connection and try to accept it
        pNewConnection = new cConnectedClient(acceptor.get_io_service());

        // Try to accept a connection some time in the future
        acceptor.async_accept(pNewConnection->GetSocket(),
          boost::bind(&cTCPServer::OnConnection, this, boost::asio::placeholders::error));
      }

      void cTCPServer::OnConnection(const boost::system::error_code& error)
      {
        if (error) {
          LOG<<"cTCPServer::OnConnection error="<<error<<", pNewConnection="<<uint64_t(pNewConnection)<<std::endl;

          // Delete the connection
          SAFE_DELETE(pNewConnection);
        } else {
          cConnectedClient* pConnection = pNewConnection;

          // The server is about to take ownership of the connection
          pNewConnection = nullptr;

          server.OnConnectedClient(pConnection);
        }

        // This connection has now been started, so we need to start accepting the next connection
        StartAccept();
      }



      // ** cConnectionHTTP

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
