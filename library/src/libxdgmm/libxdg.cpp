// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <cassert>

// libxdgmm headers
#include <libxdgmm/libxdg.h>

// Until C++0x we need to define this ourselves
#define nullptr NULL

namespace xdg
{
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


  std::string GetDirectoryErrorString(int result)
  {
    // TODO: Actually fill out these errors
    /*switch (result) {
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
    };*/

    return "";
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


  int GetDataHome(std::string& directory)
  {
    directory.clear();

    if (!GetEnvironmentVariable("XDG_DATA_HOME", directory)) {
      std::string home;
      if (GetEnvironmentVariable("HOME", home)) directory = home + "/.local/share";
    }

    return 0;
  }

  int GetConfigHome(std::string& directory)
  {
    directory.clear();

    if (!GetEnvironmentVariable("XDG_CONFIG_HOME", directory)) {
      std::string home;
      if (GetEnvironmentVariable("HOME", home)) directory = home + "/.config";
    }

    return 0;
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
