#include "../Shared/D2Packets.hpp"
#include "Diablo2.hpp"
#include "../Libraries/sdl/SDL_net.h"

#define MAX_PACKET_SIZE	512

namespace Network
{
	static SDLNet_SocketSet gServerSocketSet;
	static SDLNet_SocketSet gClientSocketSet;
	static TCPsocket gServerSocket;
	static TCPsocket gServerPlayerSockets[MAX_PLAYERS_REAL]{ 0 };
	static bool gbServerSocketsInUse[MAX_PLAYERS_REAL]{ 0 };
	static bool gbClientConnected = false;	// whether or not the client is connected to a server
	static bool gbServerRunning = false;	// whether or not the server is listening
	static TCPsocket gClientSocket;
	static char gszPacketBuffer[MAX_PACKET_SIZE]{ 0 };
	static DWORD gdwMaxAllowedPlayers = 0;

	/*
	 *	Send a packet from S->C anonymously.
	 *	@author	eezstreet
	 */
	void SendAnonymousServerPacket(TCPsocket socket, D2Packet* pPacket)
	{
		size_t sendAmount;

		memset(gszPacketBuffer, 0, MAX_PACKET_SIZE);
		sendAmount = pPacket->WriteServer(gszPacketBuffer, MAX_PACKET_SIZE);

		SDLNet_TCP_Send(socket, gszPacketBuffer, sendAmount);
	}

	/*
	 *	Send a packet on the server.
	 *	@author	eezstreet
	 */
	void SendServerPacket(int nClientMask, D2Packet* pPacket)
	{
		size_t sendAmount;

		memset(gszPacketBuffer, 0, MAX_PACKET_SIZE);
		sendAmount = pPacket->WriteServer(gszPacketBuffer, MAX_PACKET_SIZE);
		
		for (int i = 0; i < MAX_PLAYERS_REAL; i++)
		{
			if (!gbServerSocketsInUse[i])
			{ // nothing in use, don't send anything
				continue;
			}

			if (nClientMask == -1 || nClientMask & (1 << i))
			{ // actually send it to this client
				SDLNet_TCP_Send(gServerPlayerSockets[i], gszPacketBuffer, sendAmount);
			}
		}
	}

	/*
	 *	Send a packet on the client.
	 *	@author	eezstreet
	 */
	void SendClientPacket(D2Packet* pPacket)
	{
		size_t sendAmount;
		memset(gszPacketBuffer, 0, MAX_PACKET_SIZE);
		sendAmount = pPacket->WriteClient(gszPacketBuffer, MAX_PACKET_SIZE);

		if (!gbClientConnected)
		{
			return;
		}

		SDLNet_TCP_Send(gClientSocket, gszPacketBuffer, sendAmount);
	}

	/*
	 *	Set the maximum allowed players which are allowed to connect to this game.
	 *	@author	eezstreet
	 */
	void SetMaxPlayerCount(DWORD dwNewPlayerCount)
	{
		D2Packet termConnection;
		termConnection.nPacketType = D2SPACKET_CONNECTIONTERM;

		// Cap the values to be things which aren't extreme
		if (dwNewPlayerCount < 0)
		{
			dwNewPlayerCount = 0;
		}
		else if (dwNewPlayerCount > MAX_PLAYERS_REAL)
		{
			dwNewPlayerCount = MAX_PLAYERS_REAL;
		}

		// If the new count is less than the current, we may need to drop some players.
		if (dwNewPlayerCount < gdwMaxAllowedPlayers)
		{
			for (int i = dwNewPlayerCount; i < gdwMaxAllowedPlayers; i++)
			{
				if (gServerPlayerSockets[i] != nullptr)
				{
					SendAnonymousServerPacket(gServerPlayerSockets[i], &termConnection);
					SDLNet_TCP_Close(gServerPlayerSockets[i]);
					gServerPlayerSockets[i] = nullptr;
				}
			}
		}

		// Actually set the new player count
		gdwMaxAllowedPlayers = dwNewPlayerCount;
	}

	/*
	 *	Listen in on the client socket and try to read a packet.
	 *	Returns the amount of time that we should sleep on this slice.
	 *	@author	eezstreet
	 */
	DWORD ReadClientPackets(DWORD dwTimeout)
	{
		DWORD dwInitialTime = SDL_GetTicks();
		DWORD dwEndTime;
		DWORD dwOffset = 0;
		int nNumActiveSockets;

		if (!gbClientConnected)
		{	// no need to read anything if we're not connected to a server
			return 0;
		}

		// Observe activity on the sockets
		nNumActiveSockets = SDLNet_CheckSockets(gClientSocketSet, dwTimeout);
		if (nNumActiveSockets != 0)
		{	// Activity detected!
			while (SDLNet_SocketReady(gClientSocket))
			{
				// Read up to the max packet size
				int nThisCycle = SDLNet_TCP_Recv(gClientSocket,
					gszPacketBuffer + dwOffset, MAX_PACKET_SIZE);

				while (dwOffset < nThisCycle)
				{
					// Peek at the current byte
					BYTE nPacketType = gszPacketBuffer[dwOffset];
					D2Packet packet{ nPacketType };
					size_t packetLen = packet.ReadClient(gszPacketBuffer + dwOffset, nThisCycle - dwOffset);

					if (packetLen == 0)
					{	// incomplete packet, shift memory
						memmove(gszPacketBuffer, gszPacketBuffer + dwOffset, nThisCycle - dwOffset);
						dwOffset = nThisCycle = 0;
					}
					else
					{	// complete packet, shift offset
						dwOffset += packetLen;

						// TODO: do packet logic here
					}
				}
			}
		}

		// Calculate how much time is leftover for us to sleep for and return that
		dwEndTime = SDL_GetTicks();
		if (dwEndTime - dwInitialTime > dwTimeout)
		{
			return 0;
		}
		return dwTimeout - (dwEndTime - dwInitialTime);
	}

	/*
	 *	Listen in on the server sockets and try to read a packet.
	 *	Returns the amount of time that we should sleep on this slice.
	 *	@author	eezstreet
	 */
	DWORD ReadServerPackets(DWORD dwTimeout)
	{
		DWORD dwInitialTime = SDL_GetTicks();
		DWORD dwEndTime;
		int nNumActiveSockets;

		if (!gbServerRunning)
		{	// no need to read anything if we're not connected to a server
			return 0;
		}

		// Observe activity on the sockets
		nNumActiveSockets = SDLNet_CheckSockets(gServerSocketSet, dwTimeout);
		if (nNumActiveSockets != 0)
		{
			// FIXME: this whole handshake procedure needs to be looked at a little more carefully
			// Check to see if the main server socket has any activity. If so, we might have a new incoming client!
			if (SDLNet_SocketReady(gServerSocket))
			{
				bool bConnectionAccepted = false;

				// Find a free spot on the player list
				for (int i = 0; i < gdwMaxAllowedPlayers; i++)
				{
					if (gServerPlayerSockets[i] == nullptr)
					{
						gServerPlayerSockets[i] = SDLNet_TCP_Accept(gServerSocket);
						bConnectionAccepted = (gServerPlayerSockets[i] != nullptr);
					}
				}

				// If we didn't accept a connection, refuse it!
				if (!bConnectionAccepted)
				{
					// Open a temporary socket to deal with this
					TCPsocket temp;
					temp = SDLNet_TCP_Accept(gServerSocket);

					// ... and then promptly tell them to go away
					if (temp)
					{
						D2Packet termPacket;
						termPacket.nPacketType = D2SPACKET_CONNECTIONTERM;
						SendAnonymousServerPacket(temp, &termPacket);
						SDLNet_TCP_Close(temp);
					}
				}
			}

			// Loop through each client socket now to see if there's been any activity
			for (int i = 0; i < gdwMaxAllowedPlayers; i++)
			{
				DWORD dwOffset = 0;
				while (SDLNet_SocketReady(gServerPlayerSockets[i]))
				{
					// Read up to the max packet size
					int nThisCycle = SDLNet_TCP_Recv(gServerPlayerSockets[i], 
						gszPacketBuffer + dwOffset, MAX_PACKET_SIZE);

					while (dwOffset < nThisCycle)
					{
						// Peek at the current byte
						BYTE nPacketType = gszPacketBuffer[dwOffset];
						D2Packet packet{ nPacketType };
						size_t packetLen = packet.ReadServer(gszPacketBuffer + dwOffset, nThisCycle - dwOffset);

						if (packetLen == 0)
						{	// incomplete packet, shift memory
							memmove(gszPacketBuffer, gszPacketBuffer + dwOffset, nThisCycle - dwOffset);
							dwOffset = nThisCycle = 0;
						}
						else
						{	// complete packet, shift offset
							dwOffset += packetLen;

							// TODO: do packet logic here
						}
					}
				}
			}
		}

		// Calculate how much time is leftover for us to sleep for and return that
		dwEndTime = SDL_GetTicks();
		if (dwEndTime - dwInitialTime > dwTimeout)
		{
			return 0;
		}
		return dwTimeout - (dwEndTime - dwInitialTime);
	}

	/*
	 *	Connect to an external server.
	 *	@author	eezstreet
	 */
	bool ConnectToServer(char* szServerAddress, DWORD dwPort)
	{
		// Try to resolve the server address
		IPaddress ip;
		BYTE* pDotQuad;
		int result;

		if (gbClientConnected)
		{
			return false; // we already are connected ?
		}

		result = SDLNet_ResolveHost(&ip, szServerAddress, dwPort);
		if (result == -1)
		{
			Log::Print(PRIORITY_MESSAGE, "Failed to resolve host %s:%d", szServerAddress, dwPort);
			return false;
		}
		pDotQuad = (BYTE*)&ip.host;
		Log::Print(PRIORITY_MESSAGE, "%s resolved to %d.%d.%d.%d", 
			szServerAddress, pDotQuad[0], pDotQuad[1], pDotQuad[2], pDotQuad[3]);

		// Now actually connect to that IP
		gClientSocket = SDLNet_TCP_Open(&ip);
		if (gClientSocket == nullptr)
		{
			Log::Print(PRIORITY_MESSAGE, "Cannot connect to %s (%d.%d.%d.%d)", 
				szServerAddress, pDotQuad[0], pDotQuad[1], pDotQuad[2], pDotQuad[3]);
			return false;
		}

		// ... and add it to the client socket set
		SDLNet_AddSocket(gClientSocketSet, (SDLNet_GenericSocket)gClientSocket);

		gbClientConnected = true;
		return true;
	}

	/*
	 *	Disconnect from the current server
	 *	@author	eezstreet
	 */
	void DisconnectFromServer()
	{
		if (!gbClientConnected)
		{	// if we aren't actually connected to the server we don't need to do anything
			return;
		}

		// Remove the socket from the socket set
		SDLNet_DelSocket(gClientSocketSet, (SDLNet_GenericSocket)gClientSocket);

		// ...and close it
		SDLNet_TCP_Close(gClientSocket);
		gClientSocket = nullptr;
		gbClientConnected = false;
	}

	/*
	 *	Start listening on the external port. This is done when hosting a game.
	 *	@author	eezstreet
	 */
	void StartListen(DWORD dwPort)
	{
		IPaddress serverIP;
		int result;

		if (gbServerRunning)
		{ // Server is already running, don't do anything
			return; 
		}

		// Resolve the hostname
		result = SDLNet_ResolveHost(&serverIP, nullptr, GAME_PORT);
		if (result == -1)
		{
			Log::Error(__FILE__, __LINE__, "Failed to resolve host");
			return;
		}
		
		// Open the socket
		gServerSocket = SDLNet_TCP_Open(&serverIP);
		if (gServerSocket == nullptr)
		{
			char error[128];

			snprintf(error, 128, "SDLNet_TCP_Open: %s", SDLNet_GetError());
			Log::Error(__FILE__, __LINE__, error);
		}

		// Add it to the socket set
		SDLNet_AddSocket(gServerSocketSet, (SDLNet_GenericSocket)gServerSocket);
		gbServerRunning = true;
	}

	/*
	 *	Stop listening on the server port, and disconnect all clients.
	 *	@author	eezstreet
	 */
	void StopListening()
	{
		if (!gbServerRunning)
		{	// server is not running, no need to do anything here
			return;
		}

		// Remove the main server socket
		if (gServerSocket != nullptr)
		{
			SDLNet_DelSocket(gServerSocketSet, (SDLNet_GenericSocket)gServerSocket);
			SDLNet_TCP_Close(gServerSocket);
			gServerSocket = nullptr;
		}

		// Remove the client sockets, one by one
		for (int i = 0; i < MAX_PLAYERS_REAL; i++)
		{
			if (gServerPlayerSockets[i] != nullptr)
			{
				SDLNet_DelSocket(gServerSocketSet, (SDLNet_GenericSocket)gServerPlayerSockets[i]);
				// FIXME: tell clients that the server has been disconnected?
				SDLNet_TCP_Close(gServerPlayerSockets[i]);
				gServerPlayerSockets[i] = nullptr;
			}
		}

		// Finally, mark the server as no longer running
		gbServerRunning = false;
	}

	/*
	 *	Retrieve the local IP address in UTF-16 encoding.
	 *	@author	eezstreet
	 */
	void GetLocalIP(char16_t* szBuffer, size_t bufferSize, DWORD dwPort)
	{
		int result;
		IPaddress serverIP;
		BYTE* pDotQuad;
		size_t dwOffset = 0;
		size_t dwWritten = 0;

		result = SDLNet_ResolveHost(&serverIP, nullptr, dwPort);
		pDotQuad = (BYTE*)&serverIP.host;

		// Dot quad digits
		for (int i = 0; i < 4; i++)
		{
			D2Lib::qnitoa(pDotQuad[i], szBuffer + dwOffset, bufferSize - dwOffset, 10, dwWritten);
			dwOffset += dwWritten;

			if (dwOffset >= bufferSize)
			{
				return;
			}

			if (i == 3)
			{
				szBuffer[dwOffset++] = u':';
			}
			else
			{
				szBuffer[dwOffset++] = u'.';
			}
		}

		// Port
		D2Lib::qnitoa(dwPort, szBuffer + dwOffset, bufferSize - dwOffset, 10, dwWritten);
	}

	/*
	 *	Start up the networking subsystem.
	 *	@author	eezstreet
	 */
	void Init()
	{
		SDLNet_Init();

		gServerSocketSet = SDLNet_AllocSocketSet(MAX_PLAYERS_REAL + 1);
		Log_ErrorAssert(gServerSocketSet != nullptr);

		gClientSocketSet = SDLNet_AllocSocketSet(1);
		Log_ErrorAssert(gClientSocketSet != nullptr);
	}

	/*
	 *	Shut down the networking subsystem.
	 *	@author	eezstreet
	 */
	void Shutdown()
	{
		SDLNet_FreeSocketSet(gServerSocketSet);
		SDLNet_FreeSocketSet(gClientSocketSet);

		SDLNet_Quit();
	}
}