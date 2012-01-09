// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

// Boost headers
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <fcntl.h>
#include <sys/ioctl.h>

// libxdgmm headers
#include <libxdgmm/libxdgmm.h>

// Until C++0x we need to define this ourselves
#define nullptr NULL

namespace xdg
{
  std::string PipeReadToString(const std::string& sCommandLine)
  {
    //std::cout<<"PipeReadToString sCommandLine=\""<<sCommandLine<<"\""<<std::endl;

    FILE* fhPipe = popen(sCommandLine.c_str(), "r");
    if (fhPipe == nullptr) {
      std::cout<<"PipeReadToString pipe is closed"<<std::endl;
      return "";
    }

    int fd = fileno(fhPipe);
    if (fd == -1) std::cout<<"PipeReadToString fd=-1"<<std::endl;
    fcntl(fd, F_SETFD, FD_CLOEXEC); // Make sure it can be inherited

    std::ostringstream o;

    {
      boost::iostreams::stream_buffer<boost::iostreams::file_descriptor_source> fpstream(fd, boost::iostreams::file_descriptor_flags::never_close_handle);
      //boost::iostreams::stream_buffer<boost::iostreams::file_descriptor_source> fpstream(fileno(fhPipe));
      std::istream in(&fpstream);
      // TODO: This is required to automatically close the stream when we are done but isn't supported on early versions of boost
      //in.set_auto_close(false);

      std::string sLine;
      while (in) {
        std::getline(in, sLine);
        //std::cout<<"PipeReadToString \""<<sLine<<"\""<<std::endl;
        o<<sLine;
      }
    }

    pclose(fhPipe);
    fhPipe = nullptr;

    const std::string sBuffer(o.str());

    //std::cout<<"PipeReadToString returning \""<<sBuffer<<"\""<<std::endl;
    return sBuffer;
  }



  bool GetEnvironmentVariable(const std::string& sVariable, std::string& sValue)
  {
    sValue.clear();

    const char* szValue = getenv(sVariable.c_str());
    if (szValue == nullptr) return false;

    sValue = szValue;
    return true;
  }



  // ** cXdg

  cXdg::cXdg() :
    bIsValid(false)
  {
    // Initialise our handle to NULL
    handle.reserved = nullptr;

    // Create our handle
    const xdgHandle* pHandle = xdgInitHandle(&handle);

    // Set our result
    bIsValid = (pHandle != nullptr);
  }

  cXdg::~cXdg()
  {
    // Destroy our handle
    if (bIsValid) xdgWipeHandle(&handle);
  }

  bool cXdg::IsValid() const
  {
    return bIsValid;
  }

  std::string cXdg::GetHomeDirectory()
  {
    if (home.empty()) {
      // Try using xdg first
      home = PipeReadToString("xdg-user-dir");

      if (!home.empty()) {
        // The directory is everything before the first new line or tab
        const size_t n = home.length();
        for (size_t i = 0; i < n; i++) {
          if ((home[i] == '\r') || (home[i] == '\n') || (home[i] == '\t')) {
            home = home.substr(0, i);
          }
        }
      } else {
        // Xdg failed, get the directory from the "HOME" environment variable
        GetEnvironmentVariable("HOME", home);
      }
    }

    return home;
  }

  std::string cXdg::GetHomeDataDirectory()
  {
    std::string sDirectory = xdgDataHome(&handle);
    if (sDirectory.empty() || (sDirectory[0] == 0)) {
      const std::string sHome = GetHomeDirectory();
      sDirectory = sHome + "/.local/share";
    }

    return sDirectory;
  }

  std::string cXdg::GetHomeConfigDirectory()
  {
    std::string sDirectory = xdgConfigHome(&handle);
    if (sDirectory.empty() || (sDirectory[0] == 0)) {
      const std::string sHome = GetHomeDirectory();
      sDirectory = sHome + "/.config";
    }

    return sDirectory;
  }

  std::string cXdg::GetHomeTempDirectory()
  {
    std::string sDirectory = xdgCacheHome(&handle);
    if (sDirectory.empty() || (sDirectory[0] == 0)) {
      const std::string sHome = GetHomeDirectory();
      sDirectory = sHome + "/.cache";
    }

    return sDirectory;
  }


  std::string cXdg::GetOpenErrorString(int result)
  {
    switch (result) {
      case 1: {
        return "Error in command line syntax";
      }
      case 2: {
        return "One of the files passed on the command line did not exist";
      }
      case 3: {
        return "A required tool could not be found";
      }
      case 4: {
        return "The action failed";
      }
    };

    return "";
  }

  int cXdg::OpenFile(const std::string& file)
  {
    return system(("xdg-open " + file).c_str());
  }

  int cXdg::OpenFolder(const std::string& folder)
  {
    return system(("xdg-open " + folder).c_str());
  }

  int cXdg::OpenURL(const std::string& url)
  {
    // URL needs to be wrapped in single quotes
    return system(("xdg-open '" + url + "'").c_str());
  }
}

