#ifndef CHTTP_H
#define CHTTP_H

#include <spitfire/communication/network.h>
#include <spitfire/communication/uri.h>

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

      // TODO: move this into cRequest
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

        void SetMethodGet() { method = METHOD::GET; }
        bool IsMethodGet() const { return (method == METHOD::GET); }
        void SetMethodPost() { method = METHOD::POST; }
        bool IsMethodPost() const { return (method == METHOD::POST); }
        void SetHost(const string_t& _sHost) { sHost = _sHost; }
        const string_t& GetHost() const { return sHost; }
        void SetPath(const string_t& _sPath) { sPath = _sPath; }
        const string_t& GetPath() const { return sPath; }
        void SetOffsetBytes(size_t _nOffsetBytes) { nOffsetBytes = _nOffsetBytes; }

        const std::map<std::string, std::string>& GetValues() const { return mValues; }
        void AddValue(const std::string& sName, const std::string& sValue) { mValues[sName] = sValue; }

        void AddPostFileFromPath(const std::string& sName, const string_t& sFilePath);
        //void AddPostFileFromContent(const string_t& sFileName, const void* pBuffer, size_t len);

      protected:
        void SetContentType(const std::string& sContentType);

        std::string CreateVariablesString() const;
        std::string CreateRequestHeader() const; // Create a request header, everything up to the content

        METHOD method;
        string_t sHost;
        string_t sPath;
        size_t nOffsetBytes;
        std::map<std::string, std::string> mValues;

        // TODO: Support multiple attachments
        struct cFile
        {
          std::string sName;
          string_t sFilePath;
        };

        cFile file;
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

#endif // CHTTP_H
