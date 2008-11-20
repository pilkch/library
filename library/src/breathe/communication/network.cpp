// Standard libraries
#include <cassert>
#include <cmath>

#include <list>
#include <vector>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

// Other libraries
#ifdef WIN32
#include <windows.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/thread.h>

#include <breathe/math/math.h>

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
      sd(NULL)
    {
    }

    void cConnectionTCP::Open(const std::string& host, uint32_t port)
    {
      /* Resolve the host we are connecting to */
      if (SDLNet_ResolveHost(&ip, host.c_str(), port) < 0) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
      }

      /* Open a connection with the IP provided (listen on the host's port) */
      if (!(sd = SDLNet_TCP_Open(&ip))) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
      }
    }

    void cConnectionTCP::Close()
    {
      if (IsOpen()) {
        SDLNet_TCP_Close(sd);
        sd = NULL;
      }
    }

    bool cConnectionTCP::IsOpen() const
    {
      return (sd != NULL);
    }

    size_t cConnectionTCP::Send(const void* buffer, size_t len)
    {
      // I'm not sure why, but SDLNet_TCP_Send doesn't take a const void* which
      // is painful as it probably doesn't change the data anyway
      int sentBytes = SDLNet_TCP_Send(sd, (void*)buffer, len);

      // If sentBytes is less than 0, return 0, else return sentBytes
      return (sentBytes < 0) ? 0 : size_t(sentBytes);
    }

    size_t cConnectionTCP::Recv(void* buffer, size_t len)
    {
      // I'm not sure why, but SDLNet_TCP_Send doesn't take a const void* which
      // is painful as it probably doesn't change the data anyway
      int recvBytes = SDLNet_TCP_Recv(sd, buffer, len);

      // If recvBytes is less than 0, return 0, else return recvBytes
      return (recvBytes < 0) ? 0 : size_t(recvBytes);
    }
  }
}
