#ifndef BASE64_H
#define BASE64_H

#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>

namespace spitfire
{
  namespace algorithm
  {
    // Base64
    //
    // Provides base64 encoding/decoding
    //

    std::string Base64Encode(const void* pBuffer, size_t len);
    std::string Base64Encode(const std::string& sText);
    std::string Base64Decode(const std::string& sText);
  }
}

#endif // BASE64_H
