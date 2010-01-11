/*************************************************************************
 *                                                                       *
 * libfinaltvmm Library, Copyright (C) 2009 Onwards Chris Pilkington     *
 * All rights reserved.  Web: http://chris.iluo.net                      *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2.1 of the  *
 * License, or (at your option) any later version. The text of the GNU   *
 * General Public License is included with this library in the           *
 * file license.txt.                                                     *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 * See the file GPL.txt for more details.                                *
 *                                                                       *
 *************************************************************************/

#ifndef LIBFINALTVMM_H
#define LIBFINALTVMM_H

// This is a library for accessing the FinalTV server to make movie viewing submissions

#include <string>

#ifndef TEXT
#define _TEXT(s) L##s
#define TEXT(s) _TEXT(s)
#endif

namespace finaltv
{
  typedef std::wstring string_t;

  enum class RESULT {
    SUCCESS,
    ERROR_WRONG_VERSION,
    ERROR_SERVER_COULD_NOT_BE_CONTACTED,   // The FinalTV server could not be contacted due to either the server being down or a network problem on the client end
    ERROR_SERVER_INVALID_RESPONSE,
    ERROR_USERNAME_OR_PASSWORD_INCORRECT,  // This username is invalid or the password for that username is incorrect
    ERROR_USERNAME_BANNED_24_HOURS,        // This username has been banned for 24 hours
    ERROR_USERNAME_BANNED_6_MONTHS,        // This username has been banned for 6 months
    ERROR_IP_BANNED_24_HOURS,              // This ip has been banned for 24 hours
    ERROR_IP_BANNED_6_MONTHS,              // This ip has been banned for 6 months
    ERROR_REQUESTS_TOO_FREQUENT            // The client application is submitting requests too quickly (Usually WatchingMovie or WatchedMovie requests)
  };

  class cDuration
  {
  public:
    cDuration();

    bool IsValid() const;

    uint32_t GetMinutes() const;
    void SetMinutes(uint32_t minutes);
    uint32_t GetSeconds() const;
    void SetSeconds(uint32_t seconds);

  private:
    uint32_t minutes;
    uint32_t seconds;
  };

  class cConnection
  {
  public:
    cConnection();
    ~cConnection();

    bool IsOpen() const;

    RESULT Open(const string_t& sUserName, const string_t& sPassword);
    RESULT WatchingMovie(const string_t& sTitle, const cDuration& durationThroughMovie, const cDuration& durationOfMovie); // This is the movie that the user is currently watching.  It is used for status updates but does not affect the user's viewing history.
    RESULT WatchedMovie(const string_t& sTitle);                                         // This is a movie that the user has just finished watching.  It is used for status updates and also adds the movie to the user's viewing history.
    RESULT Close();

  private:
    static RESULT _PostRequest(const std::string& sRelativeURI, const std::map<std::string, std::string>& mValues);
    static RESULT PostRequest(const std::string& sRelativeURI, const string_t& sUserName, const string_t& sPassword, const std::map<std::string, std::string>& _mValues);

    string_t sUserName;
    string_t sPassword;
  };
}

#endif // LIBFINALTVMM_H
