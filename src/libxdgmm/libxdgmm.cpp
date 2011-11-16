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



  bool IsInstalled()
  {
    return (!PipeReadToString("which xdg-user-dir").empty() && !PipeReadToString("which xdg-open").empty());
  }


  // TODO: There are more of these to add
  // xdg-user-dir --help

  // XDG_DESKTOP_DIR=/home/chris/Desktop
  // XDG_DOCUMENTS_DIR=/home/chris/
  // XDG_DOWNLOAD_DIR=/home/chris/
  // XDG_MUSIC_DIR=/home/chris/
  // XDG_PICTURES_DIR=/home/chris/
  // XDG_PUBLICSHARE_DIR=/home/chris/Public
  // XDG_TEMPLATES_DIR=/home/chris/Templates
  // XDG_VIDEOS_DIR=/home/chris/


  void GetHomeDirectory(std::string& directory)
  {
    directory = PipeReadToString("xdg-user-dir");

    if (!directory.empty()) {
      // The directory is everything before the first new line or tab
      const size_t n = directory.length();
      for (size_t i = 0; i < n; i++) {
        if ((directory[i] == '\r') || (directory[i] == '\n') || (directory[i] == '\t')) {
          directory = directory.substr(0, i);
        }
      }
    } else {
      std::string home;
      if (GetEnvironmentVariable("HOME", home)) directory = home;
    }
  }

  void GetDataHomeDirectory(std::string& directory)
  {
    directory.clear();

    if (!GetEnvironmentVariable("XDG_DATA_HOME", directory)) {
      std::string home;
      GetHomeDirectory(home);
      directory = home + "/.local/share";
    }
  }

  void GetConfigHomeDirectory(std::string& directory)
  {
    directory.clear();

    if (!GetEnvironmentVariable("XDG_CONFIG_HOME", directory)) {
      std::string home;
      GetHomeDirectory(home);
      directory = home + "/.config";
    }
  }


  std::string GetOpenErrorString(int result)
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

  int OpenFile(const std::string& file)
  {
    return system(("xdg-open " + file).c_str());
  }

  int OpenFolder(const std::string& folder)
  {
    return system(("xdg-open " + folder).c_str());
  }

  int OpenURL(const std::string& url)
  {
    // URL needs to be wrapped in single quotes
    return system(("xdg-open '" + url + "'").c_str());
  }
}

