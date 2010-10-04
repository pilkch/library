// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>

#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

// Boost headers
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#ifdef __LINUX__
#include <dirent.h>
#include <pwd.h>
#include <errno.h>
#include <signal.h>

#ifndef __APPLE__
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#endif

// TODO: Remove these
#include <sys/stat.h>
#include <sys/socket.h>
#elif defined(__WIN__)
#include <windows.h>
#endif


// Spitfire Includes
#include <spitfire/spitfire.h>
#include <spitfire/util/cString.h>

#include <spitfire/platform/operatingsystem.h>
#include <spitfire/platform/pipe.h>

#ifndef FIRESTARTER
#include <spitfire/util/log.h>
#endif

namespace spitfire
{
  namespace platform
  {
    cPipeIn::cPipeIn(const string_t& sCommandLine) :
      fhPipe(nullptr),
      fd(-1)
    {
      Open(sCommandLine);
    }

    cPipeIn::~cPipeIn()
    {
      LOG<<"cPipeIn::~cPipeIn"<<std::endl;
      Close();
    }

    bool cPipeIn::Open(const string_t& sCommandLine)
    {
      LOG<<"cPipeIn::Open: "<<sCommandLine<<std::endl;
      Close();

      fhPipe = popen(string::ToUTF8(sCommandLine).c_str(), "r");
      if (fhPipe == nullptr) {
        LOG<<"cPipeIn::Open popen FAILED, returning false"<<std::endl;
        fd = -1;
        return false;
      }
      fd = fileno(fhPipe);
      if (fd == -1) LOG<<"cPipeIn::Open fd=-1"<<std::endl;
      fcntl(fd, F_SETFD, FD_CLOEXEC); // Make sure it can be inherited

      LOG<<"cPipeIn::Open returning true"<<std::endl;
      return true;
    }

    int cPipeIn::Close()
    {
      int iReturnValueOfCommand = -1;

      if (fhPipe != nullptr) {
        int status = pclose(fhPipe);
        if ((status != -1) && WIFEXITED(status)) iReturnValueOfCommand = WEXITSTATUS(status);

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
      LOG<<"cPipeIn::IsDataReady"<<std::endl;
      ASSERT(IsOpen());
      int iSelectResult = 0;

      fd_set readfds;
      FD_ZERO(&readfds);
      FD_SET(fd, &readfds);
      timeval tv;
      tv.tv_sec = 0;
      tv.tv_usec = 0;
      iSelectResult = select(int(fd) + 1, &readfds, NULL, NULL, &tv);

      LOG<<"cPipeIn::IsDataReady returning "<<(iSelectResult > 0)<<std::endl;
      return (iSelectResult > 0);
    }

    size_t cPipeIn::GetBytesReady() const
    {
      LOG<<"cPipeIn::GetBytesReady fd="<<fd<<std::endl;
      assert(IsOpen());
      u_long nBytesReady = 0;
      #ifdef __APPLE__
      if (ioctlsocket(fd, FIONREAD, &nBytesReady) != 0) LOG<<"cPipeIn::GetBytesReady ioctlsocket error"<<std::endl;
      #else
      int n = 0;
      if (ioctl(fd, FIONREAD, &n) == -1) LOG<<"cPipeIn::GetBytesReady ioctl error"<<std::endl;
      else {
        LOG<<"cPipeIn::GetBytesReady ioctl returned "<<n<<std::endl;
        nBytesReady = n;
      }
      #endif
      ASSERT(int(nBytesReady) >= 0);
      LOG<<"cPipeIn::GetBytesReady returning "<<nBytesReady<<std::endl;
      return nBytesReady;
    }

    size_t cPipeIn::Read(void* Buffer, size_t Length)
    {
      int nRead = read(fd, Buffer, Length);
      if (nRead < 0) {
        LOG<<"cPipeIn::Read FAILED: "<<nRead<<std::endl;
        nRead = 0;
      }
      if (size_t(nRead) < Length) {
        LOG<<"cPipeIn::Read errno="<<errno<<std::endl;
        LOG<<"cPipeIn::Read nRead "<<nRead<<" < Length "<<Length<<std::endl;
      }
      return size_t(nRead);
    }


    // ** cPipeOut

    cPipeOut::cPipeOut(const string_t& sCommandLine) :
      fhPipe(nullptr)
    {
      Open(sCommandLine);
    }

    cPipeOut::~cPipeOut()
    {
      LOG<<"cPipeOut::~cPipeOut"<<std::endl;
      Close();
    }

    bool cPipeOut::Open(const string_t& sCommandLine)
    {
      LOG<<"cPipeOut::Open: "<<sCommandLine<<std::endl;
      Close();

      fhPipe = popen(string::ToUTF8(sCommandLine).c_str(), "w");
      if (fhPipe == NULL) {
        fd = -1;
        return false;
      }
      fd = fileno(fhPipe);
      fcntl(fd, F_SETFD, FD_CLOEXEC); // Make sure it can be inherited
      return true;
    }

    int cPipeOut::Close()
    {
      int iReturnValueOfCommand = -1;

      if (fhPipe != nullptr) {
        int status = pclose(fhPipe);
        if ((status != -1) && WIFEXITED(status)) iReturnValueOfCommand = WEXITSTATUS(status);

        fhPipe = nullptr;
      }

      fd = -1;

      return iReturnValueOfCommand;
    }

    bool cPipeOut::IsOpen() const
    {
      return (fhPipe != nullptr);
    }


    void cPipeOut::Write(const void* Buffer, size_t Length)
    {
      ASSERT(IsOpen());
      write(fd, Buffer, Length);
    }


    // ** cPipeInOut

    cPipeInOut::cPipeInOut(const string_t& sCommandLine) :
      infp(-1),
      outfp(-1)
    {
      Open(sCommandLine);
    }

    cPipeInOut::~cPipeInOut()
    {
      LOG<<"cPipeInOut::~cPipeInOut infp "<<infp<<std::endl;
      Close();
    }

    bool cPipeInOut::Open(const string_t& sCommand)
    {
      Close();

      LOG<<"cPipeInOut::Open sCommand=\""<<sCommand<<"\""<<std::endl;

      int fddirectionin[2];    // sCommand output to our input
      int fddirectionout[2];   // Our output to sCommand input

      if (pipe(fddirectionin) != 0) {
        LOG<<"cPipeInOut::Open pipe in FAILED errno="<<errno<<std::endl;
        return false;
      }
      if (pipe(fddirectionout) != 0) {
        LOG<<"cPipeInOut::Open pipe out FAILED errno="<<errno<<std::endl;
        return false;
      }

      int pid = fork();
      if (pid < 0) {
        LOG<<"cPipeInOut::Open fork FAILED errno="<<errno<<std::endl;
        return false;
      }
      if (pid == 0)  {
        LOG<<"cPipeInOut::Open Child"<<std::endl;
        // We are in the child
        close(fddirectionin[0]);
        dup2(fddirectionin[1], STDOUT_FILENO);
        close(fddirectionin[1]);

        close(fddirectionout[1]);
        dup2(fddirectionout[0], STDIN_FILENO);
        close(fddirectionout[0]);
#ifndef NDEBUG
        // stderr sent to stdout so that it will go to the console
        close(STDERR_FILENO);
        dup2(STDERR_FILENO, STDOUT_FILENO);
#endif

        // Execute the command, replacing this process
        const std::string sCommandUTF8(string::ToUTF8(sCommand));
        execl("/bin/sh", "sh", "-c", sCommandUTF8.c_str(), nullptr);
        LOG<<"cPipeInOut::Open execl FAILED errno="<<errno<<std::endl;
        close(fddirectionin[0]);
        close(fddirectionin[1]);
        close(fddirectionout[0]);
        close(fddirectionout[1]);
        exit(1);
      }

      // We are in the parent
      LOG<<"cPipeInOut::Open Parent child pid="<<pid<<std::endl;
      infp = fddirectionin[0];
      close(fddirectionin[1]);
      fcntl(infp, F_SETFD, FD_CLOEXEC);  // Handles are not inherited

      outfp = fddirectionout[1];
      close(fddirectionout[0]);
      fcntl(outfp, F_SETFD, FD_CLOEXEC);  // Handles are not inherited

      return true;
    }

    int cPipeInOut::Close()
    {
      int iReturnValueOfCommand = 0;

      if (outfp != -1) {
#ifndef NDEBUG
        int iOut = close(outfp);
        LOG<<"cPipeInOut::Close closed write end "<<iOut<<std::endl;
#else
        close(outfp);
#endif
      }


      if (infp != -1) {
        // TODO: can we get the return value of the process we are reading?
        // iReturnValueOfCommand = ...;
#ifndef NDEBUG
        int iIn = close(infp);
        LOG<<"cPipeInOut::Close closed read end "<<iIn<<std::endl;
#else
        close(infp);
#endif
      }

      infp = -1;
      outfp = -1;

      return iReturnValueOfCommand;
    }

    bool cPipeInOut::IsOpen() const
    {
      return ((infp != -1) && (outfp != -1));
    }

    bool cPipeInOut::IsDataReady() const
    {
      int iSelectResult = 0;
      if (IsOpen()) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(infp, &readfds);
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        iSelectResult = select(int(infp) + 1, &readfds, NULL, NULL, &tv);
      }
      return (iSelectResult > 0);
    }

    size_t cPipeInOut::GetBytesReady() const
    {
      #ifndef NDEBUG
      if ((infp == -1) || (outfp == -1)) LOG<<"cPipeInOut::GetBytesReady infp or outfp == -1"<<std::endl;
      #endif
      u_long nBytesReady = 0;
      #ifdef __APPLE__
      if (ioctlsocket(infp, FIONREAD, &nBytesReady) != 0) LOG<<"cPipeInOut::GetBytesReady ioctlsocket error"<<std::endl;
      #else
      int n = 0;
      if (ioctl(infp, FIONREAD, &n) != 0) {
        LOG<<"cPipeInOut::GetBytesReady ioctl error"<<std::endl;
        nBytesReady = n;
      }
      #endif
      ASSERT(int(nBytesReady) >= 0);
      return nBytesReady;
    }

    size_t cPipeInOut::Read(void* Buffer, size_t Length)
    {
      #ifndef NDEBUG
      if (infp == -1) LOG<<"cPipeInOut::Read infp invalid"<<std::endl;
      #endif
      int nRead = read(infp, Buffer, Length);
      if (nRead < 0) {
        LOG<<"cPipeInOut::Read read failed: "<<nRead<<" infp "<<infp<<std::endl;
        nRead = 0;
      }
      if (size_t(nRead) < Length) {
        LOG<<"cPipeInOut::Read read FAILED errno="<<errno<<std::endl;
        LOG<<"cPipeInOut::Read nRead "<<nRead<<" < Length "<<Length<<std::endl;
      }
      return size_t(nRead);
    }

    void cPipeInOut::Write(const void* Buffer, size_t Length)
    {
      #ifndef NDEBUG
      if (outfp == -1) LOG<<"cPipeInOut::Write outfp invalid"<<std::endl;
      #endif
      ssize_t nLengthWritten = write(outfp, Buffer, Length);
      if (nLengthWritten < 0) {
        LOG<<"cPipeInOut::Write write FAILED errno="<<errno<<std::endl;
        LOG<<"cPipeInOut::Write write FAILED error is: "<<errno<<std::endl;
      }
    }





    std::string PipeReadToString(const string_t& sCommandLine, int& iReturnValueOfCommand)
    {
      LOG<<"PipeReadToString sCommandLine=\""<<sCommandLine<<"\""<<std::endl;

      iReturnValueOfCommand = -1;

      FILE* fhPipe = popen(string::ToUTF8(sCommandLine).c_str(), "r");
      if (fhPipe == nullptr) {
        LOG<<"PipeReadToString pipe is closed"<<std::endl;
        return "";
      }

      int fd = fileno(fhPipe);
      if (fd == -1) LOG<<"PipeReadToString fd=-1"<<std::endl;
      fcntl(fd, F_SETFD, FD_CLOEXEC); // Make sure it can be inherited

      std::ostringstream o;

      {
        boost::iostreams::stream_buffer<boost::iostreams::file_descriptor_source> fpstream(fileno(fhPipe));
        std::istream in(&fpstream);
        in.set_auto_close(false);

        std::string sLine;
        while (in) {
          std::getline(in, sLine);
          //std::cout<<"PipeReadToString \""<<sLine<<"\""<<std::endl;
          o<<sLine;
        }
      }

      int status = pclose(fhPipe);
      if ((status != -1) && WIFEXITED(status)) iReturnValueOfCommand = WEXITSTATUS(status);

      fhPipe = nullptr;

      const std::string sBuffer(o.str());

      std::cout<<"PipeReadToString \""<<sBuffer<<"\" returning "<<iReturnValueOfCommand<<std::endl;
      return sBuffer;
    }

    std::string PipeReadToString(const string_t& sCommandLine)
    {
      int iReturnValueOfCommand = -1;
      return PipeReadToString(sCommandLine, iReturnValueOfCommand);
    }


#ifndef NDEBUG
    void PipeTestIn()
    {
      LOG<<"PipeTestIn"<<std::endl;

      std::string sData = PipeReadToString(TEXT("ls -la"));

      LOG<<"PipeTestIn Read \""<<sData<<"\""<<std::endl;

      LOG<<"PipeTestIn returning"<<std::endl;
    }

    void PipeTestInOut()
    {
      LOG<<"PipeTestInOut"<<std::endl;

      // Create a pipe
      cPipeInOut pipe(TEXT("echo"));
      if (!pipe.IsOpen()) {
        LOG<<"PipeTestInOut pipe is closed"<<std::endl;
        return;
      }

      // Write a string that we will hopefully get back when we read our pipe
      const string_t sText(TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
      pipe.Write(sText.data(), sizeof(char_t) * (sText.length()));

      // We keep a single working string
      std::ostringstream o;

      while (pipe.IsDataReady()) {
        // Read the control code
        const size_t n = pipe.GetBytesReady();
        if (n != 0) {
          char szText[n];
          if (pipe.Read(szText, sizeof(szText)) == 0) {
            LOG<<"PipeTestInOut Process terminated without graceful exit"<<std::endl;
            break;
          }

          szText[n] = 0;

          o<<szText;
        }
      }

      LOG<<"PipeTestInOut Read \""<<o.str()<<"\""<<std::endl;
    }
#endif
  }
}
