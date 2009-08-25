// Standard libraries
#include <cassert>
#include <cmath>

#include <string>

#include <iostream>
#include <fstream>

#include <vector>
#include <list>
#include <map>
#include <sstream>

// Boost headers
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

// Other libraries
#ifdef WIN32
#include <windows.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/thread.h>

#include <spitfire/math/math.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/communication/network.h>
#include <breathe/communication/uri.h>

namespace breathe
{
  namespace network
  {
    cRequestStringBuilder::cRequestStringBuilder(const std::string& _uri) :
      uri(_uri)
    {
    }

    void cRequestStringBuilder::AddFormVariableAndValue(const std::string& variable, const std::string& value)
    {
      variables[EncodeString(variable)] = EncodeString(value);
    }

    // a-z, A-Z, 0-9 -> not encoded
    // ' ' -> '+';
    // any thing else -> &xx where xx is the ascii code
    std::string cRequestStringBuilder::EncodeString(const std::string& unencoded) const
    {
      std::ostringstream encoded;

      size_t i = 0;
      const size_t n = uri.length();
      for (; i < n; i++) {
        const char c = uri[i];
        if (isalnum(c)) encoded<<c;
        else if (c == ' ') encoded<<'+';
        else {
          // Needs encoding
          encoded<<'&';
          const int e = int(c);
          if (c < 10) encoded<<'0'<<e;
          else encoded<<e;
        }
      }

      return encoded.str();
    }

    std::string cRequestStringBuilder::GetRequestStringForMethodGet() const
    {
      // If we don't have any variables, then we only need the uri and can return
      if (variables.empty()) return uri;

      // Ok, we have variables, we need to use the uri and add all the variables
      std::ostringstream o;
      o<<uri<<'?';

      std::map<std::string, std::string>::const_iterator iter(variables.begin());
      const std::map<std::string, std::string>::const_iterator iterEnd(variables.end());

      // Add the first one
      if (iter != iterEnd) {
        o<<iter->first<<'='<<iter->second;

        iter++;
      }

      // Add a '&' to split this variable from the previous one, and then add this variable
      while (iter != iterEnd) {
        o<<'&'<<iter->first<<'='<<iter->second;

        iter++;
      }

      return o.str();
    }

    std::string cRequestStringBuilder::GetRequestStringForMethodPost(std::string& outVariables) const
    {
      outVariables.clear();

      // If we don't have any variables, then we only need the uri and can return
      if (variables.empty()) return uri;

      // Ok, we have variables, we need to use the uri and add all the variables
      std::ostringstream o;
      o<<uri<<'?';

      std::map<std::string, std::string>::const_iterator iter(variables.begin());
      const std::map<std::string, std::string>::const_iterator iterEnd(variables.end());

      // Add the first one
      if (iter != iterEnd) {
        o<<iter->first<<'='<<iter->second;

        iter++;
      }

      // Add a '&' to split this variable from the previous one, and then add this variable
      while (iter != iterEnd) {
        o<<'&'<<iter->first<<'='<<iter->second;

        iter++;
      }

      outVariables = o.str();
      return uri;
    }
  }
}
