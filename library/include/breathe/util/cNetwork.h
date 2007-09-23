#ifndef CNETWORK_H
#define CNETWORK_H

namespace breathe
{
	// Maximum Packet Size
	#define MAXTCPLEN 128

	
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

	class cNetwork
	{
	public:
		cNetwork();
		~cNetwork();

		bool Init();
	};
}

#endif //CNETWORK_H