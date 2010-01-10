#ifndef CIPC_H
#define CIPC_H

namespace spitfire
{
  namespace ipc
  {
    typedef uint8_t channelid_t;
    typedef uint8_t connectionid_t;

    class cConnectedClient
    {
    public:
      cConnectedClient(connectionid_t connection);

      connectionid_t GetConnectionID() const { return connection; }

    private:
      connectionid_t connection;
    };

    cConnectedClient::cConnectedClient(connectionid_t _connection) :
      connection(_connection)
    {
    }

    class cServer
    {
    public:
      explicit cServer(channelid_t channel);
      virtual ~cServer() { CloseAll(); }

      bool IsConnected() const;

      void Read(connectionid_t connection, string32_t& sString, size_t len);
      void Write(connectionid_t connection, const string32_t& sString, size_t len);

      void Read(connectionid_t connection, void* pData, size_t len);
      void Write(connectionid_t connection, void* pData, size_t len);

    protected:
      void CloseAll(); // Close all
      void Close(connectionid_t connection); // Close this connection

    private:
      virtual void EvClientConnected(connectionid_t connection) = 0;
      virtual void EvClientDisconnected(connectionid_t connection) = 0;
      virtual void EvDataReceived(connectionid_t connection, size_t len) = 0;

      channelid_t channel;
      std::list<cConnectedClient*> client;

      cConnectionTCP connection;
    };

    cServer::cServer(channelid_t _channel) :
      channel(_channel)
    {
    }

    class cClient
    {
    public:
      cClient();

      bool IsConnected() const;

      void Read(string32_t& sString, size_t len);
      void Write(const string32_t& sString, size_t len);

      void Read(void* pData, size_t len);
      void Write(void* pData, size_t len);

    private:
      virtual void EvDataReceived(size_t len) = 0;
    };
  }
}

#endif // CIPC_H
