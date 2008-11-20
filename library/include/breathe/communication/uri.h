#ifndef CURI_H
#define CURI_H

namespace breathe
{
  namespace network
  {
    void BuildWebSiteURL(string_t& sURL);
    void BuildWebSiteProgramURL(string_t& sURL);
    void BuildWebSiteProgramURLWithSubFolderOrFile(string_t& sURL, const string_t& sSubFolderOrFile);

    class cURI
    {
    public:
      typedef uint16_t port_t;

      typedef enum PROTOCOL {
        PROTOCOL_HTTP = 0,
        PROTOCOL_HTTPS,
        PROTOCOL_FTP,
        PROTOCOL_UNKNOWN

        /*
        file://localhost/home/chris/file.txt

        lastfm
        shout
        musicsearch

        rar
        zip

        udf
        smb
        ftps
        ftpx
        */
      };

      cURI();
      cURI(const cURI& uri);
      explicit cURI(const string_t& full_uri);

      cURI& operator=(const cURI& uri);

      void Parse(const string_t& full_uri) { _Clear(); _Parse(full_uri); }

      bool IsValidProtocol() const { return bIsValidProtocol; }
      bool IsValidUsername() const { return bIsValidUsername; }
      bool IsValidPassword() const { return IsValidUsername() && bIsValidPassword; }
      bool IsValidServer() const { return bIsValidServer; }
      bool IsValidPort() const { return (port != 0); }
      bool IsValidPath() const { return true; } // TODO: This obviously doesn't do anything

      PROTOCOL GetProtocol() const { return protocol; }
      const string_t& GetUsername() const { return username; }
      const string_t& GetPassword() const { return password; }
      const string_t& GetServer() const { return server; }
      port_t GetPort() const { return port; }
      const string_t& GetRelativePath() const { return relativePath; }
      string_t GetFullURI() const;

      static string_t GetStringFromProtocol(PROTOCOL protocol);
      static PROTOCOL GetProtocolFromString(const string_t& protocol);
      static port_t GetPortFromProtocol(PROTOCOL protocol);

    private:
      void _Clear();
      void _Parse(const string_t& full_uri);
      PROTOCOL _GetProtocol(const string_t& strProtocol) const;

      bool bIsValidProtocol;
      PROTOCOL protocol;
      bool bIsValidUsername;
      string_t username;
      bool bIsValidPassword;
      string_t password;
      bool bIsValidServer;
      string_t server;
      port_t port;
      string_t relativePath;
    };

    inline cURI::cURI() :
      bIsValidProtocol(false),
      bIsValidUsername(false),
      bIsValidPassword(false),
      bIsValidServer(false),
      protocol(PROTOCOL_UNKNOWN),
      port(0)
    {
    }

    inline cURI::cURI(const string_t& full_uri) :
      bIsValidProtocol(false),
      bIsValidUsername(false),
      bIsValidPassword(false),
      bIsValidServer(false),
      protocol(PROTOCOL_UNKNOWN),
      port(0)
    {
      _Parse(full_uri);
    }

    inline cURI::cURI(const cURI& rhs)
    {
      *this = rhs;
    }

    inline cURI& cURI::operator=(const cURI& rhs)
    {
      bIsValidProtocol = rhs.bIsValidProtocol;
      protocol = rhs.protocol;
      bIsValidUsername = rhs.bIsValidUsername;
      username = rhs.username;
      bIsValidPassword = rhs.bIsValidPassword;
      password = rhs.password;
      bIsValidServer = rhs.bIsValidServer;
      server = rhs.server;
      port = rhs.port;
      relativePath = rhs.relativePath;
    }

    inline void cURI::_Clear()
    {
      bIsValidProtocol = false;
      bIsValidUsername = false;
      bIsValidPassword = false;
      bIsValidServer = false;
      protocol = PROTOCOL_UNKNOWN;
      port = 0;
    }

    inline void cURI::_Parse(const string_t& full_uri)
    {
      if (full_uri.length() == 0) return;

      string_t strProtocol = breathe::string::StripAfterInclusive(full_uri, "://");
      PROTOCOL tempProtocol = GetProtocolFromString(strProtocol);
      if (tempProtocol == PROTOCOL_UNKNOWN) return;

      // Ok, we have a valid protocol, lets continue, we'll at least get the protocol and port correct
      protocol = tempProtocol;
      port = GetPortFromProtocol(protocol);
      server = breathe::string::StripAfterInclusive(breathe::string::StripLeading(full_uri, "://"), "/");

      // We want a relativePath in the form "", "folder/" or "folder/file.txt"
      relativePath = breathe::string::StripAfterInclusive(full_uri, "://");
    }


    inline string_t cURI::GetStringFromProtocol(PROTOCOL protocol)
    {
      if (protocol == PROTOCOL_HTTP) return "http";
      if (protocol == PROTOCOL_HTTPS) return "https";
      if (protocol == PROTOCOL_FTP) return "ftp";

      return "unknown";
    }

    inline cURI::PROTOCOL cURI::GetProtocolFromString(const string_t& protocol)
    {
      const string_t lowerProtocol(breathe::string::ToLower(protocol));
      if (lowerProtocol == "http") return PROTOCOL_HTTP;
      if (lowerProtocol == "https") return PROTOCOL_HTTPS;
      if (lowerProtocol == "ftp") return PROTOCOL_FTP;

      return PROTOCOL_UNKNOWN;
    }

    inline port_t cURI::GetPortFromProtocol(cURI::PROTOCOL protocol)
    {
      if (protocol == PROTOCOL_HTTP) return 80;
      if (protocol == PROTOCOL_HTTPS) return 443;
      if (protocol == PROTOCOL_FTP) return 21;

      return 0;
    }

    inline string_t cURI::GetFullURI() const
    {
      ASSERT(IsValidProtocol());
      ASSERT(IsValidServer());
      ASSERT(IsValidPort());

      string_t full_uri(GetStringFromProtocol(protocol) + "://");

      // Add a username if we have one as well as a password if we have a username and password
      if (IsValidUsername()) {
        full_uri += GetUsername();

        if (IsValidPassword()) full_uri += ":" + GetPassword();

        full_uri += "@";
      }

      full_uri += GetServer();

      // If this is not the default port then we want to add ":80" after a http server for example
      port_t port = GetPort();
      if (port != GetPortFromProtocol(protocol)) full_uri += ":" + port;

      full_uri += "/" + GetRelativePath();

      return full_uri;
    }




    // ** cRequestStringBuilder
    // Example:
    /*
    cRequestStringBuilder builder("/path/to/resource.php");
    builder.AddFormVariableAndValue("name", "Chris");
    builder.AddFormVariableAndValue("colour", "blue");

    std::string request(builder.GetRequestString());
    */

    class cRequestStringBuilder
    {
    public:
      explicit cRequestStringBuilder(const std::string& uri);

      std::string AddFormVariableAndValue(const std::string& variable, const std::string& value);

      std::string GetRequestStringForMethodGet() const;
      std::string GetRequestStringForMethodPost(std::string& outVariables) const;

    private:
      std::string EncodeString(const std::string& unencoded);

      std::string uri;
      std::map<std::string, std::string> variables;
    };
  }
}

#endif // CURI_H
