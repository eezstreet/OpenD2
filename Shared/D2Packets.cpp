#include "D2Packets.hpp"

#ifdef D2EXE
#include "../Game/Diablo2.hpp"
#elif defined(D2CLIENT)
#include "../Client/D2Client.hpp"
#elif defined(D2GAME)
#include "../Server/D2Game.hpp"
#else
#include "../Common/D2Common.hpp"
#endif

/*
 *	Read a packet from the client, on the server.
 *	@author	eezstreet
 */
size_t D2Packet::ReadServer(char* buffer, size_t bufferSize)
{
	size_t dwAmount = 1;
	if (!bufferSize)
	{
		return 0;
	}

	nPacketType = buffer[0];
	switch (nPacketType)
	{
		case D2CPACKET_QUESTDATA:
		case D2CPACKET_RESURRECT:
		case D2CPACKET_BUSYOFF:
		case D2CPACKET_STAMINAON:
		case D2CPACKET_STAMINAOFF:
		case D2CPACKET_SWAPWEAPONS:
		case D2CPACKET_LEAVEGAME:
		case D2CPACKET_JOINGAME:
			break; // sizeof = 1
	}

	return dwAmount;
}

/*
 *	Read a packet from the server, on the client.
 *	@author	eezstreet
 */
size_t D2Packet::ReadClient(char* buffer, size_t bufferSize)
{
	size_t dwAmount = 1;
	if (!bufferSize)
	{
		return 0;
	}

	nPacketType = buffer[0];
	switch (nPacketType)
	{
		case D2SPACKET_GAMELOADING:
		case D2SPACKET_LOADSUCCESS:
		case D2SPACKET_LOADCOMPLETE:
		case D2SPACKET_UNLOADCOMPLETE:
		case D2SPACKET_GAMEXIT:
		case D2SPACKET_STARTMERCLIST:
		case D2SPACKET_UNKNOWN6E:
		case D2SPACKET_UNKNOWN6F:
		case D2SPACKET_UNKNOWN70:
		case D2SPACKET_UNKNOWN71:
		case D2SPACKET_UNKNOWN72:
		case D2SPACKET_SWITCHWEAPONS:
		case D2SPACKET_CONNECTIONTERM:
			break;
	}
	
	return dwAmount;
}

/*
 *	Write a packet to the client.
 *	@author	eezstreet
 */
size_t D2Packet::WriteServer(char* buffer, size_t bufferSize)
{
	size_t dwAmount = 1;
	if (!bufferSize)
	{
		return 0;
	}

	buffer[0] = nPacketType;
	switch (nPacketType)
	{
		case D2SPACKET_GAMELOADING:
		case D2SPACKET_LOADSUCCESS:
		case D2SPACKET_LOADCOMPLETE:
		case D2SPACKET_UNLOADCOMPLETE:
		case D2SPACKET_GAMEXIT:
		case D2SPACKET_STARTMERCLIST:
		case D2SPACKET_UNKNOWN6E:
		case D2SPACKET_UNKNOWN6F:
		case D2SPACKET_UNKNOWN70:
		case D2SPACKET_UNKNOWN71:
		case D2SPACKET_UNKNOWN72:
		case D2SPACKET_SWITCHWEAPONS:
		case D2SPACKET_CONNECTIONTERM:
			break;
	}

	return dwAmount;
}

/*
 *	Write a packet to the server.
 *	@author	eezstreet
 */
size_t D2Packet::WriteClient(char* buffer, size_t bufferSize)
{
	size_t dwAmount = 1;
	if (!bufferSize)
	{
		return 0;
	}

	buffer[0] = nPacketType;
	switch (nPacketType)
	{
		case D2CPACKET_QUESTDATA:
		case D2CPACKET_RESURRECT:
		case D2CPACKET_BUSYOFF:
		case D2CPACKET_STAMINAON:
		case D2CPACKET_STAMINAOFF:
		case D2CPACKET_SWAPWEAPONS:
		case D2CPACKET_LEAVEGAME:
		case D2CPACKET_JOINGAME:
			break; // sizeof = 1
	}

	return dwAmount;
}