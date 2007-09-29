// Standard libraries
#include <string>

#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <sstream>

// Other libraries
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/log.h>

#include <breathe/util/thread.h>
#include <breathe/util/cNetwork.h>

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
			char *ptr=&in[40]; //this points into a previously received data buffer
			number=0;
			number=SDLNet_Read32(ptr);
			// number is now in your hosts byte order, ready to use. 
		*/

		bool Init()
		{
			if(SDLNet_Init()==-1) 
			{
				std::ostringstream t;
				t<<"SDLNet_Init: "<<SDLNet_GetError();
				LOG.Error("cNetwork::cNetwork", t.str());

				return false;
			}

			LOG.Success("cNetwork", "Init");

			return true;
		}

		void Destroy()
		{
			if(strlen(SDLNet_GetError()))
			{
				std::ostringstream t;
				t<<"SDLNet_GetError: "<<SDLNet_GetError();
				LOG.Error("cNetwork", t.str());
			}

			SDLNet_Quit();

			LOG.Success("cNetwork", "Shutdown");
		}


		// *** cConnectionTCP

		cConnectionTCP::cConnectionTCP()
		{
		}

		void cConnectionTCP::Open(std::string host, uint32_t port)
		{
			/* Resolve the host we are connecting to */
			if (SDLNet_ResolveHost(&ip, host.c_str(), port) < 0)
			{
				fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
				exit(EXIT_FAILURE);
			}
		 
			/* Open a connection with the IP provided (listen on the host's port) */
			if (!(sd = SDLNet_TCP_Open(&ip)))
			{
				fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
				exit(EXIT_FAILURE);
			}
		}

		void cConnectionTCP::Close()
		{
			SDLNet_TCP_Close(sd);
		}


		
		// *** cDownloadHTTP

		int cDownloadHTTP::ThreadFunction()
		{
			std::cout<<server<<" "<<path<<std::endl;

			connection.Open(server, 80);
			
				#define STR_LEN 512
				char buffer[STR_LEN];
				buffer[0] = 0;
				int len;
				unsigned long ulProgress = 0;

				#define STR_END "\r\n"
				sprintf(buffer, 
					TEXT("GET /%s HTTP/1.1") TEXT(STR_END)
					TEXT("Host: %s") TEXT(STR_END)
					TEXT("Range: bytes=%ld-") TEXT(STR_END)
					TEXT("User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)") TEXT(STR_END)
					TEXT("Accept: */*") TEXT(STR_END)
					TEXT("Accept-Language: en-us") TEXT(STR_END)
					TEXT("Connection: Keep-Alive") TEXT(STR_END)
					TEXT("") TEXT(STR_END)

					, path.c_str(), server.c_str(), ulProgress);

				len = strlen(buffer) + 1;
				if (SDLNet_TCP_Send(connection.sd, (void *)buffer, len) < len)
				{
					fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
					exit(EXIT_FAILURE);
				}

				uint32_t timeout = 32000;
				len = 1;
				std::string content;
				while (len > 0)
				{
					len = SDLNet_TCP_Recv(connection.sd, buffer, STR_LEN - 1);
					if (len > 0)
					{
						buffer[len] = 0;
						content += buffer;
					}
				}

				std::cout<<"CONTENT"<<std::endl;
				std::cout<<content<<std::endl;

			connection.Close();

			return 0;
		}

		void cDownloadHTTP::Download(std::string _path)
		{
			path = _path;
			server = breathe::string::StripAfterInclusive(breathe::string::StripLeading(path, TEXT("http://")), TEXT("/"));
			path = breathe::string::StripBeforeInclusive(_path, server + TEXT("/"));
			if (path.length() < 1) path = "/";

			Run();
		}
	}
}
