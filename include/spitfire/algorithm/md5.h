/*
*  RFC 1321 compliant MD5 implementation
*
*
*  Copyright (C) 2001-2003  Christophe Devine
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef CMD5_H
#define CMD5_H

#include <spitfire/util/string.h>

namespace spitfire
{
  namespace algorithm
  {
  class cMD5
  {
  public:
    cMD5();

    bool CalculateForString(const char* szInput);
    bool CalculateForBuffer(const char* pData, size_t len);
    bool CalculateForFile(const std::string& sFilePath);
    bool CheckString(const char* szInput) { return CalculateForString(szInput); }
    bool CheckBuffer(const char* pData, size_t len) { return CalculateForBuffer(pData, len); }
    bool CheckFile(const string_t& sFilePath) { return  CalculateForFile(sFilePath); }

    bool SetResultFromFormatted(const string_t& sMD5Hash);

    bool operator==(const cMD5& rhs) const;
    bool operator!=(const cMD5& rhs) const { return !(*this == rhs); }

    bool operator==(const std::string& rhs) const;
    bool operator!=(const std::string& rhs) const { return !(*this == rhs); }

    std::string GetResult() const;
    string_t GetResultFormatted() const;

  private:
    unsigned char h2d(unsigned char a, unsigned char b) const;

    unsigned char result[16]; // Raw result
    std::string sResult; // Result formatted
  };
  }
}

#endif // CMD5_H
