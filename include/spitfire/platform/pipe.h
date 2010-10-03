#ifndef PIPE_H
#define PIPE_H

namespace spitfire
{
  namespace platform
  {
    // *** cPipeIn

    class cPipeIn {
    public:
      explicit cPipeIn(const string_t& sCommandLine);
      ~cPipeIn();

      bool Open(const string_t& sCommandLine);
      int Close(); // Returns the return value of the command

      bool IsOpen() const;
      bool IsDataReady() const;

      size_t GetBytesReady() const;
      size_t Read(void* Buffer, size_t Length);

    private:
      FILE* fhPipe;
      int fd;
    };


    // *** cPipeOut

    class cPipeOut {
    public:
      explicit cPipeOut(const string_t& sCommandLine);
      ~cPipeOut();

      bool Open(const string_t& sCommandLine);
      int Close(); // Returns the return value of the command

      bool IsOpen() const;

      void Write(const void* Buffer, size_t Length);

    private:
      FILE* fhPipe;
      int fd;
    };


    // *** cPipeInOut
    // Bidirectional Pipe

    class cPipeInOut {
    public:
      explicit cPipeInOut(const string_t& sCommandLine);
      ~cPipeInOut();

      bool Open(const string_t& sCommandLine);
      int Close(); // Returns the return value of the command

      bool IsOpen() const;
      bool IsDataReady() const;

      size_t GetBytesReady() const;
      size_t Read(void* Buffer, size_t Length);

      void Write(const void* Buffer, size_t Length);

    private:
      int infp;
      int outfp;
    };


    // *** PipeReadToString

    std::string PipeReadToString(const string_t& sCommandLine); // Returns any data read
    std::string PipeReadToString(const string_t& sCommandLine, int& iReturnValueOfCommand); // Returns any data read and set iReturnValueOfCommand to the return code of sCommandLine
  }
}

#endif // PIPE_H

