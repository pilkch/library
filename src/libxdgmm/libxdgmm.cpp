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
#include <boost/filesystem.hpp>
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

    bool DirectoryExists(const std::string& sFolderName)
    {
      const boost::filesystem::path folder(sFolderName);
      return boost::filesystem::exists(folder);
    }


  bool GetEnvironmentVariable(const std::string& sVariable, std::string& sValue)
  {
    sValue.clear();

    const char* szValue = getenv(sVariable.c_str());
    if (szValue == nullptr) return false;

    sValue = szValue;
    return true;
  }

  // NOTE: For KDE we have KDE_FULL_SESSION

  bool IsDesktopGnome()
  {
    // Return true if the desktop session is gnome
    std::string sValue;
    GetEnvironmentVariable("DESKTOP_SESSION", sValue);
    if (sValue == "gnome-shell") return true;

    // Return true if the gnome desktop session id is set
    return GetEnvironmentVariable("GNOME_DESKTOP_SESSION_ID", sValue);
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

  std::string cXdg::GetDirectory(const std::string& sTag) const
  {
    // Try using xdg first
    std::string sDirectory = PipeReadToString("xdg-user-dir " + sTag);

    if (!sDirectory.empty()) {
      // The directory is everything before the first new line or tab
      const size_t n = sDirectory.length();
      for (size_t i = 0; i < n; i++) {
        if ((sDirectory[i] == '\r') || (sDirectory[i] == '\n') || (sDirectory[i] == '\t')) {
          sDirectory = sDirectory.substr(0, i);
          break;
        }
      }
    }

    if (sDirectory.empty()) {
      // Xdg failed, get the directory from the environment variable for this tag
      GetEnvironmentVariable(sTag.c_str(), sDirectory);
    }

    return sDirectory;
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
            break;
          }
        }
      }

      if (home.empty()) {
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

  std::string cXdg::GetHomeDesktopDirectory()
  {
    const std::string sHome = GetHomeDirectory();
    std::string sDirectory = sHome + "/Desktop";
    if (!DirectoryExists(sDirectory)) sDirectory = GetDirectory("DESKTOP");

    return sDirectory;
  }

  std::string cXdg::GetHomeDownloadsDirectory()
  {
    const std::string sHome = GetHomeDirectory();
    std::string sDirectory = sHome + "/Downloads";
    if (!DirectoryExists(sDirectory)) sDirectory = GetDirectory("DOWNLOAD");

    return sDirectory;
  }

  std::string cXdg::GetHomeDocumentsDirectory()
  {
    const std::string sHome = GetHomeDirectory();
    std::string sDirectory = sHome + "/Documents";
    if (!DirectoryExists(sDirectory)) sDirectory = GetDirectory("DOCUMENTS");

    return sDirectory;
  }

  std::string cXdg::GetHomeMusicDirectory()
  {
    const std::string sHome = GetHomeDirectory();
    std::string sDirectory = sHome + "/Music";
    if (!DirectoryExists(sDirectory)) sDirectory = GetDirectory("MUSIC");

    return sDirectory;
  }

  std::string cXdg::GetHomePicturesDirectory()
  {
    const std::string sHome = GetHomeDirectory();
    std::string sDirectory = sHome + "/Pictures";
    if (!DirectoryExists(sDirectory)) sDirectory = GetDirectory("PICTURES");

    return sDirectory;
  }

  std::string cXdg::GetHomeVideosDirectory()
  {
    const std::string sHome = GetHomeDirectory();
    std::string sDirectory = sHome + "/Videos";
    if (!DirectoryExists(sDirectory)) sDirectory = GetDirectory("VIDEOS");

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

  int cXdg::OpenFile(const std::string& sFilePathUTF8)
  {
    return system(("xdg-open \'" + sFilePathUTF8 + "\'").c_str());
  }

  int cXdg::OpenFolder(const std::string& sFolderPathUTF8)
  {
    return system(("xdg-open \'" + sFolderPathUTF8 + "\'").c_str());
  }

  int cXdg::OpenFolderHighlightFile(const std::string& sFilePathUTF8)
  {
    // Nautilus does a better job of this than xdg-open so we try to use that where possible
    if (IsDesktopGnome()) return system(("nautilus \'" + sFilePathUTF8 + "\'").c_str());

    return OpenFolder(sFilePathUTF8);
  }

  int cXdg::OpenURL(const std::string& sURLUTF8)
  {
    // URL needs to be wrapped in single quotes
    return system(("xdg-open '" + sURLUTF8 + "'").c_str());
  }
}
