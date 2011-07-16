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

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
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
      LOG.Success("cNetwork", "Init");

      return true;
    }

    void Destroy()
    {
      LOG.Success("cNetwork", "Shutdown");
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

    size_t cConnectionTCP::Recv(void* buffer, size_t len, timeoutms_t timeoutMS)
    {
      LOG<<"cConnectionTCP::Recv Reading with timeout "<<timeoutMS<<std::endl;

      ASSERT(IsOpen());

      boost::system::error_code error;

      const size_t nLength = socket.read_some(boost::asio::buffer(static_cast<char*>(buffer), len), error);

      if (error == boost::asio::error::eof) { // Connection closed cleanly by peer.
        Close();
      } else if (error) {
        LOG<<"ERROR When reading from socket"<<std::endl;
        Close();
      } else {
        ASSERT(nLength <= len);
      }

      return nLength;
    }

    size_t cConnectionTCP::Send(const void* buffer, size_t len)
    {
      LOG<<"cConnectionTCP::Send"<<std::endl;

      ASSERT(IsOpen());

      boost::system::error_code error;

      const size_t nLength = socket.write_some(boost::asio::buffer(static_cast<const char*>(buffer), len), error);

      if (error == boost::asio::error::eof) { // Connection closed cleanly by peer.
        Close();
      } else if (error) {
        LOG<<"ERROR When reading from socket"<<std::endl;
        Close();
      } else {
        ASSERT(nLength == len);
      }

      return nLength;
    }
  }
}
