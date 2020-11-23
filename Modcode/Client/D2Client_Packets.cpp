#include "D2Client.hpp"
#include "UI/Menus/LoadError.hpp"

/*
 *	This file is responsible for handling all of the behavior in response to a received packet.
 *	It's important to note that this is not responsible for actually parsing received data. Instead,
 *	that is handled by the engine. The engine intercepts specific packets (ie, D2SPACKET_COMPRESSIONINFO)
 *	and interacts on them before they arrive on the client.
 */

namespace ClientPacket
{
	// Error codes when connecting to a server
	WORD gwaTBLErrorEntries[] = {
		0x14F5,	// Unable to enter game. Bad character version
		0x14F6,	// Unable to enter game. Bad character quest data
		0x14F7,	// Unable to enter game. Bad character waypoint data
		0x14F8,	// Unable to enter game. Bad character stats data
		0x14F9,	// Unable to enter game. Bad character skills data
		0x14FB,	// Unable to enter game
		0x14FA,	// failed to join game
		0x14ED,	// Your connection has been interrupted
		0x14EE,	// The Host of this game has left
		0x14FC,	// unknown failure
		0x14FD,	// Unable to enter game, Bad inventory data
		0x14FE,	// Unable to enter game, bad dead bodies
		0x14FF,	// Unable to enter game, bad header
		0x1500,	// Unable to enter game, bad hireables
		0x1501,	// Unable to enter game, bad intro data
		0x1502,	// Unable to enter game, bad item
		0x1503,	// Unable to enter game, bad dead body item
		0x1504,	// Unable to enter game, generic bad file
		0x1505,	// Game is full
		0x14F0,	// Versions do not match. Please log onto battle.net or go to http://www.blizzard.com/support/Diablo2 to get a patch
		0x14F4,	// Unable to enter game. Your character must kill Diablo to play in a Nightmare game.
		0x14F3,	// Unable to enter game. Your character must kill Diablo in Nightmare difficulty to play in a Hell game.
		0x14F2,	// Unable to enter game. A normal character cannot join a game created by a hardcore character.
		0x14F1,	// Unable to enter game. A hardcore character cannot join a game created by a normal character.
		0x14EF,	// A dead hardcore character cannot join or create any games.
		0x2775,	// Unable to enter game. A Diablo II character cannot join a game created by a Diablo II Expansion character.
		0x2776,	// Unable to enter game. A Diablo II Expansion character cannot join a game created by a Diablo II character.
		0x14FA,	// failed to join game
		0x14FB,	// Unable to enter game
		0x0000,
	};

	/*
	 *	First stage of handshake - Server tells client what compression type is being used.
	 *	In response, the client sends a game join request packet.
	 *	@author	eezstreet
	 */
	void ProcessCompressionPacket(D2Packet* pPacket)
	{
		D2Packet response;

#if GAME_MINOR_VERSION <= 10
		if (!cl.bLocalServer)
		{	// Send a D2CPACKET_JOINREMOTE
			response.nPacketType = D2CPACKET_JOINREMOTE;
			response.packetData.ClientRemoteJoinRequest.nLocale = 0;	// FIXME
			response.packetData.ClientRemoteJoinRequest.unk1 = 0x00;
			response.packetData.ClientRemoteJoinRequest.unk2 = 0x01;
			response.packetData.ClientRemoteJoinRequest.nCharClass = cl.currentSave.header.nCharClass;
			response.packetData.ClientRemoteJoinRequest.dwVersion = GAME_MINOR_VERSION;
			D2Lib::strncpyz(response.packetData.ClientRemoteJoinRequest.szCharName,
				cl.currentSave.header.szCharacterName, 16);
		}
		else
		{	// Send a D2CPACKET_JOINLOCAL
			response.nPacketType = D2CPACKET_JOINLOCAL;
		}
#endif

		// Send the response packet
		engine->NET_SendClientPacket(&response);
	}



	/*
	 *	Error code in response to retrieving a packet.
	 *	@author	eezstreet
	 */
	void ProcessSavegameStatusPacket(D2Packet* pPacket)
	{
		WORD wResponse;

		// Retrieve data.
		switch (pPacket->packetData.ServerSaveStatus.nSaveStatus)
		{
			case SAVESTATUS_1:
				wResponse = 0;
				break;
			case SAVESTATUS_2:
				wResponse = 1;
				break;
			case SAVESTATUS_3:
				wResponse = 2;
				break;
			case SAVESTATUS_4:
				wResponse = 3;
				break;
			case SAVESTATUS_5:
				wResponse = 4;
				break;
			case SAVESTATUS_6:
				wResponse = 5;
				break;
			case SAVESTATUS_7:
				wResponse = 10;
				break;
			case SAVESTATUS_8:
				wResponse = 11;
				break;
			case SAVESTATUS_9:
				wResponse = 12;
				break;
			case SAVESTATUS_10:
				wResponse = 13;
				break;
			case SAVESTATUS_11:
				wResponse = 14;
				break;
			case SAVESTATUS_12:
				wResponse = 15;
				break;
			case SAVESTATUS_13:
				wResponse = 16;
				break;
			case SAVESTATUS_14:
				wResponse = 17;
				break;
			case SAVESTATUS_15:
				wResponse = 18;
				break;
			case SAVESTATUS_16:
				wResponse = 19;
				break;
			case SAVESTATUS_17:
				wResponse = 20;
				break;
			case SAVESTATUS_18:
				wResponse = 21;
				break;
			case SAVESTATUS_19:
				wResponse = 22;
				break;
			case SAVESTATUS_20:
				wResponse = 23;
				break;
			case SAVESTATUS_21:
				wResponse = 24;
				break;
			case SAVESTATUS_23:
				wResponse = 25;
				break;
			case SAVESTATUS_24:
				wResponse = 26;
				break;
			case SAVESTATUS_25:
				wResponse = 27;
				break;
			case SAVESTATUS_26:
				wResponse = 28;
				break;
			default:
				wResponse = 9;
				break;
		}

		// Set us back to the main menu status
		cl.bLocalServer = false;
		cl.gamestate = GS_MAINMENU;
		delete cl.pActiveMenu;
		cl.pActiveMenu = new D2Menus::LoadError(gwaTBLErrorEntries[wResponse]);
		engine->NET_Disconnect();
	}

	/*
	 *	Received metadata about the server.
	 *	This is the largest step of the handshake - once we have the metadata we need to send the *entire* savegame.
	 *	@author	eezstreet
	 */
	void ProcessServerMetaPacket(D2Packet* pPacket)
	{
		// We should verify that the metadata from the server is actually accurate and makes sense.
		// Even if we charge through this step, the server will slap us with an invalid savestate packet.
		// Still, we should do this to be nice and not overuse bandwidth...
		// FIXME: ?
		fs_handle f;
		size_t fileSize, remainder, chunks;
		D2Packet packet;

		cl.bValidatedSave = true;

		memset(&packet, 0, sizeof(packet));

		// Open savegame
		fileSize = engine->FS_Open(cl.szCurrentSave, &f, FS_READ, true);
		if (f == INVALID_HANDLE || fileSize == 0)
		{
			engine->Error(__FILE__, __LINE__, "Couldn't load savegame!");
			return;
		}

		chunks = fileSize / 255;
		remainder = fileSize - (chunks * 255);

		// Send whole chunks
		packet.nPacketType = D2CPACKET_SAVECHUNK;
		packet.packetData.ClientSendSaveChunk.dwSaveSize = fileSize;
		packet.packetData.ClientSendSaveChunk.nChunkSize = 255;
		for (size_t i = 0; i < chunks; i++)
		{
			engine->FS_Read(f, packet.packetData.ClientSendSaveChunk.nChunkBytes, 255, 1);
			engine->NET_SendClientPacket(&packet);
		}

		// Send remainder
		packet.packetData.ClientSendSaveChunk.nChunkSize = remainder;
		engine->FS_Read(f, packet.packetData.ClientSendSaveChunk.nChunkBytes, remainder, 1);
		engine->NET_SendClientPacket(&packet);

		// Close savegame, send completion packet
		engine->FS_CloseFile(f);
		packet.nPacketType = D2CPACKET_SAVEEND;
		engine->NET_SendClientPacket(&packet);

		// Send a ping packet
		packet.nPacketType = D2CPACKET_PING;
		packet.packetData.Ping.dwTickCount = engine->Milliseconds();
		packet.packetData.Ping.dwUnknown = 0;
		engine->NET_SendClientPacket(&packet);
	}

	/*
	 *	Received a PONG packet, we can calculate our ping (approx)
	 *	@author	eezstreet
	 */
	void ProcessPongPacket(D2Packet* pPacket)
	{
		cl.dwPing = engine->Milliseconds() - cl.dwLastPingPacket;
	}
}