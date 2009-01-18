#ifndef CDOWNLOADHTTP_H
#define CDOWNLOADHTTP_H

#include <breathe/util/thread.h>

#include <breathe/communication/network.h>
#include <breathe/communication/uri.h>

namespace breathe
{
  namespace network
  {
    typedef std::vector<uint8_t*> buffer_t;

    // TODO: Make sure that content has no http header information left in it, cHTTPDownloader should be taking it out.
    // if necessary add cHTTPDownloader::
    // GetContentLengthFromHeader() const;
    // GetContentLengthActual() const;
    // Getresultcodeorsomething() const;

    class cDownloadHTTP : public breathe::util::cThread
    {
    public:
      enum METHOD {
        METHOD_GET,
        METHOD_POST
      };

      cDownloadHTTP();

      // Deprecated 2008, use Download(path, METHOD_GET); instead
      //void Download(const std::string& path);
      void Download(const std::string& path, METHOD method);

      const std::string& GetContent() const { ASSERT(IsSuccessfulDownload()); return content; }

      bool IsSuccessfulDownload() const { return false; }
      bool IsFailedDownload() const { return false; }

    private:
      void ThreadFunction();
      std::string CreateRequest() const;
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


      enum STATE {
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

      METHOD method;
      breathe::network::cURI uri;
      breathe::network::cConnectionTCP connection;
    };

    inline cDownloadHTTP::cDownloadHTTP() :
      status(STATUS_UNKNOWN),
      state(STATE_BEFORE_DOWNLOADING),
      progress(0),
      method(METHOD_GET)
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
      Run();
    }
  }
}

#endif // CDOWNLOADHTTP_H
