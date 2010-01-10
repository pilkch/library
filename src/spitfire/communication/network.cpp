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

// Boost headers
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

// Other libraries
#ifdef WIN32
#include <windows.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/thread.h>

#include <spitfire/math/math.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/communication/network.h>


#define STR_LEN 512
#define STR_END "\r\n"

namespace breathe
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
      if (SDLNet_Init() == -1) {
        std::ostringstream t;
        t << "SDLNet_Init: " << SDLNet_GetError();
        LOG.Error("cNetwork::cNetwork", t.str());

        return false;
      }

      LOG.Success("cNetwork", "Init");

      return true;
    }

    void Destroy()
    {
      SDLNet_Quit();

      LOG.Success("cNetwork", "Shutdown");
    }


    // *** cConnectionTCP

    cConnectionTCP::cConnectionTCP() :
      socket(NULL)
    {
    }

    bool cConnectionTCP::Open(const std::string& host, port_t port)
    {
      ASSERT(!IsOpen());

      // Resolve the host we are connecting to
      if (SDLNet_ResolveHost(&ip, host.c_str(), port) < 0) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return false;
      }

      // Open a connection with the IP provided (listen on the host's port)
      socket = SDLNet_TCP_Open(&ip);
      if (socket == NULL) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return false;
      }

      // Create the socket set
      socketSet = SDLNet_AllocSocketSet(1);

      SDLNet_TCP_AddSocket(socketSet, socket);

      return true;
    }

    void cConnectionTCP::Close()
    {
      if (IsOpen()) {
        if (socketSet != 0) {
          SDLNet_TCP_DelSocket(socketSet, socket);
          socketSet = 0;
        }

        SDLNet_TCP_Close(socket);
        socket = NULL;
      }
    }

    bool cConnectionTCP::IsOpen() const
    {
      return (socket != NULL);
    }

    size_t cConnectionTCP::Recv(void* buffer, size_t len, timeoutms_t timeoutMS)
    {
      LOG<<"cConnectionTCP::Recv Reading with timeout "<<timeoutMS<<std::endl;

      ASSERT(IsOpen());

      int nResultSocketsCheck = SDLNet_CheckSockets(socketSet, timeoutMS);
      if (nResultSocketsCheck == -1) {
        LOG<<"cConnectionTCP::Recv SDLNet_CheckSockets returned "<<nResultSocketsCheck<<", error="<<SDLNet_GetError()<<" closing and returning 0"<<std::endl;
        Close();
        return 0;
      } else if (nResultSocketsCheck == 0) {
        LOG<<"cConnectionTCP::Recv Socket had no activity, closing and returning 0"<<std::endl;
        // TODO: This works for http, I'm not sure if this is correct for all protocols?  Would we ever want to try to read and if it fails keep the socket open and try again later?
        Close();
        return 0;
      }

      int nResultSocketsReady = SDLNet_SocketReady(socketSet);
      if (nResultSocketsReady == -1) {
        LOG<<"cConnectionTCP::Recv SDLNet_SocketReady returned "<<nResultSocketsReady<<", error="<<SDLNet_GetError()<<" returning 0"<<std::endl;
        return 0;
      } else if (nResultSocketsReady == 0) {
        LOG<<"cConnectionTCP::Recv Socket is not ready, returning 0"<<std::endl;
        return 0;
      }


      // Ok, actually receive data from the socket
      int recvBytes = SDLNet_TCP_Recv(socket, buffer, int(len));
      if (recvBytes < 0) {
        LOG<<"cConnectionTCP::Recv SDLNet_TCP_Recv returned "<<recvBytes<<", socket is closed or in error, closing and returning 0"<<std::endl;
        Close();
        return 0;
      }

      return size_t(recvBytes);
    }

    size_t cConnectionTCP::Send(const void* buffer, size_t len)
    {
      LOG<<"cConnectionTCP::Send"<<std::endl;

      ASSERT(IsOpen());

      // I'm not sure why, but SDLNet_TCP_Send doesn't take a const void* which
      // is painful as it probably doesn't change the data anyway
      char* pBuffer = new char[len];
      memcpy(pBuffer, buffer, len);
      int sentBytes = SDLNet_TCP_Send(socket, pBuffer, int(len));
      SAFE_DELETE_ARRAY(pBuffer);

      if (sentBytes < 0) {
        Close();
        return 0;
      }

      // If sentBytes is less than 0, return 0, else return sentBytes
      return (sentBytes < 0) ? 0 : size_t(sentBytes);
    }
  }
}
