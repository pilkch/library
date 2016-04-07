// Standard libraries
#include <cassert>
#include <cmath>
#include <memory.h>

#include <list>
#include <vector>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include <chrono>
#include <mutex>
#include <thread>

// Boost headers
#include <boost/asio.hpp>

// Other libraries
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __LINUX__
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>
#include <spitfire/util/log.h>
#include <spitfire/util/thread.h>

#include <spitfire/math/math.h>

#include <spitfire/communication/network.h>

// Breathe headers
#include <breathe/breathe.h>


#define STR_LEN 512
#define STR_END "\r\n"

namespace spitfire
{
  namespace network
  {
    cIPAddress::cIPAddress(uint8_t uIPOctet0, uint8_t uIPOctet1, uint8_t uIPOctet2, uint8_t uIPOctet3)
    {
      octet[0] = uIPOctet0;
      octet[1] = uIPOctet1;
      octet[2] = uIPOctet2;
      octet[3] = uIPOctet3;
    }

    cIPAddress::cIPAddress(const string_t& sIPAddress)
    {
      string::cStringParser sp(sIPAddress);

      string_t sValue;
      if (!sp.IsEnd()) {
        sp.GetToStringAndSkip(".", sValue);
        octet[0] = string::ToUnsignedInt(sValue);
      }
      if (!sp.IsEnd()) {
        sp.GetToStringAndSkip(".", sValue);
        octet[1] = string::ToUnsignedInt(sValue);
      }
      if (!sp.IsEnd()) {
        sp.GetToStringAndSkip(".", sValue);
        octet[2] = string::ToUnsignedInt(sValue);
      }
      if (!sp.IsEnd()) {
        octet[3] = string::ToUnsignedInt(sp.GetToEnd());
      }
    }

    bool cIPAddress::IsValid() const
    {
      return ((octet[0] != 0) && (octet[1] != 0) && (octet[2] != 0) && (octet[3] != 0));
    }

    string_t cIPAddress::ToString() const
    {
      return string::ToString(uint32_t(octet[0])) + "." + string::ToString(uint32_t(octet[1])) + "." + string::ToString(uint32_t(octet[2])) + "." + string::ToString(uint32_t(octet[3]));
    }

    /*
      Uint32 number;

      // put my number into a data buffer to prepare for sending to a remote host
      char out[1024];
      number=0xDEADBEEF;
      SDLNet_Write32(number, out);



      // get a number from a data buffer to use on this host
      char in[1024];
      char* ptr=&in[40]; //this points into a previously received data buffer
      number=0;
      number=SDLNet_Read32(ptr);
      // number is now in your hosts byte order, ready to use.
    */

    bool Init()
    {
      LOG<<"cNetwork Init"<<std::endl;

      return true;
    }

    void Destroy()
    {
      LOG<<"cNetwork Destroy"<<std::endl;
    }

    bool GetIPAddressesOfNetworkInterfaces(std::list<cIPAddress>& addresses)
    {
      addresses.clear();

      ifaddrs* ifaddr = nullptr;
      if (getifaddrs(&ifaddr) == -1) {
        perror("GetIPAddressesOfNetworkInterfaces getifaddrs FAILED");
        return false;
      }

      // Iterate through the network interfaces
      for (const ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        const sockaddr* pAddr = ifa->ifa_addr;
        if (pAddr == nullptr) continue;

        const int family = pAddr->sa_family;
        if (family == AF_INET) {
          char host[NI_MAXHOST];
          int s = getnameinfo(pAddr, sizeof(struct sockaddr_in), host, NI_MAXHOST, nullptr, 0, NI_NUMERICHOST);
          if (s != 0) {
            LOG<<"GetIPAddressesOfNetworkInterfaces getnameinfo FAILED: "<<gai_strerror(s)<<std::endl;
            return false;
          }

          // Add the address
          addresses.push_back(cIPAddress(host));
        }
      }

      freeifaddrs(ifaddr);

      return true;
    }


    // *** cConnectionTCP

    cConnectionTCP::cConnectionTCP() :
      bIsOpen(false),
      socket(io_service)
    {
    }

    bool cConnectionTCP::Open(const std::string& host, port_t port)
    {
      ASSERT(!IsOpen());

      boost::asio::ip::tcp::resolver resolver(io_service);

      const std::string sPort = spitfire::string::ToUTF8(spitfire::string::ToString(port));
      boost::asio::ip::tcp::resolver::query query(host, sPort);
      boost::system::error_code error = boost::asio::error::host_not_found;
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, error);
      boost::asio::ip::tcp::resolver::iterator end;

      error = boost::asio::error::host_not_found;
      while (error && (endpoint_iterator != end)) {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
      }

      if (error) {
        LOG<<"cConnectionTCP::Open ERROR Finding a connection"<<std::endl;
        return false;
      }

      bIsOpen = true;
      return true;
    }

    void cConnectionTCP::Close()
    {
      if (bIsOpen) {
        socket.close();
        bIsOpen = false;
      }
    }

    bool cConnectionTCP::IsOpen() const
    {
      return bIsOpen;
    }

    size_t cConnectionTCP::GetBytesToRead()
    {
      // Check if there are any bytes available
      boost::asio::socket_base::bytes_readable command(true);

      socket.io_control(command);

      return command.get();
    }

    size_t cConnectionTCP::GetBytesAvailable()
    {
      return socket.available();
    }

    size_t cConnectionTCP::Read(void* buffer, size_t len, timeoutms_t timeoutMS)
    {
      LOG<<"cConnectionTCP::Read Reading with timeout "<<timeoutMS<<std::endl;

      ASSERT(IsOpen());

      boost::system::error_code error;

      const size_t nLength = socket.read_some(boost::asio::buffer(static_cast<char*>(buffer), len), error);

      if (error == boost::asio::error::eof) { // Connection closed cleanly by peer.
        Close();
      } else if (error) {
        LOG<<"cConnectionTCP::Read ERROR When reading from socket"<<std::endl;
        Close();
      } else {
        ASSERT(nLength <= len);
      }

      LOG<<"cConnectionTCP::Read len="<<nLength<<std::endl;
      return nLength;
    }

    size_t cConnectionTCP::Write(const void* buffer, size_t len)
    {
      LOG<<"cConnectionTCP::Write"<<std::endl;

      ASSERT(IsOpen());

      boost::system::error_code error;

      const size_t nLength = socket.write_some(boost::asio::buffer(static_cast<const char*>(buffer), len), error);

      if (error == boost::asio::error::eof) { // Connection closed cleanly by peer.
        Close();
      } else if (error) {
        LOG<<"cConnectionTCP::Write ERROR When reading from socket"<<std::endl;
        Close();
      } else {
        ASSERT(nLength == len);
      }

      return nLength;
    }




    // ** cConnectedClient

    cConnectedClient::cConnectedClient(boost::asio::io_service& _socket) :
      util::cThread(soAction, "cServer"),
      soAction("soAction"),
      socket(_socket),
      pServer(nullptr)
    {
    }

    void cConnectedClient::Close()
    {
      socket.close();
    }

    void cConnectedClient::Start(cServer& server)
    {
      LOG<<"cConnectedClient::Start"<<std::endl;

      pServer = &server;

      Run();
    }

    void cConnectedClient::ThreadFunction()
    {
      LOG<<"cConnectedClient::ThreadFunction"<<std::endl;

      try {
        pServer->RunClientConnection(*this);
      }
      catch (std::exception& e) {
        // Catch any boost asio socket errors
        LOG<<e.what()<<std::endl;
      }

      pServer->OnClientConnectionFinished(*this);

      pServer = nullptr;

      LOG<<"cConnectedClient::ThreadFunction returning"<<std::endl;
    }

    bool cConnectedClient::IsOpen()
    {
      return socket.is_open();
    }

    void cConnectedClient::SetNoDelay()
    {
      socket.set_option(boost::asio::ip::tcp::no_delay(true));
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
      // TODO: Replace this code with something like this or C++11 bind
      /*message = sData;

      boost::asio::async_write(socket, boost::asio::buffer(message),
          boost::bind(&cConnectedClient::WriteCallback, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));*/

      boost::asio::write(socket, boost::asio::buffer(sData), boost::asio::transfer_all());
    }



    // ** cTCPConnectionListener

    cTCPConnectionListener::cTCPConnectionListener(cServer& _server, uint16_t uiPort) :
      util::cThread(soAction, "cServer"),
      soAction("soAction"),
      server(_server),
      acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), uiPort)),
      pNewConnection(nullptr)
    {
    }

    void cTCPConnectionListener::StopThreadNow()
    {
      // Tell the service to stop
      io_service.stop();

      // Stop the thread
      util::cThread::StopThreadNow();
    }

    void cTCPConnectionListener::ThreadFunction()
    {
      // Start accepting connections
      StartAccept();

      // Run the io service
      io_service.run();

      SAFE_DELETE(pNewConnection);
    }

    void cTCPConnectionListener::StartAccept()
    {
      LOG<<"cTCPConnectionListener::StartAccept"<<std::endl;

      // Make sure that there isn't a current connection in progress
      ASSERT(pNewConnection == nullptr);

      // Create a new connection and try to accept it
      pNewConnection = new cConnectedClient(acceptor.get_io_service());

      // Try to accept a connection some time in the future
      acceptor.async_accept(pNewConnection->GetSocket(),
        boost::bind(&cTCPConnectionListener::OnConnection, this, boost::asio::placeholders::error));
    }

    void cTCPConnectionListener::OnConnection(const boost::system::error_code& error)
    {
      LOG<<"cTCPConnectionListener::OnConnection"<<std::endl;

      if (error) {
        LOG<<"cTCPConnectionListener::OnConnection error="<<error<<", pNewConnection="<<uint64_t(pNewConnection)<<std::endl;

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


    // ** cServerEvent

    cServerEvent::cServerEvent() :
      type(SERVER_EVENT_TYPE::UNKNOWN),
      pConnectedClient(nullptr)
    {
    }


    // ** cServer

    cServer::cServer() :
      util::cThread(soAction, "cServer"),
      uiPort(38001),
      soAction("soAction"),
      eventQueue(soAction),
      pTCPConnectionListener(nullptr),
      pConnectionHandler(nullptr)
    {
    }

    void cServer::SetConnectionHandler(cServerConnectionHandler& connectionHandler)
    {
      pConnectionHandler = &connectionHandler;
    }

    void cServer::Start(uint16_t _uiPort)
    {
      // Remember the port
      uiPort = _uiPort;

      // Start the thread
      Run();
    }

    void cServer::Stop()
    {
      // Tell the connections to stop

      // Tell the server to stop
      if (pTCPConnectionListener != nullptr) pTCPConnectionListener->StopThreadNow();

      // Tell the thread to stop
      StopThreadNow();

      // Make sure that the event queue is now empty
      ASSERT(eventQueue.IsEmpty());
    }

    void cServer::OnConnectedClient(cConnectedClient* pNewConnection)
    {
      LOG<<"cServer::OnConnectedClient New connection started"<<std::endl;

      ASSERT(pNewConnection != nullptr);

      // Start the connection thread
      pNewConnection->Start(*this);
    }

    void cServer::RunClientConnection(cConnectedClient& connection)
    {
      // Wait for the request to be sent
      spitfire::util::SleepThisThreadMS(10);

      if (pConnectionHandler != nullptr) pConnectionHandler->HandleConnection(*this, connection);
      else {
        // There is no handler so we have no idea what to do with the connection so we just close it
        connection.Close();
      }
    }

    void cServer::OnClientConnectionFinished(cConnectedClient& connection)
    {
      LOG<<"cServer::OnClientConnectionFinished Sending event CLIENT_CONNECTION_FINISHED"<<std::endl;
      cServerEvent* pEvent = new cServerEvent;
      pEvent->type = SERVER_EVENT_TYPE::CLIENT_CONNECTION_FINISHED;
      pEvent->pConnectedClient = &connection;
      SendEvent(pEvent);
    }

    void cServer::SendEvent(cServerEvent* pEvent)
    {
      eventQueue.AddItemToBack(pEvent);
      soAction.Signal();
    }

    void cServer::ThreadFunction()
    {
      LOG<<"cServer::ThreadFunction"<<std::endl;

      pTCPConnectionListener = new cTCPConnectionListener(*this, uiPort);
      pTCPConnectionListener->Run();

      while (true) {
        soAction.WaitTimeoutMS(1000);

        if (IsToStop()) break;

        cServerEvent* pEvent = eventQueue.RemoveItemFromFront();
        if (pEvent != nullptr) {
          if (pEvent->type == SERVER_EVENT_TYPE::CLIENT_CONNECTION_FINISHED) {
            LOG<<"cServer::ThreadFunction Processing event CLIENT_CONNECTION_FINISHED"<<std::endl;
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

        // Try to avoid hogging the CPU
        spitfire::util::SleepThisThreadMS(1);
      }

      pTCPConnectionListener->StopThreadNow();
      SAFE_DELETE(pTCPConnectionListener);

      // Remove any further events because we don't care any more
      while (true) {
        cServerEvent* pEvent = eventQueue.RemoveItemFromFront();
        if (pEvent == nullptr) break;

        spitfire::SAFE_DELETE(pEvent);
      }

      LOG<<"cServer::ThreadFunction returning"<<std::endl;
    }
  }
}
