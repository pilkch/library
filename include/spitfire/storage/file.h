#ifndef FILE_H
#define FILE_H

// Standard headers
#include <fstream>

// Spitfire headers
#include <spitfire/util/string.h>

#ifdef __WIN__
#pragma push_macro("CreateDirectory")
#undef CreateDirectory
#pragma push_macro("CreateFile")
#undef CreateFile
#pragma push_macro("FileExists")
#undef FileExists
#endif

namespace spitfire
{
  namespace storage
  {
    const string_t sLINE_ENDING_CRLF(TEXT("\r\n"));
    const string_t sLINE_ENDING_LF(TEXT("\n"));

    const string_t sLINE_ENDING_UNIX_AND_LINUX = sLINE_ENDING_LF;
    const string_t sLINE_ENDING_WINDOWS = sLINE_ENDING_CRLF;

    #ifdef __WIN__
    const string_t sLINE_ENDING_NATIVE = sLINE_ENDING_WINDOWS;
    #else
    const string_t sLINE_ENDING_NATIVE = sLINE_ENDING_UNIX_AND_LINUX;
    #endif


    // UTF-8 EF BB BF
    // UTF-16 (big-endian) FE FF
    // UTF-16 (little-endian) FF FE
    // UTF-16BE, UTF-32BE (big-endian) No BOM!
    // UTF-16LE, UTF-32LE (little-endian) No BOM!
    // UTF-32 (big-endian) 00 00 FE FF
    // UTF-32 (little-endian) FF FE 00 00
    // SCSU (compression) 0E FE FF
    enum class BYTEORDER {
      UTF8 = 0,
      UTF16BE,
      UTF16LE,
      UTF32BE,
      UTF32LE,
      INVALID
    };

    BYTEORDER DetectByteOrderMark(const string_t& filename, size_t& bytes);

    void ReadText(const string_t& filename, std::vector<std::string>& contents);
    void ReadText(const string_t& filename, std::vector<std::wstring>& contents);
    void AppendText(const string_t& filename, const std::string& contents);
    void AppendText(const string_t& filename, const std::wstring& contents);


    // For writing to text files
    // http://stackoverflow.com/questions/8689344/portable-end-of-line-newline-in-c/8689547#8689547
    // Open text files in binary mode too
    // For platform specific files such as configuration files, the platform specific new line crlf/lf is prefered

    // ** cFile
    // Provides basic functionality that is shared between cReadFile and cWriteFile
    template <class T>
    class cFile
    {
    public:
      cFile() {}
      virtual ~cFile() {}

      bool IsOpen() const;

      uint64_t GetFileSizeBytes() const;

      uint64_t Tell() const;
      void Seek(uint64_t uBytePosition);
      void SeekOffset(int64_t iBytePosition);
      void SeekEnd(int64_t iBytesRelativeToEnd);

    protected:
      T file;

    private:
      NO_COPY(cFile<T>);
    };


    // ** cReadFile

    class cReadFile : public cFile<std::ifstream>
    {
    public:
      cReadFile();
      explicit cReadFile(const string_t& sFilePath);
      cReadFile(const string_t& sFilePath, uint32_t uTimeoutMS);

      bool Open(const string_t& sFilePath);
      bool Open(const string_t& sFilePath, uint32_t uTimeoutMS);
      void Close();

      BYTEORDER GetByteOrder() const;

      size_t Read(void* pBuffer, size_t uiBufferSizeBytes);

      // Integer reading
      template <class T> bool ReadIntLE(T& value);
      template <class T> bool ReadIntBE(T& value);

      // String reading
      void ReadStringUTF8(string_t& str); // Reads string of str + in UTF8 encoded format
      bool ReadLine(std::string& str);    // Reads a line as a string until \r\n or \r or \n - does not include the trailing \r or \n. 8 bit characters encoding unspecified.
      bool ReadLineUTF8(string_t& str);   // Reads a line as a string until \r\n or \r or \n - does not include the trailing \r or \n. 8 bit characters encoding unspecified.

    private:
      BYTEORDER byteOrder;
    };

    // ** cReadTextFile

    typedef cReadFile cReadTextFile;


    // ** cWriteFile

    class cWriteFile : public cFile<std::ofstream>
    {
    public:
      cWriteFile();
      explicit cWriteFile(const string_t& sFilePath);
      cWriteFile(const string_t& sFilePath, uint32_t uiTimeOutMS);

      bool Open(const string_t& sFilePath);
      bool Open(const string_t& sFilePath, uint32_t uiTimeOutMS);
      void Close();

      size_t Write(const void* pBuffer, size_t uiBufferSizeBytes);

      // Integer writing
      template <class T> void WriteIntLE(T i);
      template <class T> void WriteIntBE(T i);

      // String writing
      bool WriteString(const std::string& str);     // Writes a simple string
      #ifdef UNICODE
      bool WriteString(const string_t& str);        // Writes using UTF8 encoding
      #endif
      bool WriteStringUTF8(const string_t& str);    // Writes a simple string encoded in UTF8
      void WriteStringAnsi(const string_t& str);    // Writes a simple string encoded in the ACP. Not recommended.
      void WriteLine(const std::string& str);       // Uses the OS Native Ending
      void WriteLineUTF8(const string_t& str);      // Uses the OS Native Ending
      void WriteLineCRLF(const std::string& str);   // Uses \r\n at the end of lines. ie. Windows
      void WriteLineLF(const std::string& str);     // Uses \n at the end of lines. ie. Unix
    };

    // ** cWriteTextFile

    typedef cWriteFile cWriteTextFile;


    // ** cWriteFileAppend

    class cWriteFileAppend : public cWriteFile
    {
    public:
      explicit cWriteFileAppend(const string_t& sFilePath);

      bool Open(const string_t& sFilePath);
      bool Open(const string_t& sFilePath, uint32_t uiTimeOutMS);
    };


    // ** cWriteTextFileAppend

    class cWriteTextFileAppend : public cWriteTextFile
    {
    public:
      explicit cWriteTextFileAppend(const string_t& sFilePath);

      bool Open(const string_t& sFilePath);
      bool Open(const string_t& sFilePath, uint32_t uiTimeOutMS);
    };


    // ** Inlines

    template <class T>
    bool cFile<T>::IsOpen() const
    {
      return file.is_open();
    }

    template <class T>
    uint64_t cFile<T>::GetFileSizeBytes() const
    {
      // Remember the current position
      uint64_t position = file.tellg();

      // Skip to the end and get the length
      file.seekg(0, std::ios::end);
      uint64_t length = file.tellg();

      // Return to the original position
      file.seekg(position, std::ios::beg);

      return length;
    }

    template <class T>
    uint64_t cFile<T>::Tell() const
    {
      return file.tellg();
    }

    template <class T>
    void cFile<T>::Seek(uint64_t uBytePosition)
    {
      file.seekg(uBytePosition, std::ios::beg);
    }

    template <class T>
    void cFile<T>::SeekOffset(int64_t iBytePosition)
    {
      file.seekg(iBytePosition, std::ios::cur);
    }

    template <class T>
    void cFile<T>::SeekEnd(int64_t iBytesRelativeToEnd)
    {
      file.seekg(iBytesRelativeToEnd, std::ios::end);
    }
  }
}

#endif // FILE_H
