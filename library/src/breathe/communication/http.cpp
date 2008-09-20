// Standard libraries
#include <cassert>
#include <cmath>

#include <string>

#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <sstream>

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
#include <breathe/communication/uri.h>
#include <breathe/communication/http.h>


const size_t STR_LEN = 512;
#define STR_END "\r\n"

namespace breathe
{
  namespace network
  {
    const string_t sWebSiteBaseURL = TEXT("http://chris.iluo.net/");

    const string_t sWebSiteProgramSubFolder = TEXT("drive/");

    void BuildWebSiteURL(string_t& sURL)
    {
      sURL = sWebSiteBaseURL;
    }

    void BuildWebSiteProgramURL(string_t& sURL)
    {
      sURL = sWebSiteBaseURL + sWebSiteProgramSubFolder;
    }

    void BuildWebSiteProgramURLWithSubFolderOrFile(string_t& sURL, const string_t& sSubFolderOrFile)
    {
      sURL = sWebSiteBaseURL + sWebSiteProgramSubFolder + sSubFolderOrFile;
    }


    // *** cDownloadHTTP

		int cDownloadHTTP::ThreadFunction()
		{
			std::cout<<server<<" "<<path<<std::endl;
			content = "";

			connection.Open(server, 80);

				char buffer[STR_LEN];
				buffer[0] = 0;
				int len;
				unsigned long ulProgress = 0;

				sprintf(buffer,
					"GET /%s HTTP/1.1" STR_END
					"Host: %s" STR_END
					"Range: bytes=%ld-" STR_END
					"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)" STR_END
					"Accept: */*" STR_END
					"Accept-Language: en-us" STR_END
					"Connection: Keep-Alive" STR_END
					"" STR_END

					, path.c_str(), server.c_str(), ulProgress);

				len = strlen(buffer) + 1;
				if (!connection.Send(buffer, len)) {
					fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
					exit(EXIT_FAILURE);
				}

				len = 1;
				while (len > 0) {
					len = connection.Recv(buffer, STR_LEN - 1);
					if (len > 0) {
						buffer[len] = 0;
						content += buffer;
					}
				}

				std::cout<<"CONTENT"<<std::endl;
				std::cout<<content<<std::endl;

			connection.Close();

			return 0;
		}

    inline void cDownloadHTTP::Download(const std::string& _path)
    {
      path = _path;
      server = breathe::string::StripAfterInclusive(breathe::string::StripLeading(path, "http://"), "/");
      path = breathe::string::StripBeforeInclusive(_path, server + "/");
      if (path.length() < 1) path = "/";

      Run();
	}
  }
}
