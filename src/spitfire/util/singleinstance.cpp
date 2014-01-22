// Standard headers
#include <iostream>
#include <string>
#include <boost/asio.hpp>

// Spitfire headers
#include <spitfire/communication/network.h>
#include <spitfire/storage/filesystem.h>
#include <spitfire/util/thread.h>
#include <spitfire/util/singleinstance.h>

namespace spitfire
{
  namespace util
  {
    namespace ipc
    {
      std::string GetDomainSocketFilePath()
      {
        return spitfire::filesystem::MakeFilePath(spitfire::filesystem::GetApplicationSettingsDirectory(SPITFIRE_APPLICATION_NAME_LWR), "_domain_socket");
      }


      // ** cDomainSocket

      class cDomainSocket
      {
      public:
        explicit cDomainSocket(boost::asio::io_service& io_service);

        void SetOpen();

        void Close();

        bool IsOpen() const;

        const boost::asio::local::stream_protocol::socket& GetSocket() const;
        boost::asio::local::stream_protocol::socket& GetSocket();

        size_t GetBytesToRead();
        size_t GetBytesAvailable();

        size_t Read(void* buffer, size_t len);
        void ReadAll(std::string& sBuffer);

      private:
        bool bIsOpen;
        boost::asio::local::stream_protocol::socket socket;
      };

      cDomainSocket::cDomainSocket(boost::asio::io_service& io_service) :
        socket(io_service)
      {
        bIsOpen = socket.is_open();
      }

      void cDomainSocket::SetOpen()
      {
        bIsOpen = true;
      }

      void cDomainSocket::Close()
      {
        if (bIsOpen) {
          socket.close();
          bIsOpen = false;
        }
      }

      bool cDomainSocket::IsOpen() const
      {
        return bIsOpen;
      }

      const boost::asio::local::stream_protocol::socket& cDomainSocket::GetSocket() const
      {
        return socket;
      }

      boost::asio::local::stream_protocol::socket& cDomainSocket::GetSocket()
      {
        return socket;
      }

      size_t cDomainSocket::GetBytesToRead()
      {
        // Check if there are any bytes available
        boost::asio::socket_base::bytes_readable command(true);

        socket.io_control(command);

        return command.get();
      }

      size_t cDomainSocket::GetBytesAvailable()
      {
        return socket.available();
      }

      size_t cDomainSocket::Read(void* buffer, size_t len)
      {
        LOG<<"cDomainSocket::Read"<<std::endl;

        ASSERT(IsOpen());

        boost::system::error_code error;

        const size_t nLength = socket.read_some(boost::asio::buffer(static_cast<char*>(buffer), len), error);

        if (error == boost::asio::error::eof) { // Connection closed cleanly by peer.
          Close();
        } else if (error) {
          LOG<<"cDomainSocket::Read ERROR When reading from socket"<<std::endl;
          Close();
        } else {
          ASSERT(nLength <= len);
        }

        LOG<<"cDomainSocket::Read len="<<nLength<<std::endl;
        return nLength;
      }

      void cDomainSocket::ReadAll(std::string& sBuffer)
      {
        sBuffer.clear();

        const size_t nBytesToRead = GetBytesToRead();

        LOG<<"cDomainSocket::ReadAll nBytesToRead="<<nBytesToRead<<std::endl;

        std::vector<uint8_t> buffer;
        buffer.clear();
        buffer.reserve(1024);

        const size_t nDataSize = 512;
        uint8_t data[nDataSize];

        while (IsOpen()) {
          const size_t nBytesAvailable = GetBytesAvailable();
          if (nBytesAvailable == 0) {
            //LOG<<"cDomainSocket::ReadAll No bytes available, breaking"<<std::endl;
            break;
          }

          const size_t nBytesRead = Read(data, nDataSize);
          if (nBytesRead == 0) {
            LOG<<"cDomainSocket::ReadAll No bytes read, breaking"<<std::endl;
            break;
          }

          LOG<<"cDomainSocket::ReadAll "<<nBytesRead<<" bytes read"<<std::endl;
          const size_t nBufferSize = buffer.size();

          // Resize the read buffer
          buffer.resize(nBufferSize + nBytesRead);

          // Append the data
          memcpy(buffer.data() + nBufferSize, data, nBytesRead);
        };

        if (buffer.empty()) {
          LOG<<"cDomainSocket::ReadAll No data received, returning"<<std::endl;
          return;
        }

        sBuffer = (const char*)buffer.data();

        LOG<<"cDomainSocket::ReadAll Received \""<<sBuffer<<"\""<<std::endl;
      }
    }

    namespace singleinstance
    {
      // ** cServer

      class cServer : public spitfire::util::cThread
      {
      public:
        cServer();

        bool Start();

      private:
        virtual void ThreadFunction() override;

        spitfire::util::cSignalObject soAction;

        spitfire::util::cSignalObject soStartedOrFailed;
        bool bIsStarted;
      };

      cServer::cServer() :
        spitfire::util::cThread(soAction, "singleinstance::cServer_thread"),
        soAction("singleinstance::cServer_soAction"),
        soStartedOrFailed("singleinstance::cServer_soStartedOrFailed")
      {
      }

      bool cServer::Start()
      {
        StopThreadNow();

        soStartedOrFailed.Reset();
        bIsStarted = false;

        Run();

        soStartedOrFailed.WaitForever();

        return bIsStarted;
      }

      void cServer::ThreadFunction()
      {
        const std::string sFilePath = ipc::GetDomainSocketFilePath();

        spitfire::filesystem::DeleteFile(sFilePath.c_str());

        try
        {
          boost::asio::io_service io_service;
          boost::asio::local::stream_protocol::endpoint ep(sFilePath.c_str());
          boost::asio::local::stream_protocol::acceptor acceptor(io_service, ep);

          bIsStarted = true;
          soStartedOrFailed.Signal();

          while (true) {
            ipc::cDomainSocket socket(io_service);

            LOG<<"cServer::ThreadFunction Accepting connections"<<std::endl;
            acceptor.accept(socket.GetSocket());

            socket.SetOpen();

            // Wait a little bit for the client to start sending data
            util::SleepThisThreadMS(200);

            LOG<<"cServer::ThreadFunction Reading"<<std::endl;
            std::string sBuffer;
            socket.ReadAll(sBuffer);

            // Parse the arguments
            spitfire::string::cStringParser sp(sBuffer);
            if (!sp.StartsWithAndSkip("nArguments=")) {
              LOG<<"cServer::ThreadFunction Error finding the number of arguments"<<std::endl;
              continue;
            }

            std::string sValue;
            sp.GetToStringAndSkip("\n", sValue);
            const size_t nArguments = spitfire::string::ToUnsignedInt(sValue);

            bool bError = false;

            std::vector<std::string> arguments;
            for (size_t i = 0; i < nArguments; i++) {
              if (!sp.StartsWithAndSkip("argument=")) {
                LOG<<"cServer::ThreadFunction Error finding argument "<<i<<std::endl;
                bError = true;
                break;
              }
              sp.GetToStringAndSkip("\n", sValue);
              arguments.push_back(sValue);
            }

            if (!bError) {
              LOG<<"Arguments:"<<std::endl;
              const size_t n = arguments.size();
              for (size_t i = 0; i < n; i++) LOG<<"argument["<<i<<"]="<<"\""<<arguments[i]<<"\""<<std::endl;
            }
          }
        }
        catch (std::exception &e)
        {
          LOG<<"cServer::ThreadFunction Test Exception: "<<e.what()<<std::endl;
          bIsStarted = false;
        }

        spitfire::filesystem::DeleteFile(sFilePath.c_str());

        LOG<<"cServer::ThreadFunction returning"<<std::endl;
      }


      // ** cClient

      class cClient
      {
      public:
        cClient();

        bool OpenClient();

        bool SendToServer(const string_t& sText);

      private:
        boost::asio::io_service io_service;
        boost::asio::local::stream_protocol::socket socket;
      };

      cClient::cClient() :
        socket(io_service)
      {
      }

      bool cClient::OpenClient()
      {
        LOG<<"cClient::OpenClient"<<std::endl;

        const std::string sFilePath = ipc::GetDomainSocketFilePath();

        boost::asio::local::stream_protocol::endpoint endpoint(sFilePath.c_str());
        boost::system::error_code error_code;
        socket.connect(endpoint, error_code);
        if (error_code || !socket.is_open()) {
          LOG<<"cClient::OpenClient Could not open socket, returning false"<<std::endl;
          return false;
        }

        LOG<<"cClient::OpenClient returning true"<<std::endl;
        return true;
      }

      bool cClient::SendToServer(const string_t& sText)
      {
        LOG<<"cClient::SendToServer Writing \""<<sText<<"\""<<std::endl;
        boost::system::error_code error_code;
        boost::asio::write(socket, boost::asio::buffer(sText), boost::asio::transfer_all(), error_code);
        if (error_code) {
          LOG<<"cClient::SendToServer Could not write to socket, returning false"<<std::endl;
          return false;
        }

        LOG<<"cClient::SendToServer returning true"<<std::endl;
        return true;
      }


      // ** cSingleInstance

      cSingleInstance::cSingleInstance() :
        pServer(nullptr),
        pClient(nullptr)
      {
      }

      cSingleInstance::~cSingleInstance()
      {
        if (pServer != nullptr) pServer->StopThreadSoon();

        delete pClient;

        delete pServer;
      }

      bool cSingleInstance::StartServer()
      {
        StopServerNow();

        pServer = new cServer;

        return pServer->Start();
      }

      void cSingleInstance::StopServerSoon()
      {
        if (pServer != nullptr) pServer->StopThreadSoon();
      }

      void cSingleInstance::StopServerNow()
      {
        if (pServer != nullptr) pServer->StopThreadNow();
      }

      bool cSingleInstance::StartClient()
      {
        delete pClient;

        pClient = new cClient;

        return pClient->OpenClient();
      }

      bool cSingleInstance::SendToServer(const string_t& sText)
      {
        if (pClient != nullptr) return pClient->SendToServer(sText);

        return false;
      }
    }
  }
}
