#include "Diablo2.hpp"
#include <fcntl.h>
#ifdef WIN32
#include <winsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

typedef handle socket_handle;

namespace Network
{
	static Bitstream* gpPlayerBitstreams[MAX_PLAYERS_REAL];
	socket_handle gSocketHandle;
#ifdef WIN32
	WSADATA gWSAData;
#endif

	/*
	 *	Send a continuous region of memory from server to client.
	 *	@author	eezstreet
	 */
	void SendServerContinuous();

	/*
	 *	Send a number of bits from server to client.
	 *	@author	eez

	/*
	 *	Send a single byte from server to client.
	 *	@author	eezstreet
	 */
	void SendServerByte();

	/*
	 *	Send a single word from server to client.
	 *	@author	eezstreet
	 */
	void SendServerWord();

	/*
	 *	Send a single dword from server to client.
	 *	@author	eezstreet
	 */
	void SendServerDword();

	/*
	 *	Send a continuous region of memory from client to server.
	 *	@author	eezstreet
	 */
	void SendClientContinuous();

	/*
	 *	Send a set of bits from client to server.
	 *	@author	eezstreet
	 */
	void SendClientBits();

	/*
	 *	Send a single byte from client to server.
	 *	@author	eezstreet
	 */
	void SendClientByte();

	/*
	 *	Send a single word from client to server.
	 *	@author	eezstreet
	 */
	void SendClientWord();

	/*
	 *	Send a single dword from client to server.
	 *	@author	eezstreet
	 */
	void SendClientDword();

	/*
	 *	Transmit all waiting packets across the network, and process any incoming data.
	 *	@author	eezstreet
	 */
	// Server version
	void ProcessServerPackets();

	// Client version
	void ProcessClientPackets();

	/*
	 *	Read a number of bits on the server.
	 *	@author	eezstreet
	 */
	bool ReadServerBits();

	/*
	 *	Read a single byte on the server.
	 *	@author	eezstreet
	 */
	bool ReadServerByte();

	/*
	 *	Read a single word on the server.
	 *	@author	eezstreet
	 */
	bool ReadServerWord();

	/*
	 *	Read a single dword on the server.
	 *	@author	eezstreet
	 */
	bool ReadServerDword();

	/*
	 *	Read a single byte on the client.
	 *	@author	eezstreet
	 */
	bool ReadClientByte();

	/*
	 *	Read a single word on the client.
	 *	@author	eezstreet
	 */
	bool ReadClientWord();

	/*
	 *	Read a single dword on the client.
	 *	@author	eezstreet
	 */
	bool ReadClientDword();

	/*
	 *	Connect to an external server.
	 *	@author	eezstreet
	 */
	void ConnectToServer(char* szServerAddress, DWORD dwPort)
	{
		addrinfo* result;
		addrinfo hints{ 0 };
		char port[8]{ 0 };

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		snprintf(port, 8, "%d", dwPort);
		getaddrinfo(szServerAddress, port, &hints, &result);

		freeaddrinfo(result);
	}

	/*
	 *	Disconnect from an external server, or stop listening.
	 *	@author	eezstreet
	 */
	void DisconnectFromServer()
	{
		shutdown(gSocketHandle, SD_SEND);
	}

	/*
	 *	Start listening on the external port. This is done when hosting a game.
	 *	@author	eezstreet
	 */
	void StartListen(DWORD dwPort)
	{
		addrinfo* result;
		addrinfo hints{ 0 };
		char port[8]{ 0 };

		hints.ai_family = AF_INET; // FIXME: IPv6 support?
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;
		snprintf(port, 8, "%d", dwPort);
		getaddrinfo(nullptr, port, &hints, &result);	// resolve address at localhost
		bind(gSocketHandle, result->ai_addr, (int)result->ai_addrlen);	// bind it!
		freeaddrinfo(result); // free the address info
	}

	/*
	 *	Start up the networking subsystem.
	 *	@author	eezstreet
	 */
	void Init()
	{
		// Create all of the necessary bitstreams
		for (int i = 0; i < MAX_PLAYERS_REAL; i++)
		{
			gpPlayerBitstreams[i] = new Bitstream();
		}

		// Start up WinSock (Windows only)
#ifdef WIN32
		WSAStartup(MAKEWORD(2, 2), &gWSAData);
#endif

		// Create the socket.
		// FIXME: IPv6 support?
		gSocketHandle = socket(AF_INET, SOCK_STREAM, 0);

		// Set socket as nonblocking
#ifdef WIN32
		DWORD dwBlocking = 1;
		ioctlsocket(gSocketHandle, FIONBIO, &dwBlocking);
#else
		fcntl(gSocketHandle, F_SETFL, fcntl(gSocketHandle, F_GETFL, 0) | O_NONBLOCK);
#endif
	}

	/*
	 *	Shut down the networking subsystem.
	 *	@author	eezstreet
	 */
	void Shutdown()
	{
		// Shut down WinSock (Windows only)
#ifdef WIN32
		WSACleanup();
#endif
		// Delete the socket
		closesocket(gSocketHandle);

		// Destroy all of the bitstreams
		for (int i = 0; i < MAX_PLAYERS_REAL; i++)
		{
			delete gpPlayerBitstreams[i];
		}
	}
}