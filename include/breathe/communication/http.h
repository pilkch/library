#ifndef CDOWNLOADHTTP_H
#define CDOWNLOADHTTP_H

#include <spitfire/util/thread.h>

#include <breathe/communication/network.h>
#include <breathe/communication/uri.h>

namespace breathe
{
  namespace network
  {
    typedef std::vector<uint8_t*> buffer_t;

    namespace http
    {
      // http://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol
      // http://en.wikipedia.org/wiki/List_of_HTTP_headers


      enum class METHOD {
        GET,
        POST
      };

      enum class STATUS {
        UNKNOWN = 0,

        CONTINUE = 100,
        SWITCHING_PROTOCOLS,

        OK = 200,
        CREATED,
        ACCEPTED,
        NON_AUTH_INFO,
        NO_CONTENT,
        RESET_CONTENT,
        PARTIAL_CONTENT,

        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY,
        FOUND,
        SEE_OTHER,
        NOT_MODIFIED,
        USE_PROXY,
        UNUSED_306, // 306 is Unused
        TEMP_REDIRECT = 307,

        BAD_REQUEST = 400,
        UNAUTHORIZED,
        PAYMENT_REQUIRED,
        FORBIDDEN,
        NOT_FOUND,
        METHOD_NOT_ALLOWED,
        NOT_ACCEPTABLE,
        PROXY_AUTH_REQUIRED,
        REQUEST_TIMEOUT,
        CONFLICT,
        GONE,
        LENGTH_REQUIRED,
        PRE_CONDITION_FAILED,
        REQUEST_ENTITY_TOO_LONG,
        REQUEST_URI_TOO_LONG,
        UNSUPPORTED_MEDIA,
        REQUEST_RANGE_NOT_SATISFIABLE,
        EXPECTATION_FAILED,

        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED,
        BAD_GATEWAY,
        SERVICE_UNAVAILABLE,
        GATEWAY_TIMEOUT,
        HTTP_VERSION_NOT_SUPPORTED
      };


      enum class STATE {
        BEFORE_DOWNLOADING,
        CONNECTING,
        SENDING_REQUEST,
        RECEIVING_HEADER,
        RECEIVING_CONTENT,
        FINISHED,

        INVALID_URI,
        CONNECTION_FAILED,
        DISCONNECTED
      };


      // TODO: Make sure that content has no http header information left in it, cHTTPDownloader should be taking it out.
      // if necessary add these:
      // GetContentLengthFromHeader() const;
      // GetContentLengthActual() const;
      // Getresultcodeorsomething() const;

      class cDownloadHTTP : public breathe::util::cThread
      {
      public:
        cDownloadHTTP();

        // Deprecated 2008, use Download(path, METHOD_GET); instead
        //void Download(const std::string& path);
        void Download(const std::string& path, METHOD method);

        const std::string& GetContent() const { ASSERT(IsSuccessfulDownload()); return content; }

        bool IsSuccessfulDownload() const { return (state == STATE::FINISHED); }
        bool IsFailedDownload() const { return !(state == STATE::FINISHED); }

        STATUS GetStatus() const { return status; }
        STATE GetState() const { return state; }

      private:
        void ThreadFunction();
        std::string CreateRequest() const;
        void ParseHeader(const char* header);
        std::string Decode(const std::string& encodedString);
        std::string Encode(const std::string& rawString);

        STATUS status;
        STATE state;
        uint32_t progress;
        std::string content;

        METHOD method;
        breathe::network::cURI uri;
      };

      inline cDownloadHTTP::cDownloadHTTP() :
        status(STATUS::UNKNOWN),
        state(STATE::BEFORE_DOWNLOADING),
        progress(0),
        method(METHOD::GET)
      {
      }

      inline void cDownloadHTTP::Download(const std::string& full_uri, METHOD _method)
      {
        if (IsRunning()) return;

        // Start downloading at the beginning
        progress = 0;

        // Parse the uri
        uri.Parse(full_uri);

        method = _method;

        // Now we are ready to download the file
        //Run();

        ThreadFunction();
      }
    }
  }
}

#endif // CDOWNLOADHTTP_H
