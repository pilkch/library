// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <string>
#include <sstream>

#include <fcntl.h>
#include <sys/ioctl.h>

// libxdgmm headers
#include <libxdgmm/libxdgmm.h>

// Until C++0x we need to define this ourselves
#define nullptr NULL

namespace xdg
{
  // *** cPipeIn

  class cPipeIn {
  public:
    explicit cPipeIn(const std::string& sCommandLine);
    ~cPipeIn();

    bool Open(const std::string& sCommandLine);
    int Close();

    bool IsOpen() const;
    bool IsDataReady() const;

    size_t GetBytesReady() const;
    size_t Read(void* Buffer, size_t Length);

  private:
    FILE* fhPipe;
    int fd;
  };

  cPipeIn::cPipeIn(const std::string& sCommandLine) :
    fhPipe(nullptr),
    fd(-1)
  {
    Open(sCommandLine);
  }

  cPipeIn::~cPipeIn()
  {
    Close();
  }

  bool cPipeIn::Open(const std::string& sCommandLine)
  {
    Close();

    fhPipe = popen(sCommandLine.c_str(), "r");
    if (fhPipe == nullptr) {
      //std::cout<<"cPipeIn::Open popen FAILED, returning false"<<std::endl;
      fd = -1;
      return false;
    }
    fd = fileno(fhPipe);
    //if (fd == -1) std::cout<<"cPipeIn::Open fd=-1"<<std::endl;
    fcntl(fd, F_SETFD, FD_CLOEXEC); // Make sure it can be inherited

    //std::cout<<"cPipeIn::Open returning true"<<std::endl;
    return true;
  }

  int cPipeIn::Close()
  {
    int iReturnValueOfCommand = 0;

    if (fhPipe != nullptr) {
      iReturnValueOfCommand = pclose(fhPipe);
      fhPipe = nullptr;
    }

    fd = -1;

    return iReturnValueOfCommand;
  }

  bool cPipeIn::IsOpen() const
  {
    return (fhPipe != nullptr);
  }

  bool cPipeIn::IsDataReady() const
  {
    assert(IsOpen());
    int iSelectResult = 0;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    iSelectResult = select(int(fd) + 1, &readfds, NULL, NULL, &tv);

    //std::cout<<"cPipeIn::IsDataReady returning "<<(iSelectResult > 0)<<std::endl;
    return (iSelectResult > 0);
  }

  size_t cPipeIn::GetBytesReady() const
  {
    u_long nBytesReady = 0;
    int n = 0;
    if (ioctl(fd, FIONREAD, &n) == -1) {
      //std::cout<<"cPipeIn::GetBytesReady ioctl error"<<std::endl;
    } else {
      //std::cout<<"cPipeIn::GetBytesReady ioctl returned "<<n<<std::endl;
      nBytesReady = n;
    }
    assert(int(nBytesReady) >= 0);
    //std::cout<<"cPipeIn::GetBytesReady returning "<<nBytesReady<<std::endl;
    return nBytesReady;
  }

  size_t cPipeIn::Read(void* Buffer, size_t Length)
  {
    int nRead = read(fd, Buffer, Length);
    if (nRead < 0) {
      //std::cout<<"cPipeIn::Read FAILED: "<<nRead<<std::endl;
      nRead = 0;
    }
    if (size_t(nRead) < Length) {
      //std::cout<<"cPipeIn::Read errno="<<errno<<std::endl;
      //std::cout<<"cPipeIn::Read nRead "<<nRead<<" < Length "<<Length<<std::endl;
    }
    return size_t(nRead);
  }



  std::string ReadPipeToString(const std::string& sCommandLine)
  {
    //std::cout<<"ReadPipeToString sCommandLine=\""<<sCommandLine<<"\""<<std::endl;

    // Create a pipe
    cPipeIn pipe(sCommandLine);
    if (!pipe.IsOpen()) {
      //std::cout<<"ReadPipeToString pipe is closed"<<std::endl;
      return "";
    }

    // We keep a single working string
    std::ostringstream o;

    while (pipe.IsDataReady()) {
      // Read the control code
      const size_t n = pipe.GetBytesReady();
      //std::cout<<"ReadPipeToString "<<n<<" bytes ready"<<std::endl;
      if (n != 0) {
        char szText[n];
        if (pipe.Read(szText, sizeof(szText)) == 0) {
          //std::cout<<"ReadPipeToString Process terminated without graceful exit"<<std::endl;
          break;
        }

        szText[n] = 0;

        o<<szText;
      }
    }

    //std::cout<<"ReadPipeToString Read \""<<o.str()<<"\""<<std::endl;

    return o.str();
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

    if (directory.empty()) {
      std::string home;
      if (GetEnvironmentVariable("HOME", home)) directory = home;
    }
  }

  void GetDataHomeDirectory(std::string& directory)
  {
    directory.clear();

    if (!GetEnvironmentVariable("XDG_DATA_HOME", directory)) {
      std::string home;
      if (GetEnvironmentVariable("HOME", home)) directory = home + "/.local/share";
    }
  }

  void GetConfigHomeDirectory(std::string& directory)
  {
    directory.clear();

    if (!GetEnvironmentVariable("XDG_CONFIG_HOME", directory)) {
      std::string home;
      if (GetEnvironmentVariable("HOME", home)) directory = home + "/.config";
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
