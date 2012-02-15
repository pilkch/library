#ifndef CNETWORK_H
#define CNETWORK_H

#include <spitfire/math/math.h>

namespace spitfire
{
  typedef uint32_t timeoutms_t;

  namespace network
  {
    // Maximum Packet Size
    const int MAXTCPLEN = 128;

    typedef uint16_t port_t;

    class cIPAddress;
    extern const cIPAddress localhost;

    /*class cIPAddress
    {
    public:
      cIPAddress(uint8_t uIPOctet0, uint8_t uIPOctet1, uint8_t uIPOctet2, uint8_t uIPOctet3);
      explicit cIPAddress(const string_t& sIPAddressOrHostName);

      bool IsIpAddress() const { return bIsIP; }
      bool IsHostName() const { return !bIsIP; }

      string_t GetIPAddress() const { return  }
      string_t GetHostNameString() const {  }

    private:
      bool bIsIP;
      uint8_t uIPOctet[4];
      string_t sHostName;
    };*/

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



    // For choosing a static port look at this list
    // http://en.wikipedia.org/wiki/List_of_well-known_ports_(computing)

    // Random dynamic port between 49152 and 65535
    inline port_t GetDynamicPort()
    {
      return port_t(49152) + port_t(math::random(16383));
    }


    class cConnectionTCP
    {
    public:
      cConnectionTCP();

      bool Open(const std::string& host, port_t port);
      void Close();

      bool IsOpen() const;

      size_t Recv(void* buffer, size_t len, timeoutms_t timeoutMS);
      size_t Send(const void* buffer, size_t len);

    private:
      bool bIsOpen;
      boost::asio::io_service io_service;
      boost::asio::ip::tcp::socket socket;
    };
  }
}

#endif // CNETWORK_H
