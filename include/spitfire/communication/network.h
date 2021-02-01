#pragma once

// Standard headers
#include <list>
#include <experimental/net>

// Spitfire headers
#include <spitfire/util/queue.h>
#include <spitfire/util/string.h>
#include <spitfire/util/thread.h>

namespace spitfire
{
  typedef uint32_t timeoutms_t;

  namespace network
  {
    // Maximum Packet Size
    const int MAXTCPLEN = 128;

    typedef uint16_t port_t;

    class cIPAddress
    {
    public:
      cIPAddress(uint8_t uIPOctet0, uint8_t uIPOctet1, uint8_t uIPOctet2, uint8_t uIPOctet3);
      explicit cIPAddress(const string_t& sIPAddress);

      bool IsValid() const;

      uint8_t GetOctect0() const { return octet[0]; }
      uint8_t GetOctect1() const { return octet[1]; }
      uint8_t GetOctect2() const { return octet[2]; }
      uint8_t GetOctect3() const { return octet[3]; }

      string_t ToString() const;

    private:
      uint8_t octet[4];
    };

    // Outgoing Communication Packet
    struct Packet_Message_Out {
      uint8_t Packet_Length;  // Length of packet being sent
      uint8_t Packet_Type;
      char Message[126];    // message, null terminated
    };

    // Incoming Communication Packets
    //  Two packets are required, the first is always one byte long
    //  and contains the size of the following packet plus itself
    struct Packet_Sizer {
      uint8_t Packet_Length;  // Length of next packet
    };

    struct Packet_Message_In {
      uint8_t Packet_Type;
      char Message[126];    // message, null terminated
    };

    /*************************************************
    ** Type - use
    **  1 - login from client to server
    **  2 - login confirmed from server to client
    **  3 - login failed from server to client
    ** 10 - message from client to server
    ** 11 - message from server to client
    ** 99 - logoff from client to server
    *************************************************/

    bool Init();
    void Destroy();

    // Get the IP for a hostname
    // ie. "www.google.com" might return "172.217.25.132"
    std::string hostname_lookup_ip(const std::string& hostname);

    bool GetIPAddressesOfNetworkInterfaces(std::list<cIPAddress>& addresses);


    class tcp_connection {
    public:
        tcp_connection();
        ~tcp_connection();

        bool connect(const std::string& ip, int port);
        void close();

        int get_sd() const { return sd; }

      size_t get_bytes_available() const;

    private:
        int sd;
    };


    // ** cConnectionTCP
    //
    // A TCP connection for either the server or client

    class cConnectionTCP
    {
    public:
      cConnectionTCP();

      bool Open(const std::string& host, port_t port);
      void Close();

      bool IsOpen() const;

      size_t GetBytesToRead();
      size_t GetBytesAvailable();

      size_t Read(void* buffer, size_t len, timeoutms_t timeoutMS);
      size_t Write(const void* buffer, size_t len);

    private:
      bool bIsOpen;
      std::experimental::net::io_context io_context;
      std::experimental::net::ip::tcp::socket socket;
    };





    class cServer;

    // ** cConnectedClient
    //
    // A connected client on a server

    class cConnectedClient : public spitfire::util::cThread
    {
    public:
      explicit cConnectedClient(std::experimental::net::io_context& socket);

      void Start(cServer& server);

      void Close();

      bool IsOpen();

      void SetNoDelay(); // Set no delay so that we don't buffer our data before sending (This should only be required for EventSources)

      size_t GetBytesToRead();
      size_t GetBytesAvailable();

      const std::experimental::net::ip::tcp::socket& GetSocket() const
      {
        return socket;
      }

      std::experimental::net::ip::tcp::socket& GetSocket()
      {
        return socket;
      }

      size_t Read(uint8_t* pBuffer, size_t nBufferSize);

      void Write(const uint8_t* pBuffer, size_t nBufferSize);
      void Write(const std::string& sData);

    private:
      virtual void ThreadFunction() override;

      /*void WriteCallback(const std::error_code& error, size_t bytes_transferred)
      {
        std::cout<<"WriteCallback error="<<error<<", bytes="<<bytes_transferred<<std::endl;
      }*/

      util::cSignalObject soAction;

      std::experimental::net::ip::tcp::socket socket;
      //std::string message;

      cServer* pServer;
    };


    // ** cTCPConnectionListener
    //
    // A thread that listens for connecting clients

    class cTCPConnectionListener : public spitfire::util::cThread
    {
    public:
      cTCPConnectionListener(cServer& server, uint16_t uiPort);

      void StopThreadNow();

    private:
      virtual void ThreadFunction() override;

      void StartAccept();
      void OnConnection(const std::error_code& error);

      util::cSignalObject soAction;

      cServer& server;
      std::experimental::net::io_context io_context;
      std::experimental::net::ip::tcp::acceptor acceptor;

      cConnectedClient* pNewConnection;
    };


    // ** cServerConnectionHandler
    //
    // An abstract connection handler that could implement any protocol

    class cServerConnectionHandler
    {
    public:
      virtual ~cServerConnectionHandler() {}

      virtual void HandleConnection(cServer& server, cConnectedClient& connection) = 0;
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

    // ** cServer
    //
    // A generic TCP server that handles listening and handling multiple connections on a background thread

    class cServer : public util::cThread
    {
    public:
      cServer();

      friend class cServerConnectionHandler;

      void SetConnectionHandler(cServerConnectionHandler& connectionHandler);
      void SetRootPath(const string_t& sFolderPath);

      void Start(uint16_t uiPort);
      void Stop();

      void OnConnectedClient(cConnectedClient* pNewConnection);
      void RunClientConnection(cConnectedClient& connection);
      void OnClientConnectionFinished(cConnectedClient& connection);

    private:
      void SendEvent(cServerEvent* pEvent);

      virtual void ThreadFunction() override;

      uint16_t uiPort;

      util::cSignalObject soAction;
      spitfire::util::cThreadSafeQueue<cServerEvent> eventQueue;

      cTCPConnectionListener* pTCPConnectionListener;

      cServerConnectionHandler* pConnectionHandler; // For calling back into the application, every connection is sent here
    };
  }
}
