// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <filesystem>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include <fcntl.h>
#include <sys/ioctl.h>

// libxdgmm headers
#include <libxdgmm/libxdgmm.h>

namespace xdg
{
    bool DirectoryExists(const std::string& sFolderName)
    {
      const std::filesystem::path folder(sFolderName);
      return std::filesystem::exists(folder);
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
    std::string sDirectory;

    // Xdg failed, get the directory from the environment variable for this tag
    GetEnvironmentVariable(sTag.c_str(), sDirectory);

    return sDirectory;
  }

  std::string cXdg::GetHomeDirectory()
  {
    if (home.empty()) {
      // Xdg failed, get the directory from the "HOME" environment variable
      GetEnvironmentVariable("HOME", home);
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
    return system(("xdg-open \"" + sFilePathUTF8 + "\"").c_str());
  }

  int cXdg::OpenFolder(const std::string& sFolderPathUTF8)
  {
    return system(("xdg-open \"" + sFolderPathUTF8 + "\"").c_str());
  }

  int cXdg::OpenFolderHighlightFile(const std::string& sFilePathUTF8)
  {
    // Nautilus does a better job of this than xdg-open so we try to use that where possible
    if (IsDesktopGnome()) return system(("nautilus \"" + sFilePathUTF8 + "\"").c_str());

    return OpenFolder(sFilePathUTF8);
  }

  int cXdg::OpenURL(const std::string& sURLUTF8)
  {
    // The URL needs to be wrapped in single quotes
    return system(("xdg-open '" + sURLUTF8 + "'").c_str());
  }


  bool ScreenSaverInhibit()
  {
    return (system("xdg-screensaver reset") == 0);
  }


  std::string ToString(size_t value)
  {
    std::ostringstream o;
    o<<value;
    return o.str();
  }


  cScreenSaverInhibit::cScreenSaverInhibit(size_t _XWindowsWindowID) :
    XWindowsWindowID(_XWindowsWindowID)
  {
    system(("xdg-screensaver suspend " + ToString(XWindowsWindowID)).c_str());
  }

  cScreenSaverInhibit::~cScreenSaverInhibit()
  {
    system(("xdg-screensaver resume " + ToString(XWindowsWindowID)).c_str());
  }
}
