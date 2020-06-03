#include "../Shared/D2Packets.hpp"
#include "Diablo2.hpp"
#include "Logging.hpp"
#include <SDL2_net/SDL_net.h>

#define MAX_PACKET_SIZE	512

// Compression flags
#define NETFLAG_COMPRESSION	0x01
#define NETFLAG_CUSTOM		0x80

/*
 *	Diablo II is a very simple game. Assuming of course, we aren't talking about Battle.net and everything related to it.
 *	Essentially the game's communication is a simple TCP protocol.
 *	Each packet contains a one-byte header consisting of the "type", and variable data following.
 *	This information is transferred over server port 4000. 
 */

namespace Network
{
	static SDLNet_SocketSet gServerSocketSet;
	static SDLNet_SocketSet gClientSocketSet;
	static TCPsocket gServerSocket;
	static TCPsocket gServerPlayerSockets[MAX_PLAYERS_REAL]{};
	static bool gbServerSocketsInUse[MAX_PLAYERS_REAL]{};
	static bool gbClientConnected = false;	// whether or not the client is connected to a server
	static bool gbServerRunning = false;	// whether or not the server is listening
	static TCPsocket gClientSocket;
	static unsigned char gszPacketReadBuffer[MAX_PACKET_SIZE]{};
	static unsigned char gszPacketWriteBuffer[MAX_PACKET_SIZE]{};
	static DWORD gdwMaxAllowedPlayers = 0;

	/*
	 *	Diablo II has the ability to compress packets going from the server to the client.
	 *	The first packet sent by the server indicates the type of compression that will subsequently be used.
	 *	This is a series of flags:
	 *	- 0x01: Compression is present
	 *	- 0x80:	Use custom compression (not used)
	 *	Retail Diablo II -always- uses 0x01 as the type.
	 *	OpenD2 at the moment -always- uses 0x00 (no compression), at the moment.
	 */
	static int gnServerCompression = 0;

	unsigned int gpnIndexTable[] =
	{
		0x0247, 0x0236, 0x0225, 0x0214, 0x0203, 0x01F2, 0x01E1, 0x01D0,
		0x01BF, 0x01AE, 0x019D, 0x018C, 0x017B, 0x016A, 0x0161, 0x0158,
		0x014F, 0x0146, 0x013D, 0x0134, 0x012B, 0x0122, 0x0119, 0x0110,
		0x0107, 0x00FE, 0x00F5, 0x00EC, 0x00E3, 0x00DA, 0x00D1, 0x00C8,
		0x00BF, 0x00B6, 0x00AD, 0x00A8, 0x00A3, 0x009E, 0x0099, 0x0094,
		0x008F, 0x008A, 0x0085, 0x0080, 0x007B, 0x0076, 0x0071, 0x006C,
		0x0069, 0x0066, 0x0063, 0x0060, 0x005D, 0x005A, 0x0057, 0x0054,
		0x0051, 0x004E, 0x004B, 0x0048, 0x0045, 0x0042, 0x003F, 0x003F,
		0x003C, 0x003C, 0x0039, 0x0039, 0x0036, 0x0036, 0x0033, 0x0033,
		0x0030, 0x0030, 0x002D, 0x002D, 0x002A, 0x002A, 0x0027, 0x0027,
		0x0024, 0x0024, 0x0021, 0x0021, 0x001E, 0x001E, 0x001B, 0x001B,
		0x0018, 0x0018, 0x0015, 0x0015, 0x0012, 0x0012, 0x0012, 0x0012,
		0x000F, 0x000F, 0x000F, 0x000F, 0x000C, 0x000C, 0x000C, 0x000C,
		0x0009, 0x0009, 0x0009, 0x0009, 0x0006, 0x0006, 0x0006, 0x0006,

		0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003,
		0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003, 0x0003,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	};

	unsigned char gpnCharacterTable[] =
	{
		0x00, 0x00, 0x01, 0x00, 0x01, 0x04, 0x00, 0xFF, 0x06, 0x00, 0x14, 0x06,
		0x00, 0x13, 0x06, 0x00, 0x05, 0x06, 0x00, 0x02, 0x06, 0x00, 0x80, 0x07,
		0x00, 0x6D, 0x07, 0x00, 0x69, 0x07, 0x00, 0x68, 0x07, 0x00, 0x67, 0x07,
		0x00, 0x1E, 0x07, 0x00, 0x15, 0x07, 0x00, 0x12, 0x07, 0x00, 0x0D, 0x07,
		0x00, 0x0A, 0x07, 0x00, 0x08, 0x07, 0x00, 0x07, 0x07, 0x00, 0x06, 0x07,
		0x00, 0x04, 0x07, 0x00, 0x03, 0x07, 0x00, 0x6C, 0x08, 0x00, 0x51, 0x08,
		0x00, 0x20, 0x08, 0x00, 0x1F, 0x08, 0x00, 0x1D, 0x08, 0x00, 0x18, 0x08,
		0x00, 0x17, 0x08, 0x00, 0x16, 0x08, 0x00, 0x11, 0x08, 0x00, 0x10, 0x08,
		0x00, 0x0F, 0x08, 0x00, 0x0C, 0x08, 0x00, 0x0B, 0x08, 0x00, 0x09, 0x08,
		0x01, 0x96, 0x09, 0x97, 0x09, 0x01, 0x90, 0x09, 0x95, 0x09, 0x01, 0x64,
		0x09, 0x6B, 0x09, 0x01, 0x62, 0x09, 0x63, 0x09, 0x01, 0x56, 0x09, 0x58,
		0x09, 0x01, 0x52, 0x09, 0x55, 0x09, 0x01, 0x4D, 0x09, 0x50, 0x09, 0x01,
		0x45, 0x09, 0x4C, 0x09, 0x01, 0x40, 0x09, 0x43, 0x09, 0x01, 0x31, 0x09,
		0x3B, 0x09, 0x01, 0x28, 0x09, 0x30, 0x09, 0x01, 0x1A, 0x09, 0x25, 0x09,
		0x01, 0x0E, 0x09, 0x19, 0x09, 0x02, 0xE2, 0x0A, 0xE8, 0x0A, 0xF0, 0x0A,
		0xF8, 0x0A, 0x02, 0xC0, 0x0A, 0xC2, 0x0A, 0xCE, 0x0A, 0xE0, 0x0A, 0x02,
		0xA0, 0x0A, 0xA2, 0x0A, 0xB0, 0x0A, 0xB8, 0x0A, 0x02, 0x8A, 0x0A, 0x8F,
		0x0A, 0x93, 0x0A, 0x98, 0x0A, 0x02, 0x81, 0x0A, 0x82, 0x0A, 0x83, 0x0A,
		0x89, 0x0A, 0x02, 0x7C, 0x0A, 0x7D, 0x0A, 0x7E, 0x0A, 0x7F, 0x0A, 0x02,
		0x77, 0x0A, 0x78, 0x0A, 0x79, 0x0A, 0x7A, 0x0A, 0x02, 0x73, 0x0A, 0x74,
		0x0A, 0x75, 0x0A, 0x76, 0x0A, 0x02, 0x6E, 0x0A, 0x6F, 0x0A, 0x70, 0x0A,
		0x72, 0x0A, 0x02, 0x61, 0x0A, 0x65, 0x0A, 0x66, 0x0A, 0x6A, 0x0A, 0x02,
		0x5D, 0x0A, 0x5E, 0x0A, 0x5F, 0x0A, 0x60, 0x0A, 0x02, 0x57, 0x0A, 0x59,
		0x0A, 0x5A, 0x0A, 0x5B, 0x0A, 0x02, 0x4A, 0x0A, 0x4B, 0x0A, 0x4E, 0x0A,
		0x53, 0x0A, 0x02, 0x46, 0x0A, 0x47, 0x0A, 0x48, 0x0A, 0x49, 0x0A, 0x02,
		0x3F, 0x0A, 0x41, 0x0A, 0x42, 0x0A, 0x44, 0x0A, 0x02, 0x3A, 0x0A, 0x3C,
		0x0A, 0x3D, 0x0A, 0x3E, 0x0A, 0x02, 0x36, 0x0A, 0x37, 0x0A, 0x38, 0x0A,
		0x39, 0x0A, 0x02, 0x32, 0x0A, 0x33, 0x0A, 0x34, 0x0A, 0x35, 0x0A, 0x02,
		0x2B, 0x0A, 0x2C, 0x0A, 0x2D, 0x0A, 0x2E, 0x0A, 0x02, 0x26, 0x0A, 0x27,
		0x0A, 0x29, 0x0A, 0x2A, 0x0A, 0x02, 0x21, 0x0A, 0x22, 0x0A, 0x23, 0x0A,
		0x24, 0x0A, 0x03, 0xFB, 0x0B, 0xFC, 0x0B, 0xFD, 0x0B, 0xFE, 0x0B, 0x1B,
		0x0A, 0x1B, 0x0A, 0x1C, 0x0A, 0x1C, 0x0A, 0x03, 0xF2, 0x0B, 0xF3, 0x0B,
		0xF4, 0x0B, 0xF5, 0x0B, 0xF6, 0x0B, 0xF7, 0x0B, 0xF9, 0x0B, 0xFA, 0x0B,
		0x03, 0xE9, 0x0B, 0xEA, 0x0B, 0xEB, 0x0B, 0xEC, 0x0B, 0xED, 0x0B, 0xEE,
		0x0B, 0xEF, 0x0B, 0xF1, 0x0B, 0x03, 0xDE, 0x0B, 0xDF, 0x0B, 0xE1, 0x0B,
		0xE3, 0x0B, 0xE4, 0x0B, 0xE5, 0x0B, 0xE6, 0x0B, 0xE7, 0x0B, 0x03, 0xD6,
		0x0B, 0xD7, 0x0B, 0xD8, 0x0B, 0xD9, 0x0B, 0xDA, 0x0B, 0xDB, 0x0B, 0xDC,
		0x0B, 0xDD, 0x0B, 0x03, 0xCD, 0x0B, 0xCF, 0x0B, 0xD0, 0x0B, 0xD1, 0x0B,
		0xD2, 0x0B, 0xD3, 0x0B, 0xD4, 0x0B, 0xD5, 0x0B, 0x03, 0xC5, 0x0B, 0xC6,
		0x0B, 0xC7, 0x0B, 0xC8, 0x0B, 0xC9, 0x0B, 0xCA, 0x0B, 0xCB, 0x0B, 0xCC,
		0x0B, 0x03, 0xBB, 0x0B, 0xBC, 0x0B, 0xBD, 0x0B, 0xBE, 0x0B, 0xBF, 0x0B,
		0xC1, 0x0B, 0xC3, 0x0B, 0xC4, 0x0B, 0x03, 0xB2, 0x0B, 0xB3, 0x0B, 0xB4,
		0x0B, 0xB5, 0x0B, 0xB6, 0x0B, 0xB7, 0x0B, 0xB9, 0x0B, 0xBA, 0x0B, 0x03,
		0xA9, 0x0B, 0xAA, 0x0B, 0xAB, 0x0B, 0xAC, 0x0B, 0xAD, 0x0B, 0xAE, 0x0B,
		0xAF, 0x0B, 0xB1, 0x0B, 0x03, 0x9F, 0x0B, 0xA1, 0x0B, 0xA3, 0x0B, 0xA4,
		0x0B, 0xA5, 0x0B, 0xA6, 0x0B, 0xA7, 0x0B, 0xA8, 0x0B, 0x03, 0x92, 0x0B,
		0x94, 0x0B, 0x99, 0x0B, 0x9A, 0x0B, 0x9B, 0x0B, 0x9C, 0x0B, 0x9D, 0x0B,
		0x9E, 0x0B, 0x03, 0x86, 0x0B, 0x87, 0x0B, 0x88, 0x0B, 0x8B, 0x0B, 0x8C,
		0x0B, 0x8D, 0x0B, 0x8E, 0x0B, 0x91, 0x0B, 0x03, 0x2F, 0x0B, 0x4F, 0x0B,
		0x54, 0x0B, 0x5C, 0x0B, 0x71, 0x0B, 0x7B, 0x0B, 0x84, 0x0B, 0x85, 0x0B
	};

	unsigned int gpnBitMasks[] =
	{
		0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F,
		0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF
	};

	/*
	 *	Send a packet from S->C anonymously.
	 *	@author	eezstreet
	 */
	void SendAnonymousServerPacket(TCPsocket socket, D2Packet* pPacket)
	{
		size_t sendAmount;

		memset(gszPacketWriteBuffer, 0, MAX_PACKET_SIZE);
		sendAmount = pPacket->WriteServer(gszPacketWriteBuffer, MAX_PACKET_SIZE);

		SDLNet_TCP_Send(socket, gszPacketWriteBuffer, sendAmount);
	}

	/*
	 *	Send a packet on the server.
	 *	@author	eezstreet
	 */
	void SendServerPacket(int nClientMask, D2Packet* pPacket)
	{
		size_t sendAmount;

		memset(gszPacketWriteBuffer, 0, MAX_PACKET_SIZE);
		sendAmount = pPacket->WriteServer(gszPacketWriteBuffer, MAX_PACKET_SIZE);
		
		for (int i = 0; i < MAX_PLAYERS_REAL; i++)
		{
			if (!gbServerSocketsInUse[i])
			{ // nothing in use, don't send anything
				continue;
			}

			if (nClientMask == -1 || nClientMask & (1 << i))
			{ // actually send it to this client
				SDLNet_TCP_Send(gServerPlayerSockets[i], gszPacketWriteBuffer, sendAmount);
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
		size_t sentAmount;
		memset(gszPacketWriteBuffer, 0, MAX_PACKET_SIZE);
		sendAmount = pPacket->WriteClient(gszPacketWriteBuffer, MAX_PACKET_SIZE);

		if (!gbClientConnected)
		{
			return;
		}

		sentAmount = SDLNet_TCP_Send(gClientSocket, gszPacketWriteBuffer, sendAmount);
		if (sentAmount < sendAmount)
		{
			return;
		}
	}

	/*
	 *	Set the maximum allowed players which are allowed to connect to this game.
	 *	@author	eezstreet
	 */
	void SetMaxPlayerCount(DWORD dwNewPlayerCount)
	{
		D2Packet termConnection{};
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
	 *	Get the number of bytes that a compressed packet uses.
	 *	@author Mewgood
	 *	@note	This does not include the size of the compression header.
	 */
	size_t GetCompressedPacketSize(unsigned char* pPacket, size_t& rtHeaderSize)
	{
		if (pPacket[0] < 0xF0)
		{	// Upper 4 bits unoccupied = 1 byte header
			rtHeaderSize = 1;
			return pPacket[0] - 1;
		}
		// Upper 4 bits occupied = mask the amount
		rtHeaderSize = 2;
		return ((pPacket[0] & 0xF0u) << 8u) + pPacket[1] - 2;
	}

	/*
	 *	Decompresses a single packet.
	 *	@author	Mewgood/eezstreet
	 */
	unsigned char* DecompressPacket(unsigned char* pPacket, size_t dwPacketCompressedSize, size_t& rtPacketDecompressedSize)
	{
		static unsigned char gszDecompressedPacket[MAX_PACKET_SIZE]{ 0 };
		unsigned int count = 0x20;
		unsigned int a, b, c, d;
		unsigned int index;

		b = 0;

		rtPacketDecompressedSize = 0;

		while (true)
		{
			while (count >= 0x08 && dwPacketCompressedSize > 0)
			{
				count -= 0x08;
				dwPacketCompressedSize--;
				a = (*pPacket) << count;
				pPacket++;
				b |= a;
			}

			index = gpnIndexTable[b >> 0x18u];
			a = gpnCharacterTable[index];
			d = (b >> (0x18 - a)) & gpnBitMasks[a];
			c = gpnCharacterTable[index + 2 * d + 2];

			count += c;
			if (count > 0x20)
			{
				break;
			}

			a = gpnCharacterTable[index + 2 * d + 1];
			gszDecompressedPacket[rtPacketDecompressedSize++] = (char)a;

			b <<= (c & 0xFFu);
		}

		return gszDecompressedPacket;
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
		size_t dwOffset = 0;
		int nNumActiveSockets;

		if (!gbClientConnected)
		{	// no need to read anything if we're not connected to a server
			return 0;
		}

		// Clear out the packet buffer
		memset(gszPacketReadBuffer, 0, MAX_PACKET_SIZE);

		// Observe activity on the sockets
		nNumActiveSockets = SDLNet_CheckSockets(gClientSocketSet, dwTimeout);
		if (nNumActiveSockets != 0)
		{	// Activity detected!
			while (SDLNet_SocketReady(gClientSocket))
			{
				// Read up to the max packet size
				int nThisCycle = SDLNet_TCP_Recv(gClientSocket,
					gszPacketReadBuffer + dwOffset, MAX_PACKET_SIZE);

				while (dwOffset < nThisCycle)
				{
					// Peek at the current byte
					BYTE nPacketType = gszPacketReadBuffer[dwOffset];
					unsigned char* packet;
					size_t packetLen, realPacketLen;
					D2Packet inPacket{};

					// Decompress the packet, if necessary.
					if (gnServerCompression & NETFLAG_COMPRESSION)
					{
						size_t headerLen;

						// Get the number of bytes that we need to fetch. We need the header, as well as the trailing data.
						packetLen = GetCompressedPacketSize(&gszPacketReadBuffer[dwOffset], headerLen);
						if (nThisCycle < packetLen + headerLen)
						{	// didn't fetch enough data, need to recv more!
							packetLen = 0;
						}
						else
						{
							// Actually decompress the packet
							packet = DecompressPacket((unsigned char*)&gszPacketReadBuffer[dwOffset + headerLen], 
								packetLen, realPacketLen);

							// Increment packet length by header length so that we do the correct shifting, below
							packetLen += headerLen;

							// Actually read the payload
							inPacket.ReadClient(packet, realPacketLen);
						}
					}
					else
					{
						// This is a lot more straightforward. We just need to read it.
						packet = &gszPacketReadBuffer[dwOffset];
						packetLen = inPacket.ReadClient(packet, nThisCycle - dwOffset);
					}

					if (packetLen == 0)
					{	// incomplete packet, shift memory
						memmove(gszPacketReadBuffer, gszPacketReadBuffer + dwOffset, nThisCycle - dwOffset);
						dwOffset = nThisCycle = 0;
					}
					else
					{	// complete packet, shift offset
						dwOffset += packetLen;

						// There's specific packets which we need to intercept. 
						switch (nPacketType)
						{
							case D2SPACKET_COMPRESSIONINFO:
								gnServerCompression = inPacket.packetData.ServerCompressionInfo.nCompressionType;
								if (gnServerCompression & NETFLAG_CUSTOM)
								{
									Log::Error(__FILE__, __LINE__, "Custom compression is not supported.");
									return 0;
								}
								break;
							case D2SPACKET_UNCOMPRESSED:
								gnServerCompression = 0;
								break;
                            default:
                                break;
						}

						ClientProcessPacket(&inPacket);
						
						Log::Print(PRIORITY_DEBUG, "Client received packet: %d", nPacketType);
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

		memset(gszPacketReadBuffer, 0, MAX_PACKET_SIZE);

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
						D2Packet termPacket{};
						termPacket.nPacketType = D2SPACKET_CONNECTIONTERM;
						SendAnonymousServerPacket(temp, &termPacket);
						SDLNet_TCP_Close(temp);
					}
				}
			}

			// Loop through each client socket now to see if there's been any activity
			for (int i = 0; i < gdwMaxAllowedPlayers; i++)
			{
				size_t dwOffset = 0;
				while (SDLNet_SocketReady(gServerPlayerSockets[i]))
				{
					// Read up to the max packet size
					int nThisCycle = SDLNet_TCP_Recv(gServerPlayerSockets[i], 
						gszPacketReadBuffer + dwOffset, MAX_PACKET_SIZE);

					while (dwOffset < nThisCycle)
					{
						// Peek at the current byte
						BYTE nPacketType = gszPacketReadBuffer[dwOffset];
						D2Packet packet{};
						size_t packetLen = packet.ReadServer(gszPacketReadBuffer + dwOffset, nThisCycle - dwOffset);

						if (packetLen == 0)
						{	// incomplete packet, shift memory
							memmove(gszPacketReadBuffer, gszPacketReadBuffer + dwOffset, nThisCycle - dwOffset);
							dwOffset = nThisCycle = 0;
						}
						else
						{	// complete packet, shift offset
							dwOffset += packetLen;

							ServerProcessPacket(&packet);

							Log::Print(PRIORITY_DEBUG, "Server received packet: %d", nPacketType);
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
	 *	Try to resolve the specified address.
	 *	@author	eezstreet
	 */
	bool TryResolveAddress(char16_t* szAddress, DWORD dwAddressSize)
	{
		char szASCIIAddress[32]{ 0 };
		IPaddress ip;

		D2Lib::qwctomb(szASCIIAddress, 32, szAddress);
		return SDLNet_ResolveHost(&ip, szASCIIAddress, GAME_PORT) == 0;
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

		// Turn off compression
		gnServerCompression = 0;

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
		for (auto & gServerPlayerSocket : gServerPlayerSockets)
		{
			if (gServerPlayerSocket != nullptr)
			{
				SDLNet_DelSocket(gServerSocketSet, (SDLNet_GenericSocket)gServerPlayerSocket);
				// FIXME: tell clients that the server has been disconnected?
				SDLNet_TCP_Close(gServerPlayerSocket);
				gServerPlayerSocket = nullptr;
			}
		}

		// Finally, mark the server as no longer running
		gbServerRunning = false;
	}

	/*
	 *	Start up the networking subsystem.
	 *	@author	eezstreet
	 */
	void Init()
	{
		SDLNet_Init();

		gServerSocketSet = SDLNet_AllocSocketSet(MAX_PLAYERS_REAL + 1);
		Log_ErrorAssert(gServerSocketSet != nullptr)

		gClientSocketSet = SDLNet_AllocSocketSet(1);
		Log_ErrorAssert(gClientSocketSet != nullptr)
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
