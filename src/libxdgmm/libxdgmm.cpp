// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include <fcntl.h>
#include <sys/ioctl.h>

// libxdgmm headers
#include <libxdgmm/libxdgmm.h>

// Until C++0x we need to define this ourselves
#define nullptr NULL

namespace xdg
{
  std::string ReadPipeToString(const std::string& sCommandLine)
  {
    //std::cout<<"ReadPipeToString sCommandLine=\""<<sCommandLine<<"\""<<std::endl;

    FILE* fhPipe = popen(sCommandLine.c_str(), "r");
    if (fhPipe == nullptr) {
      std::cout<<"PipeReadToString pipe is closed"<<std::endl;
      return "";
    }

    int fd = fileno(fhPipe);
    if (fd == -1) std::cout<<"ReadPipeToString fd=-1"<<std::endl;
    fcntl(fd, F_SETFD, FD_CLOEXEC); // Make sure it can be inherited

    std::vector<char> buffer;

    {
      char buf[80];
      const size_t len = sizeof(buf);

      size_t n = len;
      while (n == len) {
        int nRead = read(fd, buf, len);
        if (nRead <= 0) {
          std::cout<<"ReadPipeToString FAILED: "<<nRead<<std::endl;
          break;
        }
        if (size_t(nRead) < len) {
          int iErrno = errno;
          if (iErrno != 0) std::cout<<"ReadPipeToString errno="<<iErrno<<std::endl;
        }

        //std::cout<<"ReadPipeToString nRead "<<nRead<<" < len "<<len<<std::endl;
        n = size_t(nRead);
        for (size_t i = 0; i < n; i++) buffer.push_back(buf[i]);
      }
    }

    pclose(fhPipe);
    fhPipe = nullptr;

    buffer.push_back(0);
    const std::string sBuffer(buffer.data());

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
    return ((system("which xdg-user-dir") == 0) && (system("which xdg-open") == 0));
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
    directory = ReadPipeToString("xdg-user-dir");

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

    std::cout<<"GetHomeDirectory returning \""<<directory<<"\""<<std::endl;
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
