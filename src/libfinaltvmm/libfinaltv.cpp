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
#include <libfinaltvmm/libfinaltv.h>

// Until C++0x we need to define this ourselves
#define nullptr NULL

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
    request_stream<<"GET "<<HOST_FINALTV_ROOT<<sRelativeURI<<" HTTP/1.0\r\n";
    request_stream<<"Host: "<<HOST_NAME<<"\r\n";
    request_stream<<"Accept: */*\r\n";
    request_stream<<"Connection: close\r\n\r\n";

    // TODO: DO A POST REQUEST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#if 0

    std::ostringstream o;

    o<<"GET /"<<HOST_FINALTV_ROOT<<sRelativeURI<<" HTTP/1.1"<<STR_END;

           o<<"Host: "<<uri.GetServer()<<STR_END;

           o<<"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)"<<STR_END;
           o<<"Accept: */*"<<STR_END;
           o<<"Accept-Language: en-us"<<STR_END;

           o<<"Connection: close"<<STR_END;

           o<<STR_END;


    // For POST we have to do more work
           std::ostringstream oPostValues;
           for (each mValues) {
           oPostValues<<ToURLEncoded(iter->first)<<"=" + ToURLEncoded(iter->second);
  }

           const std::string sPostValues(oPostValues.str());

    // TODO: Obviously this is incorrect we should actually get this value from somewhere
           const size_t content_length = sPostValues.length();
           std::cout<<"cDownloadHTTP::CreateRequest POST is not complete"<<std::endl;
           assert(false);

           o<<"Content-Type: application/x-www-form-urlencoded"<<STR_END;
           o<<"Content-Length: "<<content_length<<STR_END;
           o<<STR_END;

           o<<STR_END;

           o<<sPostValues;

           const std::string sHeader(o.str());

           ... send sHeader;
#endif


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
    mValues["password"] = ToUTF8(sPassword);

    return _PostRequest(sRelativeURI, mValues);
  }

  RESULT cConnection::Open(const string_t& _sUserName, const string_t& _sPassword)
  {
    Close();

    std::map<std::string, std::string> mValues;

    RESULT result = PostRequest("login.php", _sUserName, _sPassword, mValues);

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

    mValues["watching"] = "true";
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

    mValues["watched"] = "true";
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
