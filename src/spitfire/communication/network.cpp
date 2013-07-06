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
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
      boost::asio::ip::tcp::resolver::iterator end;

      boost::system::error_code error = boost::asio::error::host_not_found;
      while (error && endpoint_iterator != end) {
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
  }
}
