#ifndef CHTTP_H
#define CHTTP_H

// Boost headers
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

// Spitfire headers
#include <spitfire/communication/network.h>
#include <spitfire/communication/uri.h>
#include <spitfire/storage/file.h>
#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/html.h>
#include <spitfire/util/datetime.h>
#include <spitfire/util/queue.h>
#include <spitfire/util/thread.h>

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

        void Clear();

        void SetMethodGet() { method = METHOD::GET; }
        bool IsMethodGet() const { return (method == METHOD::GET); }
        void SetMethodPost() { method = METHOD::POST; }
        bool IsMethodPost() const { return (method == METHOD::POST); }
        void SetHost(const string_t& _sHost) { sHost = _sHost; }
        const string_t& GetHost() const { return sHost; }
        void SetPath(const string_t& _sPath) { sPath = _sPath; }
        const string_t& GetPath() const { return sPath; }
        void SetOffsetBytes(size_t _nOffsetBytes) { nOffsetBytes = _nOffsetBytes; }

        std::string GetContentType() const;
        void SetContentType(const std::string& sContentType);
        size_t GetContentLengthBytes() const;
        void SetContentLengthBytes(size_t nLengthBytes);

        const std::map<std::string, std::string>& GetValues() const { return mValues; }
        void AddValue(const std::string& sName, const std::string& sValue) { mValues[sName] = sValue; }

        const std::map<std::string, std::string>& GetFormData() const { return mFormData; }
        void AddFormData(const std::string& sName, const std::string& sValue) { mFormData[sName] = sValue; }

        void AddPostFileFromPath(const std::string& sName, const string_t& sFilePath);
        //void AddPostFileFromContent(const string_t& sFileName, const void* pBuffer, size_t len);

        bool IsCloseConnection() const;

      protected:
        std::string CreateVariablesString() const;
        std::string CreateRequestHeader() const; // Create a request header, everything up to the content

        METHOD method;
        string_t sHost;
        string_t sPath;
        size_t nOffsetBytes;
        std::map<std::string, std::string> mValues;
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
        void SetContentTypeTextHTMLUTF8();
        bool GetContentDispositionInline(bool& bServeInline, std::string& sFile) const;
        void SetContentDispositionInline(const std::string& sFile);
        void SetDateTimeNow();
        void SetExpires(int iExpires);
        void SetCacheControlPrivateMaxAgeZero();
        void SetCloseConnection();

        std::string ToString() const;

      private:
        STATUS status;
        size_t nContentLengthBytes;
        std::string sMimeType;
        bool bContentDispositionServeInline;
        std::string sContentDispositionFile;
        util::cDateTime dateTime;
        int iExpires;
        bool bCacheControlPrivateMaxAgeZero;
        bool bCloseConnection;
      };



      class cServer;

      class cConnectedClient
      {
      public:
        explicit cConnectedClient(boost::asio::io_service& socket);

        bool IsRunning() const;

        size_t GetBytesToRead();
        size_t GetBytesAvailable();

        const boost::asio::ip::tcp::socket& GetSocket() const
        {
          return socket;
        }

        boost::asio::ip::tcp::socket& GetSocket()
        {
          return socket;
        }

        void Close();

        size_t Read(uint8_t* pBuffer, size_t nBufferSize);

        void SendResponse(const cResponse& response);
        void SendContent(const std::string& sContentUTF8);

        void Write(const uint8_t* pBuffer, size_t nBufferSize);
        void Write(const std::string& sData);

        void Run(cServer& server);

      private:
        /*void WriteCallback(const boost::system::error_code& error, size_t bytes_transferred)
        {
          std::cout<<"WriteCallback error="<<error<<", bytes="<<bytes_transferred<<std::endl;
        }*/

        boost::asio::ip::tcp::socket socket;
        //std::string message;

        bool bIsRunning;
      };



      class cTCPServer
      {
      public:
        cTCPServer(cServer& server, uint16_t uiPort);

        void Run();
        void StopThreadNow();

      private:
        void StartAccept();
        void OnConnection(const boost::system::error_code& error);

        cServer& server;
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::acceptor acceptor;

        cConnectedClient* pNewConnection;
      };



      class cServerRequestHandler
      {
      public:
        virtual ~cServerRequestHandler() {}

        virtual bool HandleRequest(cServer& server, cConnectedClient& connection, const cRequest& request) = 0;
      };

      enum class SERVER_EVENT_TYPE {
        CLIENT_CONNECTION_FINISHED,
        UNKNOWN
      };

      class cServerEvent
      {
      public:
        cServerEvent();

        SERVER_EVENT_TYPE type;
        cConnectedClient* pConnectedClient;
      };

      class cServer : public util::cThread
      {
      public:
        cServer();

        friend class cServerRequestHandler;

        void SetRequestHandler(cServerRequestHandler& pRequestHandler);
        void SetRootPath(const string_t& sFolderPath);

        void Start();
        void Stop();

        void OnConnectedClient(cConnectedClient* pNewConnection);
        void RunClientConnection(cConnectedClient& connection);
        void OnClientConnectionFinished(cConnectedClient& connection);

        void ServeError404(cConnectedClient& connection, const cRequest& request);
        void ServeError(cConnectedClient& connection, const cRequest& request, STATUS status);
        void ServePage(cConnectedClient& connection, const cRequest& request, const string_t& sMimeTypeUTF8, const string_t& sPageContentUTF8);
        void ServeFile(cConnectedClient& connection, const cRequest& request, const string_t& sMimeTypeUTF8, const string_t& sRelativeFilePath);
        void ServeFile(cConnectedClient& connection, const cRequest& request);
        void ServeFileWithResolvedFilePath(cConnectedClient& connection, const cRequest& request, const string_t& sFilePath);

      private:
        bool GetLocalFilePathInWebDirectory(std::string& sRelativeLocalFilePath, const std::string sRelativeFilePath) const;
        bool IsFileInWebDirectory(const std::string sRelativeFilePath) const;

        void SendEvent(cServerEvent* pEvent);

        virtual void ThreadFunction() override;

        util::cSignalObject soAction;
        spitfire::util::cThreadSafeQueue<cServerEvent> eventQueue;

        cTCPServer* pTCPServer;

        cServerRequestHandler* pRequestHandler; // For calling back into the application, every request is sent here first

        std::list<cConnectedClient*> clients;
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

      inline void cRequest::Clear()
      {
        method = METHOD::GET;
        sHost.clear();
        sPath.clear();
        nOffsetBytes = 0;
        mValues.clear();
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

#endif // CHTTP_H
