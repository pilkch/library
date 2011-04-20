#ifndef BASE64_H
#define BASE64_H

#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>

namespace spitfire
{
  namespace algorithm
  {
    // Base64
    //
    // Provides base64 encoding/decoding
    //

    std::string Base64Encode(const unsigned char* pBuffer, unsigned int len);
    std::string Base64Encode(const std::string& sText) { return Base64Encode(static_cast<const unsigned char*>(sText.c_str()), sText.length()); }
    std::string Base64Decode(const std::string& sText);
  }
}

#endif // BASE64_H
