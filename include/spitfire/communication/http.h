#pragma once

// Standard headers
#include <experimental/net>

// Spitfire headers
#include <spitfire/communication/network.h>
#include <spitfire/communication/uri.h>
#include <spitfire/storage/file.h>
#include <spitfire/storage/filesystem.h>
#include <spitfire/util/datetime.h>

namespace spitfire
{
  namespace network
  {
    typedef std::vector<uint8_t*> buffer_t;

    string_t GetMimeTypeFromExtension(const string_t& sExtension, bool& bServeInline);

    namespace http
    {
      // http://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol
      // http://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol#Request_methods
      // http://en.wikipedia.org/wiki/List_of_HTTP_headers

      std::string URLEncode(const std::string& url);

      enum class METHOD {
        GET,
        POST
      };

      class cHTTP;

      class cRequest
      {
      public:
        friend class cHTTP;

        cRequest();

        void Clear();

        void SetMethod(METHOD _method) { method = _method; }
        void SetMethodGet() { method = METHOD::GET; }
        bool IsMethodGet() const { return (method == METHOD::GET); }
        void SetMethodPost() { method = METHOD::POST; }
        bool IsMethodPost() const { return (method == METHOD::POST); }
        void SetHost(const string_t& _sHost) { sHost = _sHost; }
        const string_t& GetHost() const { return sHost; }
        void SetPath(const string_t& _sPath) { sPath = _sPath; }
        const string_t& GetPath() const { return sPath; }
        void SetOffsetBytes(size_t _nOffsetBytes) { nOffsetBytes = _nOffsetBytes; }

        std::string GetAccept() const;
        std::string GetContentType() const;
        void SetContentType(const std::string& sContentType);
        size_t GetContentLengthBytes() const;
        void SetContentLengthBytes(size_t nLengthBytes);

        const std::map<std::string, std::string>& GetOtherHeaders() const { return mOtherHeaders; }
        void AddOtherHeader(const std::string& sName, const std::string& sValue) { mOtherHeaders[sName] = sValue; }

        const std::map<std::string, std::string>& GetVariables() const { return mVariables; }
        void AddVariable(const std::string& sName, const std::string& sValue) { mVariables[sName] = sValue; }

        const std::map<std::string, std::string>& GetFormData() const { return mFormData; }
        void AddFormData(const std::string& sName, const std::string& sValue) { mFormData[sName] = sValue; }

        void AddPostFileFromPath(const std::string& sName, const string_t& sFilePath);
        //void AddPostFileFromContent(const string_t& sFileName, const void* pBuffer, size_t len);

        bool IsConnectionClose() const;
        bool IsConnectionKeepAlive() const { return !IsConnectionClose(); }
        void SetConnectionClose();
        void SetConnectionKeepAlive();

      protected:
        std::string CreateVariablesString() const;
        std::string CreateRequestHeader() const; // Create a request header, everything up to the content

        METHOD method;
        string_t sHost;
        string_t sPath;
        size_t nOffsetBytes;
        std::map<std::string, std::string> mOtherHeaders;
        std::map<std::string, std::string> mVariables;
        std::map<std::string, std::string> mFormData;

        // TODO: Support multiple attachments
        struct cFile
        {
          std::string sName;
          string_t sFilePath;
        };

        cFile file;
      };


      bool ParseRequest(cRequest& request, const std::string& sRequest);


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

      string_t GetStatusAsString(STATUS status);
      string_t GetStatusDescription(STATUS status);


      class cResponse
      {
      public:
        cResponse();

        void SetStatus(STATUS status);
        void SetContentLengthBytes(size_t nContentLengthBytes);
        void SetContentMimeType(const std::string& sMimeType);
        void SetContentTypeTextPlainUTF8();
        void SetContentTypeTextHTMLUTF8();
        void SetContentTypeTextEventStream();
        bool GetContentDispositionInline(bool& bServeInline, std::string& sFile) const;
        void SetContentDispositionInline(const std::string& sFile);
        void SetExpiresMinusOne();
        void SetExpiresOneMonth();
        void SetExpiresOneYear();
        void SetCacheControlNoCache();
        void SetCacheControlPrivateMaxAgeZero();
        void SetCacheControlPublic();
        void SetConnectionClose();
        void SetConnectionKeepAlive();

        std::string ToString() const;

      private:
        enum class EXPIRES {
          MINUS_ONE,
          ONE_MONTH,
          ONE_YEAR,
        };
        enum class CACHE_CONTROL {
          NOT_SPECIFIED,
          NO_CACHE,
          PUBLIC,
          PRIVATE_MAX_AGE_ZERO,
        };

        STATUS status;
        bool bContentLengthSet;
        size_t nContentLengthBytes;
        std::string sMimeType;
        bool bContentDispositionServeInline;
        std::string sContentDispositionFile;
        EXPIRES expires;
        CACHE_CONTROL cacheControl;
        bool bConnectionKeepAlive;
      };


      class cServerUtil
      {
      public:
        bool IsFileInWebDirectory(const std::string sRelativeFilePath) const;

        void SendResponse(spitfire::network::cConnectedClient& connection, const cResponse& response) const;
        void SendContent(spitfire::network::cConnectedClient& connection, const std::string& sContentUTF8) const;

        void ServeError404(spitfire::network::cConnectedClient& connection, const cRequest& request) const;
        void ServeError(spitfire::network::cConnectedClient& connection, const cRequest& request, STATUS status) const;
        void ServePage(spitfire::network::cConnectedClient& connection, const cRequest& request, const string_t& sMimeTypeUTF8, const string_t& sPageContentUTF8) const;
        void ServeFile(spitfire::network::cConnectedClient& connection, const cRequest& request, const string_t& sMimeTypeUTF8, const string_t& sRelativeFilePath) const;
        void ServeFile(spitfire::network::cConnectedClient& connection, const cRequest& request) const;
        void ServeFileWithResolvedFilePath(spitfire::network::cConnectedClient& connection, const cRequest& request, const string_t& sFilePath) const;

      private:
        bool GetLocalFilePathInWebDirectory(std::string& sRelativeLocalFilePath, const std::string sRelativeFilePath) const;
      };








      enum class STATE {
        BEFORE_DOWNLOADING,
        CONNECTING,
        SENDING_REQUEST,
        SENDING_CONTENT,
        RECEIVING_HEADER,
        RECEIVING_CONTENT,
        FINISHED,

        INVALID_URI,
        CONNECTION_FAILED,
        DISCONNECTED
      };

      class cRequestListener
      {
      public:
        bool IsToStop() const { return _IsToStop(); }

        void OnStateChanged(STATE state) { _OnStateChanged(state); }
        void OnStatusReceived(STATUS status) { _OnStatusReceived(status); }
        void OnHeaderReceived(const std::string& sHeader) { _OnHeaderReceived(sHeader); }
        void OnTextContentReceived(const std::string& sContent) { _OnTextContentReceived(sContent); }
        void OnBinaryContentReceived(const void* pContent, size_t len) { _OnBinaryContentReceived(pContent, len); }

      private:
        virtual bool _IsToStop() const { return false; }

        virtual void _OnStateChanged(STATE state) {}
        virtual void _OnStatusReceived(STATUS status) {}
        virtual void _OnHeaderReceived(const std::string& sHeader) {}

        // For text
        virtual void _OnTextContentReceived(const std::string& sContent) {}

        // For binary content
        virtual void _OnBinaryContentReceived(const void* pContent, size_t len) {}
      };

      typedef cRequestListener cRequestListenerVoid;



      #ifdef BUILD_NETWORK_TLS
      void DownloadHTTPS(const std::string& url, spitfire::network::http::cRequestListener& listener);
      #endif // BUILD_NETWORK_TLS


      // ** cHTTP
      //
      // A class for performing HTTP requests and receiving data or downloading files
      // TODO: Make sure that content has no http header information left in it, cHTTP should be taking it out.
      // if necessary add these:
      // GetContentLengthFromHeader() const;
      // GetContentLengthActual() const;
      // Getresultcodeorsomething() const;

      class cHTTP
      {
      public:
        cHTTP();

        void Download(const std::string& path, METHOD method, cRequestListener& listener) const;
        void Download(const std::string& path, METHOD method) const { cRequestListenerVoid listener; Download(path, method, listener); }
        void SendRequest(const cRequest& request, cRequestListener& listener) const;
        void SendRequest(const cRequest& request) const { cRequestListenerVoid listener; SendRequest(request, listener); }

        bool IsSuccessful() const { return (state == STATE::FINISHED); }
        bool IsFailed() const { return !(state == STATE::FINISHED); }

        STATUS GetStatus() const { return status; }
        STATE GetState() const { return state; }

      private:
        void SendRequest(cConnectionTCP& connection, const cRequest& request, cRequestListener& listener) const;

        mutable STATUS status;
        mutable STATE state;
        mutable uint32_t progress;
      };

      // ** Inlines

      // ** cRequest

      inline cRequest::cRequest() :
        method(METHOD::GET),
        sPath(TEXT("/")),
        nOffsetBytes(0)
      {
        SetConnectionClose();
      }

      inline void cRequest::Clear()
      {
        method = METHOD::GET;
        sHost.clear();
        sPath.clear();
        nOffsetBytes = 0;
        mOtherHeaders.clear();
        mFormData.clear();

        file.sName.clear();
        file.sFilePath.clear();
      }


      // ** cHTTP

      inline cHTTP::cHTTP() :
        status(STATUS::UNKNOWN),
        state(STATE::BEFORE_DOWNLOADING),
        progress(0)
      {
      }
    }
  }
}
