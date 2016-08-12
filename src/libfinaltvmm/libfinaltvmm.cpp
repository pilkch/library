// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <iostream>
#include <string>
#include <sstream>
#include <map>

// Boost headers
#include <boost/asio.hpp>

// libfinaltvmm headers
#include <libfinaltvmm/libfinaltvmm.h>

namespace finaltv
{
  const std::string STR_END = "\r\n";

  const std::string HOST_NAME = "chris.iluo.net";
  const std::string HOST_FINALTV_ROOT = "/finaltv/";

  std::string ToUTF8(const std::wstring& source)
  {
    std::string result(source.size(), char(0));
    typedef std::ctype<wchar_t> ctype_t;
    const ctype_t& ct = std::use_facet<ctype_t>(std::locale());
#ifdef __LINUX__
    ct.narrow(source.data(), source.data() + source.size(), '0', &(*result.begin()));
#else
    ct.narrow(source.data(), source.data() + source.size(), '\u00B6', &(*result.begin()));
#endif
    return result;
  }

  bool IsSpecialCharacter(char c)
  {
    switch (c) {
      case '*':
      case '-':
      case '.':
      case '_': {
        return true;
      }
    }

    return false;
  }

  char ConvertToHexDigit(char value)
  {
    assert(value < 16);

    if (value < char(10)) return '0' + char(value);

    return 'a' + char(value) - char(10);
  }

  std::string Encode(const std::string& rawString)
  {
    char encodingBuffer[4] = { '%', '\0', '\0', '\0' };

    std::size_t rawLen = rawString.size();

    std::string encodedString;
    encodedString.reserve(rawLen);

    for (std::size_t i = 0; i < rawLen; ++i) {
      char curChar = rawString[i];

      if (curChar == ' ') encodedString += '+';
      else if (isalpha(curChar) || isdigit(curChar) || IsSpecialCharacter(curChar)) encodedString += curChar;
      else {
        unsigned int temp = static_cast<unsigned int>(curChar);

        encodingBuffer[1] = ConvertToHexDigit(temp / 0x10);
        encodingBuffer[2] = ConvertToHexDigit(temp % 0x10);
        encodedString += encodingBuffer;
      }
    }

    return encodedString;
  }


  // ** cDuration

  cDuration::cDuration() :
    minutes(0),
    seconds(0)
  {
  }

  bool cDuration::IsValid() const
  {
    return (seconds < 60);
  }

  uint32_t cDuration::GetMinutes() const
  {
    assert(IsValid());
    return minutes;
  }

  void cDuration::SetMinutes(uint32_t _minutes)
  {
    minutes = _minutes;
    assert(IsValid());
  }

  uint32_t cDuration::GetSeconds() const
  {
    assert(IsValid());
    return seconds;
  }

  void cDuration::SetSeconds(uint32_t _seconds)
  {
    seconds = _seconds;
    assert(IsValid());
  }


  // ** cConnection

  cConnection::cConnection()
  {
  }

  cConnection::~cConnection()
  {
    Close();
  }

  RESULT cConnection::_PostRequest(const std::string& sRelativeURI, const std::map<std::string, std::string>& mValues)
  {
    boost::asio::io_service io_service;

    // Get a list of endpoints corresponding to the server name.
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(HOST_NAME, "http");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    boost::asio::ip::tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && (endpoint_iterator != end)) {
      socket.close();
      socket.connect(*endpoint_iterator++, error);
    }
    if (error) {
      return RESULT::ERROR_SERVER_COULD_NOT_BE_CONTACTED;
    }

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    //std::ostringstream request_stream;


    std::string sRelativeURIWithAnyVariables = sRelativeURI;

    if (!mValues.empty()) {
      std::ostringstream oVariables;
      std::map<std::string, std::string>::const_iterator iter = mValues.begin();
      const std::map<std::string, std::string>::const_iterator iterEnd = mValues.end();
        // Write the first item so that we can safely add "&" between every other item
      if (iter != iterEnd) {
        oVariables<<Encode(iter->first)<<"="<<Encode(iter->second);
        iter++;
      }
      while (iter != iterEnd) {
        oVariables<<"&"<<Encode(iter->first)<<"="<<Encode(iter->second);
        iter++;
      }

      sRelativeURIWithAnyVariables += "?" + oVariables.str();
    }

    request_stream<<"GET "<<HOST_FINALTV_ROOT<<sRelativeURIWithAnyVariables<<" HTTP/1.1"<<STR_END;
    request_stream<<"Host: "<<HOST_NAME<<""<<STR_END;
    request_stream<<"User-Agent: Mozilla/4.0 (compatible; libfinaltv 1.0; Linux)"<<STR_END;
    request_stream<<"Accept: */*"<<STR_END;
    request_stream<<"Connection: close"<<STR_END;
    request_stream<<STR_END;


    //std::cout<<"Sending request: "<<std::endl;
    //std::cout<<request_stream.str()<<std::endl;

    // Send the request.
    boost::asio::write(socket, request);

    // Read the response status line.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || (http_version.substr(0, 5) != "HTTP/")) {
      std::cout<<"Invalid response"<<std::endl;
      return RESULT::ERROR_SERVER_INVALID_RESPONSE;
    }
    if (status_code != 200) {
      std::cout<<"Response returned with status code "<<status_code<<std::endl;
      return RESULT::ERROR_SERVER_INVALID_RESPONSE;
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && (header != "\r")) {
      //std::cout<<header<<"\n";
    }
    //std::cout<<"\n";

    std::ostringstream o;

    // Process the content in this response if there is any
    if (response.size() > 0) {
      // Valid response
      o<<&response;
    }

    // Read until EOF, writing data to output as we go.
    while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
      o<<&response;
    }
    if (error != boost::asio::error::eof) {
      std::cout<<"Error not at the end of the file "<<status_code<<std::endl;
    }

    std::istringstream buffer(o.str());

    std::string sLine;
    while (std::getline(buffer, sLine)) {
      if (sLine == "success") return RESULT::SUCCESS;
      else if (sLine == "error_wrong_version") return RESULT::ERROR_WRONG_VERSION;
      else if (sLine == "error_no_action") return RESULT::ERROR_NO_ACTION;
      else if (sLine == "error_username_or_password_incorrect") return RESULT::ERROR_USERNAME_OR_PASSWORD_INCORRECT;
      else if (sLine == "error_username_banned_24_hours") return RESULT::ERROR_USERNAME_BANNED_24_HOURS;
      else if (sLine == "error_username_banned_6_months") return RESULT::ERROR_USERNAME_BANNED_6_MONTHS;
      else if (sLine == "error_ip_banned_24_hours") return RESULT::ERROR_IP_BANNED_24_HOURS;
      else if (sLine == "error_ip_banned_6_months") return RESULT::ERROR_IP_BANNED_6_MONTHS;
      else if (sLine == "error_requests_too_frequent") return RESULT::ERROR_REQUESTS_TOO_FREQUENT;
    }

    return RESULT::ERROR_SERVER_INVALID_RESPONSE;
  }

  RESULT cConnection::PostRequest(const std::string& sRelativeURI, const string_t& sUserName, const string_t& sPassword, const std::map<std::string, std::string>& _mValues)
  {
    std::map<std::string, std::string> mValues(_mValues);

    mValues["version"] = "1";

    mValues["user"] = ToUTF8(sUserName);
    mValues["pass"] = ToUTF8(sPassword);

    return _PostRequest(sRelativeURI, mValues);
  }

  RESULT cConnection::Open(const string_t& _sUserName, const string_t& _sPassword)
  {
    Close();

    std::map<std::string, std::string> mValues;

    mValues["action"] = "login";

    RESULT result = PostRequest("submit.php", _sUserName, _sPassword, mValues);

    if (result == RESULT::SUCCESS) {
      sUserName = _sUserName;
      sPassword = _sPassword;
    }

    return result;
  }

  std::string ToUTF8(const cDuration& duration)
  {
    std::ostringstream o;
    o<<(duration.GetMinutes() * 60) + duration.GetSeconds();

    return o.str();
  }

  RESULT cConnection::WatchingMovie(const string_t& sTitle, const cDuration& durationThroughMovie, const cDuration& durationOfMovie)
  {
    assert(IsOpen());

    std::map<std::string, std::string> mValues;

    mValues["action"] = "watching";
    mValues["title"] = ToUTF8(sTitle);
    mValues["position_seconds"] = ToUTF8(durationThroughMovie);
    mValues["length_seconds"] = ToUTF8(durationOfMovie);

    RESULT result = PostRequest("submit.php", sUserName, sPassword, mValues);

    return result;
  }

  RESULT cConnection::WatchedMovie(const string_t& sTitle)
  {
    assert(IsOpen());

    std::map<std::string, std::string> mValues;

    mValues["action"] = "watched";
    mValues["title"] = ToUTF8(sTitle);

    RESULT result = PostRequest("submit.php", sUserName, sPassword, mValues);

    return result;
  }

  RESULT cConnection::Close()
  {
    if (IsOpen()) {
      sUserName = TEXT("");
      sPassword = TEXT("");
    }

    return RESULT::SUCCESS;
  }

  bool cConnection::IsOpen() const
  {
    return (!sUserName.empty() && !sPassword.empty());
  }
}
