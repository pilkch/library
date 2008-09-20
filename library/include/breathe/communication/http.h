#ifndef CDOWNLOADHTTP_H
#define CDOWNLOADHTTP_H

namespace breathe
{
  namespace network
  {
    class cDownloadHTTP : public breathe::util::cThread
    {
    public:
      cDownloadHTTP();

      void Download(const std::string& path);
      const std::string& GetContent() const { assert(IsSuccessfulDownload()); return content; }

      bool IsSuccessfulDownload() const { return false; }
      bool IsFailedDownload() const { return false; }

    private:
      int ThreadFunction();
      std::string CreateRequest(uint32_t progress) const;
      void ParseHeader(const char* header);
      std::string Decode(const std::string& encodedString);
      std::string Encode(const std::string& rawString);

      enum STATUS {
        STATUS_UNKNOWN = 0,

        STATUS_CONTINUE = 100,
        STATUS_SWITCHING_PROTOCOLS,

        STATUS_OK = 200,
        STATUS_CREATED,
        STATUS_ACCEPTED,
        STATUS_NON_AUTH_INFO,
        STATUS_NO_CONTENT,
        STATUS_RESET_CONTENT,
        STATUS_PARTIAL_CONTENT,

        STATUS_MULTIPLE_CHOICES = 300,
        STATUS_MOVED_PERMANENTLY,
        STATUS_FOUND,
        STATUS_SEE_OTHER,
        STATUS_NOT_MODIFIED,
        STATUS_USE_PROXY,
        STATUS_UNUSED_306, // 306 is Unused
        STATUS_TEMP_REDIRECT = 307,

        STATUS_BAD_REQUEST = 400,
        STATUS_UNAUTHORIZED,
        STATUS_PAYMENT_REQUIRED,
        STATUS_FORBIDDEN,
        STATUS_NOT_FOUND,
        STATUS_METHOD_NOT_ALLOWED,
        STATUS_NOT_ACCEPTABLE,
        STATUS_PROXY_AUTH_REQUIRED,
        STATUS_REQUEST_TIMEOUT,
        STATUS_CONFLICT,
        STATUS_GONE,
        STATUS_LENGTH_REQUIRED,
        STATUS_PRE_CONDITION_FAILED,
        STATUS_REQUEST_ENTITY_TOO_LONG,
        STATUS_REQUEST_URI_TOO_LONG,
        STATUS_UNSUPPORTED_MEDIA,
        STATUS_REQUEST_RANGE_NOT_SATISFIABLE,
        STATUS_EXPECTATION_FAILED,

        STATUS_INTERNAL_SERVER_ERROR = 500,
        STATUS_NOT_IMPLEMENTED,
        STATUS_BAD_GATEWAY,
        STATUS_SERVICE_UNAVAILABLE,
        STATUS_GATEWAY_TIMEOUT,
        STATUS_HTTP_VERSION_NOT_SUPPORTED
      };


      enum STATE
      {
        STATE_BEFORE_DOWNLOADING = 0,
        STATE_DOWNLOADING,
        STATE_FINISHED,

        STATE_INVALID_PATH,
        STATE_DISCONNECTED
      };

      STATUS status;
      STATE state;
      uint32_t progress;
      std::string content;

      breathe::network::cURI uri;
      breathe::network::cConnectionTCP connection;
    };

    inline cDownloadHTTP::cDownloadHTTP() :
      status(status_before_downloading),
      progress(0)
    {
    }

    inline std::string cDownloadHTTP::CreateRequest() const
    {
      request = "";

      std::ostringstream o;
      o  <<"GET /"<<uri.GetPath()<<" HTTP/1.1" STR_END
          <<"Host: "<<uri.GetServer()<< STR_END
          <<"Range: bytes="<<progress<<"-" STR_END
          <<"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)" STR_END
          <<"Accept: */*" STR_END
          <<"Accept-Language: en-us" STR_END
          <<"Connection: Keep-Alive" STR_END
          <<"" STR_END;
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

    inline int cDownloadHTTP::ThreadFunction()
    {
      std::cout<<server<<" "<<path<<std::endl;
      content = "";

      connection.Open(server, 80);

      {
        // Send header
        std::string request(CreateRequest());
        if (!connection.Send(request))
        {
          std::stderr<<"SDLNet_TCP_Send: "<<SDLNet_GetError()<<std::endl;
          exit(EXIT_FAILURE);
        }
      }

      int len = 0;

      char buffer[STR_LEN];
      buffer[0] = 0;

      do
      {
        if (!connection.Recv(header, STR_LEN)) break;

        ParseHeader(header);
        len = SDLNet_TCP_Recv(connection.sd, buffer, STR_LEN - 1);
        if (len > 0)
        {
          buffer[len] = 0;
          content += buffer;
        }
      } while (len > 0);

      std::cout<<"CONTENT"<<std::endl;
      std::cout<<content<<std::endl;

      connection.Close();

      return 0;
    }

    inline std::string cDownloadHTTP::Decode(const std::string& encodedString)
    {
      std::size_t encodedLen = encodedString.size();
      const char* encStr = encodedString.c_str();
      std::string decodedString;
      const char* tmpStr = NULL;
      std::size_t cnt = 0;

      // Reserve enough space for the worst case.
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
          PUSH_BACK_CHAR(decodedString, ' ');
        } else if ('%' == curChar) {
          if (tmpStr != NULL) {
            decodedString.append(tmpStr, cnt);
            tmpStr = NULL;
            cnt = 0;
          }
          if (i + 2 < encodedLen && isHexDigit(encStr[i + 1]) && isHexDigit(encStr[i + 2])) {
            unsigned int value;

            value = convertHexDigit(encStr[++i]);
            value = (value * 0x10) + convertHexDigit(encStr[++i]);
            PUSH_BACK_CHAR(decodedString, static_cast<char>(value));
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

    inline std::string cDownloadHTTP::Encode(const std::string& rawString)
    {
      std::size_t rawLen = rawString.size();
      std::string encodedString;
      char encodingBuffer[4] = { '%', '\0', '\0', '\0' };

      encodedString.reserve(rawLen);

      for (std::size_t i = 0; i < rawLen; ++i) {
        char curChar = rawString[i];

        if (curChar == ' ') encodedString += '+';
        else if (isAlpha(curChar) || isDigit(curChar) || isSpecial(curChar)) encodedString += curChar;
        else {
          unsigned int temp = static_cast<unsigned int>(curChar);

          encodingBuffer[1] = convertToHexDigit(temp / 0x10);
          encodingBuffer[2] = convertToHexDigit(temp % 0x10);
          encodedString += encodingBuffer;
        }
      }

      return encodedString;
    }

    inline void cDownloadHTTP::Download(const std::string& full_uri)
    {
      // Start downloading at the beginning
      progress = 0;

      // Parse the uri
      uri.Parse(full_uri);

      // Now we are ready to download the file
      Run();
    }
  }
}

#endif // CDOWNLOADHTTP_H
