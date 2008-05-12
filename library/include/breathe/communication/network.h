#ifndef CNETWORK_H
#define CNETWORK_H

namespace breathe
{
  namespace network
  {
    // Maximum Packet Size
    const int MAXTCPLEN = 128;

    typedef uint16_t port_t;

		// Outgoing Communication Packet
		struct Packet_Message_Out {
			Uint8	Packet_Length;		// Length of packet being sent
			Uint8	Packet_Type;
			char	Message[126];		// message, null terminated
		};

		// Incoming Communication Packets
		//  Two packets are required, the first is always one byte long
		//  and contains the size of the following packet plus itself
		struct Packet_Sizer {
			Uint8	Packet_Length;		// Length of next packet
		};

		struct Packet_Message_In {
			Uint8	Packet_Type;
			char	Message[126];		// message, null terminated
		};

		/*************************************************
		** Type - use
		**  1 - login from client to server
		**  2 - login confirmed from server to client
		**  3 - login failed from server to client
		** 10 - message from client to server
		** 11 - message from server to client
		** 99 - logoff from client to server
		*************************************************/

		bool Init();
		void Destroy();



    // For choosing a static port look at this list
    // http://en.wikipedia.org/wiki/List_of_well-known_ports_(computing)

    // Random dynamic port between 49152 and 65535
    inline port_t GetDynamicPort()
    {
      return 49152 + math::random(16383);
    }


		class cConnectionTCP
		{
		public:
			cConnectionTCP();

			void Open(const std::string& host, uint32_t port);
			void Close();

			IPaddress ip;
			TCPsocket sd;
		};


		class cDownloadHTTP : public breathe::util::cThread
		{
		public:
			void Download(const std::string& path);
			std::string GetContent() const;


		private:
			int ThreadFunction();

			std::string path;
			std::string server;
			std::string content;

			breathe::network::cConnectionTCP connection;
		};
	}
}

#endif //CNETWORK_H
