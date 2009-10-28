#ifndef CURI_H
#define CURI_H

namespace breathe
{
  namespace network
  {
    void BuildWebSiteURL(string_t& sURL);
    void BuildWebSiteProgramURL(string_t& sURL);
    void BuildWebSiteProgramURLWithSubFolderOrFile(string_t& sURL, const string_t& sSubFolderOrFile);

    typedef uint16_t port_t;

    class cURI
    {
    public:
      enum class PROTOCOL {
        HTTP,
        HTTPS,
        FTP,
        UNKNOWN

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
      explicit cURI(const std::string& full_uri);

      cURI& operator=(const cURI& uri);

      void Parse(const std::string& full_uri) { _Clear(); _Parse(full_uri); }

      bool IsValidProtocol() const { return (protocol != PROTOCOL::UNKNOWN); }
      bool IsValidUsername() const { return bIsValidUsername; }
      bool IsValidPassword() const { return IsValidUsername() && bIsValidPassword; }
      bool IsValidServer() const { return (!server.empty()); }
      bool IsValidPort() const { return (port != 0); }
      bool IsValidPath() const { return true; } // TODO: This obviously doesn't do anything

      PROTOCOL GetProtocol() const { return protocol; }
      const std::string& GetUsername() const { return username; }
      const std::string& GetPassword() const { return password; }
      const std::string& GetServer() const { return server; }
      port_t GetPort() const { return port; }
      const std::string& GetRelativePath() const { return relativePath; }
      std::string GetFullURI() const;

      static std::string GetStringFromProtocol(PROTOCOL protocol);
      static PROTOCOL GetProtocolFromString(const std::string& protocol);
      static port_t GetPortFromProtocol(PROTOCOL protocol);

    private:
      void _Assign(const cURI& rhs);
      void _Clear();
      void _Parse(const std::string& full_uri);
      PROTOCOL _GetProtocol(const std::string& strProtocol) const;

      PROTOCOL protocol;
      bool bIsValidUsername;
      std::string username;
      bool bIsValidPassword;
      std::string password;
      std::string server;
      port_t port;
      std::string relativePath;
    };

    inline cURI::cURI() :
      protocol(PROTOCOL::UNKNOWN),
      bIsValidUsername(false),
      bIsValidPassword(false),
      port(0)
    {
    }

    inline cURI::cURI(const std::string& full_uri) :
      protocol(PROTOCOL::UNKNOWN),
      bIsValidUsername(false),
      bIsValidPassword(false),
      port(0)
    {
      _Parse(full_uri);
    }

    inline cURI::cURI(const cURI& rhs)
    {
      _Assign(rhs);
    }

    inline cURI& cURI::operator=(const cURI& rhs)
    {
      _Assign(rhs);
      return *this;
    }

    inline void cURI::_Assign(const cURI& rhs)
    {
      protocol = rhs.protocol;
      bIsValidUsername = rhs.bIsValidUsername;
      username = rhs.username;
      bIsValidPassword = rhs.bIsValidPassword;
      password = rhs.password;
      server = rhs.server;
      port = rhs.port;
      relativePath = rhs.relativePath;
    }

    inline void cURI::_Clear()
    {
      bIsValidUsername = false;
      bIsValidPassword = false;
      protocol = PROTOCOL::UNKNOWN;
      port = 0;
    }

    inline void cURI::_Parse(const std::string& full_uri)
    {
      if (full_uri.empty()) return;

      std::string strProtocol = breathe::string::StripAfterInclusive(full_uri, "://");
      PROTOCOL tempProtocol = GetProtocolFromString(strProtocol);
      if (tempProtocol == PROTOCOL::UNKNOWN) return;

      // Ok, we have a valid protocol, lets continue, we'll at least get the protocol and port correct
      protocol = tempProtocol;
      port = GetPortFromProtocol(protocol);
      server = breathe::string::StripAfterInclusive(breathe::string::StripBeforeInclusive(full_uri, "://"), "/");

      // We want a relativePath in the form "", "folder/" or "folder/file.txt"
      relativePath = breathe::string::StripAfterInclusive(full_uri, "://");
    }


    inline std::string cURI::GetStringFromProtocol(PROTOCOL protocol)
    {
      if (protocol == PROTOCOL::HTTP) return "http";
      if (protocol == PROTOCOL::HTTPS) return "https";
      if (protocol == PROTOCOL::FTP) return "ftp";

      return "unknown";
    }

    inline cURI::PROTOCOL cURI::GetProtocolFromString(const std::string& protocol)
    {
      const std::string lowerProtocol(breathe::string::ToLower(protocol));
      if (lowerProtocol == "http") return PROTOCOL::HTTP;
      if (lowerProtocol == "https") return PROTOCOL::HTTPS;
      if (lowerProtocol == "ftp") return PROTOCOL::FTP;

      return PROTOCOL::UNKNOWN;
    }

    inline port_t cURI::GetPortFromProtocol(cURI::PROTOCOL protocol)
    {
      if (protocol == PROTOCOL::HTTP) return 80;
      if (protocol == PROTOCOL::HTTPS) return 443;
      if (protocol == PROTOCOL::FTP) return 21;

      return 0;
    }

    inline std::string cURI::GetFullURI() const
    {
      ASSERT(IsValidProtocol());
      ASSERT(IsValidServer());
      ASSERT(IsValidPort());

      std::string full_uri(GetStringFromProtocol(protocol) + "://");

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

      void AddFormVariableAndValue(const std::string& variable, const std::string& value);

      std::string GetRequestStringForMethodGet() const;
      std::string GetRequestStringForMethodPost(std::string& outVariables) const;

    private:
      std::string EncodeString(const std::string& unencoded) const;

      std::string uri;
      std::map<std::string, std::string> variables;
    };
  }
}

#endif // CURI_H
