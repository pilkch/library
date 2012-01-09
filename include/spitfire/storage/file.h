#ifndef FILE_H
#define FILE_H

// Spitfire headers
#include <spitfire/util/cString.h>

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


    // ** cFile

    template <class T>
    class cFile
    {
    public:
      cFile();
      virtual ~cFile();

      bool IsOpen() const;
      std::string GetLine();


      T file;

    protected:
      BYTEORDER byteOrder;

    private:
      NO_COPY(cFile<T>);
    };

    // *** Inlines

    template <class T>
    cFile<T>::cFile()
    {
    }

    template <class T>
    cFile<T>::~cFile()
    {
      if (file.is_open()) file.close();
    }

    template <class T>
    bool cFile<T>::IsOpen() const
    {
      return file.is_open();
    }
  }
}

#endif // FILE_H
