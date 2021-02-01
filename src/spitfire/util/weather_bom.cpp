#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <poll.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <gnutls/gnutls.h>

#include <libgnutlsmm/gnutlsmm.h>

#include <spitfire/spitfire.h>
#include <spitfire/communication/http.h>
#include <spitfire/communication/uri.h>
#include <spitfire/storage/file.h>
#include <spitfire/storage/json.h>
#include <spitfire/util/weather_bom.h>

namespace {

// Use the system ca-certificates file
// Unfortunately this can differ in each distribution.  This worked on Ubuntu
const std::string ca_certificates_file_path = "/etc/ssl/certs/ca-certificates.crt";

const size_t MAX_BUF = 4 * 1024; // 4k read buffer

// Get the IP for a hostname
// ie. "www.google.com" might return "172.217.25.132"
std::string hostname_lookup_ip(const std::string& hostname)
{
    const struct hostent* he = gethostbyname(hostname.c_str());
    if (he == nullptr) {
        std::cerr<<"hostname_lookup_ip gethostbyname failed, errno="<<errno<<std::endl;
        return "";
    }

    const struct in_addr** addr_list = (const struct in_addr**)he->h_addr_list;

    for (int i = 0; addr_list[i] != nullptr; i++) {
        // Get the first result
        const std::string ip = inet_ntoa(*addr_list[i]);
        return ip;
    }
    
    return "";
}

class tcp_connection {
public:
    tcp_connection();
    ~tcp_connection();

    bool connect(const std::string& ip, int port);
    void close();

    int get_sd() const { return sd; }

  size_t get_bytes_available() const;

private:
    int sd;
};

tcp_connection::tcp_connection() :
    sd(-1)
{
}

tcp_connection::~tcp_connection()
{
    close();
}

bool tcp_connection::connect(const std::string& ip, int port)
{
    close();

    // Connect to server
    sd = ::socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sa;
    ::memset(&sa, '\0', sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = ::htons(port);
    ::inet_pton(AF_INET, ip.c_str(), &sa.sin_addr);

    const int result = ::connect(sd, (struct sockaddr *) &sa, sizeof(sa));
    return (result >= 0);
}

void tcp_connection::close()
{
    if (sd != -1) {
        ::shutdown(sd, SHUT_RDWR); // No more receptions or transmissions
        ::close(sd);
        sd = -1;
    }
}

size_t tcp_connection::get_bytes_available() const
{
  int bytes_available = 0;
  ::ioctl(sd, FIONREAD, &bytes_available);
  return size_t(std::max(0, bytes_available));
}


enum class POLL_READ_RESULT {
    ERROR,
    DATA_READY,
    TIMED_OUT
};

class poll_read {
public:
    explicit poll_read(int fd);

    POLL_READ_RESULT poll(int timeout_ms);

private:
    struct pollfd fds;
};

poll_read::poll_read(int fd)
{
    // Monitor the fd for input
    fds.fd = fd;
    fds.events = POLLIN;
    fds.revents = 0;
}

POLL_READ_RESULT poll_read::poll(int timeout_ms)
{
    fds.revents = 0;

    const int result = ::poll(&fds, 1, timeout_ms);
    if (result < 0) {
        return POLL_READ_RESULT::ERROR;
    } else if (result > 0) {
        if ((fds.revents & POLLIN) != 0) {
            // Zero it out so we can reuse it for the next call to poll
            fds.revents = 0;
            return POLL_READ_RESULT::DATA_READY;
        }
    }

    return POLL_READ_RESULT::TIMED_OUT;
}


void Download(const std::string& url, spitfire::network::http::cRequestListener& listener)
{
  std::cout<<"Download \""<<url<<"\""<<std::endl;

  const spitfire::network::cURI uri(spitfire::network::http::URLEncode(url));

  const std::string ip = hostname_lookup_ip(uri.GetHost());

  tcp_connection connection;

  gnutlsmm::client_session session;

  session.init();

  // X509 stuff
  gnutlsmm::certificate_credentials credentials;

  credentials.init();

  // Set the trusted ca file
  credentials.set_x509_trust_file(ca_certificates_file_path.c_str(), GNUTLS_X509_FMT_PEM);

  // Put the x509 credentials to the current session
  session.set_credentials(credentials);

  // Set TLS version and cypher priorities
  // https://gnutls.org/manual/html_node/Priority-Strings.html
  // NOTE: No SSL, only TLS1.2
  // TODO: TLS1.3 didn't seem to work, server dependent?
  //session.set_priority ("NORMAL:+SRP:+SRP-RSA:+SRP-DSS:-DHE-RSA:-VERS-SSL3.0:%SAFE_RENEGOTIATION:%LATEST_RECORD_VERSION", nullptr);
  session.set_priority("SECURE128:+SECURE192:-VERS-ALL:+VERS-TLS1.2:%SAFE_RENEGOTIATION", nullptr);

  // connect to the peer
  connection.connect(ip, uri.GetPort());
  session.set_transport_ptr((gnutls_transport_ptr_t)(ptrdiff_t)connection.get_sd());

  // Perform the TLS handshake
  const int result = session.handshake();
  if (result < 0) {
      throw std::runtime_error("Handshake failed, error " + std::to_string(result));
  }

  std::cout << "Handshake completed" << std::endl;

  std::cout << "Sending HTTP request" << std::endl;
  const std::string request =
    "GET /" + uri.GetRelativePath() + " HTTP/1.0\r\n"
    "Host: " + uri.GetHost() + "\r\n"
    "\r\n";
  std::cout<<"Request: \""<<request<<"\""<<std::endl;
  session.send(request.c_str(), request.length());

  std::cout << "Reading response" << std::endl;

  char buffer[MAX_BUF + 1];

  poll_read p(connection.get_sd());

  const int timeout_ms = 2000;

  // Once we start not receiving data we retry 10 times in 100ms and then exit
  size_t no_bytes_retries = 0;
  const size_t max_no_bytes_retries = 10;
  const size_t retries_timeout_ms = 10;

  std::string received_so_far;

  bool reading_headers = true;

  // NOTE: gnutls_record_recv may return GNUTLS_E_PREMATURE_TERMINATION
  // https://lists.gnupg.org/pipermail/gnutls-help/2014-May/003455.html
  // This means the peer has terminated the TLS session using a TCP RST (i.e., called close()).
  // Since gnutls cannot distinguish that termination from an attacker terminating the session it warns you with this error code.

  while (no_bytes_retries < max_no_bytes_retries) {
      // Check if there is already something in the gnutls buffers
      if (session.check_pending() == 0) {
          // There was no gnutls data ready, check the socket
          switch (p.poll(timeout_ms)) {
              case POLL_READ_RESULT::DATA_READY: {
                  // Check if bytes are actually available (Otherwise if we try to read again the gnutls session object goes into a bad state and gnutlsxx throws an exception)
                  if (connection.get_bytes_available() == 0) {
                      //std::cout<<"but no bytes available"<<std::endl;
                      no_bytes_retries++;

                      // Don't hog the CPU
                      spitfire::util::SleepThisThreadMS(retries_timeout_ms);

                      continue;
                  }
              }
              case POLL_READ_RESULT::ERROR: {
                  break;
              }
              case POLL_READ_RESULT::TIMED_OUT: {
                  // We hit the 2 second timeout, we are probably done
                  break;
              }
          }
      }

      const ssize_t result = session.recv(buffer, MAX_BUF);
      if (result == 0) {
          std::cout<<"Peer has closed the TLS connection"<<std::endl;
          break;
      } else if (result < 0) {
          std::cout<<"Read error: "<<gnutls_strerror_name(result)<<" "<<gnutls_strerror(result)<<std::endl;
          break;
      }

      const size_t bytes_read = result;
      //std::cout << "Received " << bytes_read << " bytes" << std::endl;
      if (reading_headers) {
          received_so_far.append(buffer, bytes_read);

          size_t i = received_so_far.find("\r\n\r\n");
          if (i != std::string::npos) {
              std::cout<<"Headers received"<<std::endl;

              // Anything after this was file content
              i += strlen("\r\n\r\n");

              // We are now up to the content
              reading_headers = false;

              std::cout<<"Reading content"<<std::endl;

              // Add to the file content
              listener.OnTextContentReceived(std::string(&received_so_far[i], received_so_far.length() - i));
          }
      } else {
          // Everything else is content
          listener.OnTextContentReceived(std::string(buffer, bytes_read));
      }
  }

  session.bye(GNUTLS_SHUT_RDWR);

  std::cout<<"Finished"<<std::endl;
}

}


namespace spitfire {

namespace util {

namespace weather {

namespace bom {

  class text_content_listener : public spitfire::network::http::cRequestListener {
  public:
    virtual void _OnTextContentReceived(const std::string& sContent) override {
      content = sContent;
    }

    std::string content;
  };

  const std::string API_LOCATIONS = "https://api.weather.bom.gov.au/v1/locations";

  bool ParseSearchLocationsJSONResponse(const std::string& response_json, std::vector<locations_response_entry>& locations)
  {
    locations.clear();

    spitfire::json::cDocument document;
    spitfire::json::reader reader;
    if (!reader.ReadFromStringUTF8(document, response_json)) {
      return false;
    }

    const spitfire::json::cNode* data = document.GetChild("data");
    if ((data == nullptr) || !data->IsTypeArray()) {
      return false;
    }

    const std::vector<spitfire::json::cNode*> data_children = data->GetValueObjectOrArray();
    for (auto&& child : data_children) {
      locations_response_entry item;
      child->GetAttribute("geohash", item.geohash);
      child->GetAttribute("id", item.id);
      child->GetAttribute("name", item.name);
      child->GetAttribute("postcode", item.postcode);
      child->GetAttribute("state", item.state);

      locations.push_back(item);
    }

    return true;
  }

  bool ParseGetWeatherJSONResponse(const std::string& weather_json, weather_reading& reading)
  {
    spitfire::json::cDocument document;
    spitfire::json::reader reader;
    if (!reader.ReadFromStringUTF8(document, weather_json)) {
      return false;
    }

    const spitfire::json::cNode* data = document.GetChild("data");
    if ((data == nullptr) || !data->IsTypeObject()) {
      return false;
    }
  
    // At least the temperature should be present
    data->GetAttribute("temp", reading.temp_celcius);
    data->GetAttribute("temp_feels_like", reading.temp_feels_like_celcius);

    const spitfire::json::cNode* wind = data->GetChild("wind");
    if ((wind != nullptr) && wind->IsTypeObject()) {
      wind->GetAttribute("speed_kilometre", reading.wind.speed_kilometres);
      wind->GetAttribute("direction", reading.wind.direction);
    }

    const spitfire::json::cNode* gust = data->GetChild("gust");
    if ((gust != nullptr) && gust->IsTypeObject()) {
      gust->GetAttribute("speed_kilometre", reading.gust.speed_kilometres);
    }

    data->GetAttribute("rain_since_9am", reading.rain_since_9am_millimetres);
    data->GetAttribute("humidity", reading.humidity);

    return true;
  }

  // Example API calls:
  // https://api.weather.bom.gov.au/v1/locations?search=2615
  // https://api.weather.bom.gov.au/v1/locations?search=Hawker+ACT

  bool SearchLocations(const std::string& search, std::vector<locations_response_entry>& locations)
  {
    locations.clear();

    const std::string url = API_LOCATIONS + "?search=" + search;

    //spitfire::network::http::cHTTP http;
    text_content_listener listener;
    //http.Download(url, spitfire::network::http::METHOD::GET, listener);

    Download(url, listener);

    const std::string response_json = listener.content;
    std::cout<<"Response JSON: "<<response_json<<std::endl;

    return ParseSearchLocationsJSONResponse(response_json, locations);
  }

  // Example API calls:
  // https://api.weather.bom.gov.au/v1/locations/r3dp2m/observations
  bool GetWeatherReading(const std::string& geohash, weather_reading& reading)
  {
    // Trim the geohash to just 6 characters for this request
    const std::string geohash_trimmed = geohash.substr(0, 6);

    const std::string url = API_LOCATIONS + "/" + geohash_trimmed + "/observations";
  
    //spitfire::network::http::cHTTP http;
    text_content_listener listener;
    //http.Download(url, spitfire::network::http::METHOD::GET, listener);

    Download(url, listener);

    const std::string response_json = listener.content;
    std::cout<<"Response JSON: "<<response_json<<std::endl;

    return ParseGetWeatherJSONResponse(response_json, reading);
  }
}

}

}

}
