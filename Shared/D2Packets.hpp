#pragma once
#include "D2Shared.hpp"

// Packets that are sent from the server
enum D2ServerPacketTypes
{
	D2SPACKET_GAMELOADING = 0x00,	// (sizeof = 1) - Game is loading still
	D2SPACKET_GAMEFLAGS = 0x01,		// Variable size, based on version. Sends metadata about the server to the client
	D2SPACKET_LOADSUCCESS = 0x02,	// (sizeof = 1) - first stage loading completed successfully
	D2SPACKET_LOADACT = 0x03,		// BYTE act, DWORD mapID, WORD areaID, DWORD unknown (sizeof = 12) - start loading acts
	D2SPACKET_LOADCOMPLETE = 0x04,	// (sizeof = 1) - game has finished loading completely
	D2SPACKET_UNLOADCOMPLETE = 0x05,// (sizeof = 1) - Game has finished unloading
	D2SPACKET_GAMEXIT = 0x06,		// (sizeof = 1) - game exit was successful
	D2SPACKET_MAPREVEAL = 0x07,		// WORD x, WORD y, BYTE areaID (sizeof = 6) - reveal a part of the automap
	D2SPACKET_MAPHIDE = 0x08,		// WORD x, WORD y, BYTE areaID (sizeof = 6) - hide part of the automap
	D2SPACKET_ASSIGNWARP = 0x09,	// BYTE actorType, DWORD actorID, WORD x, WORD y, WORD warpID (sizeof = 12) - assign warp zone to unit
	D2SPACKET_REMOVEOBJECT = 0x0A,	// BYTE actorType, DWORD actorID (sizeof = 6) - remove an actor
	D2SPACKET_HANDSHAKE = 0x0B,		// BYTE actorType, DWORD actorID (sizeof = 6) - ?
	D2SPACKET_NPCHIT = 0x0C,		// BYTE actorType, DWORD actorID, WORD animationID, BYTE life (sizeof = 9) - ?
	D2SPACKET_PLAYERSTOP = 0x0D,	// BYTE actorType, DWORD actorID, BYTE unknown, WORD x, WORD y, BYTE unknown, BYTE life (sizeof = 13) - stop a player's movement
	D2SPACKET_OBJECTSTATE = 0x0E,	// BYTE actorType, DWORD actorID, BYTE unknown (0x03), BYTE canChangeBack, DWORD actorState (sizeof = 12) - change an actor's state
	D2SPACKET_PLAYERMOVECOORD=0x0F,	// BYTE actorType, DWORD actorID, BYTE movementType, WORD targetX, WORD targetY, 00, WORD currentX, WORD currentY (sizeof = 16) - move a player to coordinates
	D2SPACKET_PLAYERMOVEACTOR=0x10,	// BYTE actorType, DWORD actorID, BYTE movementType, BYTE targetType, DWORD targetID, WORD currentX, WORD currentY (sizeof = 16) - move a player towards a target actor
	D2SPACKET_REPORTKILL = 0x11,	// BYTE actorType, DWORD actorID, WORD unknown (sizeof = 8) - report an actor (player/monster) as being killed
	D2SPACKET_UNKNOWN12 = 0x12,		// sizeof = 26
	D2SPACKET_UNKNOWN13 = 0x13,		// sizeof = 14
	D2SPACKET_UNKNOWN14 = 0x14,		// sizeof = 18
	D2SPACKET_REASSIGNPLAYER = 0x15,// BYTE actorType, DWORD actorID, WORD x, WORD y, BOOL unknown (sizeof = 11)
	D2SPACKET_UNKNOWN16 = 0x16,		// sizeof = variable
	D2SPACKET_UNUSED17 = 0x17,
	D2SPACKET_UNKNOWN18 = 0x18,		// sizeof = 15
	D2SPACKET_ADDGOLDBYTE = 0x19,	// BYTE amount (sizeof = 2) - add gold (amount in bytes)
	D2SPACKET_ADDEXPBYTE = 0x1A,	// BYTE amount (sizeof = 2) - add experience (amount in byte)
	D2SPACKET_ADDEXPWORD = 0x1B,	// WORD amount (sizeof = 3) - add experience (amount in word)
	D2SPACKET_ADDEXPDWORD = 0x1C,	// DWORD amount (sizeof = 5) - add experience (amount in dword)
	D2SPACKET_BASEATTRBYTE = 0x1D,	// BYTE attribute, BYTE amount (sizeof = 3) - set the base of an attribute (amount in byte)
	D2SPACKET_BASEATTRWORD = 0x1E,	// BYTE attribute, WORD amount (sizeof = 4) - set the base of an attribute (amount in word)
	D2SPACKET_BASEATTRDWORD = 0x1F,	// BYTE attribute, DWORD amount (sizeof = 6) - set the base of an attribute (amount in dword)
	D2SPACKET_ATTRUPDATE = 0x20,	// DWORD actorID, BYTE attribute, DWORD amount (sizeof = 10) - update an attribute on an actor
	D2SPACKET_UNKNOWN21 = 0x21,		// sizeof = 12
	D2SPACKET_UPDATEITEMSKILL=0x22,	// WORD unknown (actor type?), DWORD actorID, WORD skill, WORD amount, WORD unknown (sizeof = 12) - update the skills on an actor. This will cause passives to reset also.
	D2SPACKET_SETSKILL = 0x23,		// BYTE actorType, DWORD actorID, BYTE leftSkill, WORD skill, FF FF FF FF (sizeof = 13) - set the skill index of left/right skill
	D2SPACKET_UNKNOWN24 = 0x24,		// sizeof = 90
	D2SPACKET_UNKNOWN25 = 0x25,		// sizeof = 90
	D2SPACKET_CHAT = 0x26,			// Responsible for both normal and overhead chat.
									// NORMAL CHAT
									// WORD chatKind, WORD unknown (0x02), 00 00 00 00, BYTE chatFlags (0x01 = whisper, 0x05 = chat), BYTE* charName, 00, WORD* message, 00 (sizeof = variable)
									// OVERHEAD CHAT
									// 05 00 00, BYTE actorType, DWORD actorID, WORD unknown, 00, WORD* message, 00 (sizeof = variable)
	D2SPACKET_NPCINFO = 0x27,		// BYTE actorType, DWORD actorID, BYTE[34] unknown (sizeof = 40)
	D2SPACKET_QUESTINFO = 0x28,		// (sizeof = 103)
	D2SPACKET_GAMEQUESTINFO = 0x29,	// (sizeof = 97)
	D2SPACKET_NPCTRANSACTION=0x2A,	// BYTE tradeType, BYTE tradeResult (0x00 = bought, 0x01 = sold, 0x0C = not enough gold), DWORD unknown, DWORD itemID, DWORD goldAfterPurchase (sizeof = 15) - purchase an item from an NPC
	D2SPACKET_UNUSED2B = 0x2B,
	D2SPACKET_PLAYSOUND = 0x2C,		// BYTE actorType, DWORD actorID, WORD sound (sizeof = 8) - play a sound on the actor
	D2SPACKET_UNUSED2D = 0x2D,
	D2SPACKET_UNUSED2E = 0x2E,
	D2SPACKET_UNUSED2F = 0x2F,
	D2SPACKET_UNUSED30 = 0x30,
	D2SPACKET_UNUSED31 = 0x31,
	D2SPACKET_UNUSED32 = 0x32,
	D2SPACKET_UNUSED33 = 0x33,
	D2SPACKET_UNUSED34 = 0x34,
	D2SPACKET_UNUSED35 = 0x35,
	D2SPACKET_UNUSED36 = 0x36,
	D2SPACKET_UNUSED37 = 0x37,
	D2SPACKET_UNUSED38 = 0x38,
	D2SPACKET_UNUSED39 = 0x39,
	D2SPACKET_UNUSED3A = 0x3A,
	D2SPACKET_UNUSED3B = 0x3B,
	D2SPACKET_UNUSED3C = 0x3C,
	D2SPACKET_UNUSED3D = 0x3D,
	D2SPACKET_UPDATEITEMSTATS=0x3E,	// (sizeof = variable)
	D2SPACKET_USESTACKEDITEM=0x3F,	// (sizeof = 8)
	D2SPACKET_UNKNOWN40 = 0x40,		// (sizeof = 13)
	D2SPACKET_UNUSED41 = 0x41,
	D2SPACKET_CLEARCURSOR = 0x42,	// BYTE actorType, DWORD playerID (sizeof = 6) - clear the current item in the cursor
	D2SPACKET_UNUSED43 = 0x43,
	D2SPACKET_UNUSED44 = 0x44,
	D2SPACKET_UNKNOWN45 = 0x45,		// (sizeof = 13)
	D2SPACKET_UNUSED46 = 0x46,
	D2SPACKET_RELATOR1 = 0x47,		// WORD param1, DWORD actorID, DWORD param2
	D2SPACKET_RELATOR2 = 0x48,		// WORD param1, DWORD actorID, DWORD param2
	D2SPACKET_UNUSED49 = 0x49,
	D2SPACKET_UNUSED4A = 0x4A,
	D2SPACKET_UNUSED4B = 0x4B,
	D2SPACKET_SKILLONACTOR = 0x4C,	// BYTE actorType, DWORD actorID, WORD skillId, WORD unknown, DWORD targetID, 00 00 (sizeof = 16) - cast a skill, targeting a specific actor
	D2SPACKET_SKILLCAST = 0x4D,		// BYTE actorType, DWORD actorID, DWORD skillID, BYTE unknown, WORD x, WORD y, 00 00 (sizeof = 17) - cast a skill targetting a coordinate spot
	D2SPACKET_MERCFORHIRE = 0x4E,	// WORD mercID, DWORD unknown (sizeof = 7) - adds a merc to the merc hire list
	D2SPACKET_STARTMERCLIST = 0x4F,	// (sizeof = 1)
	D2SPACKET_UNKNOWN50 = 0x50,		// (sizeof = 15)
	D2SPACKET_ASSIGNOBJECT = 0x51,	// BYTE objectType, DWORD objectID, WORD objectCode, WORD x, WORD y, BYTE state, BYTE interactionType (sizeof = 14)
	D2SPACKET_QUESTLOGINFO = 0x52,	// (sizeof = 42)
	D2SPACKET_SLOTREFRESH = 0x53,	// DWORD slotID, BYTE unknown (0x00 or 0x80), DWORD tickCount (sizeof = 10) - refreshes a player slot
	D2SPACKET_UNKNOWN54 = 0x54,		// (sizeof = 10)
	D2SPACKET_UNKNOWN55 = 0x55,		// (sizeof = 3)
	D2SPACKET_UNUSED56 = 0x56,
	D2SPACKET_UNUSED57 = 0x57,
	D2SPACKET_UNKNOWN58 = 0x58,		// (sizeof = 14)
	D2SPACKET_ASSIGNPLAYER = 0x59,	// DWORD actorID, BYTE charType, BYTE[16] charName, WORD x, WORD y (sizeof = 26) - assign a player to an actor ID
	D2SPACKET_EVENTMESSAGE = 0x5A,	// (sizeof = 40)
	D2SPACKET_PLAYERJOINED = 0x5B,	// WORD packetLength, DWORD playerID, BYTE charType, BYTE[16] charName, WORD charlevel, WORD partyID, 00 00 00 00 00 00 00 00 (sizeof = 36) - a player has just joined the game
	D2SPACKET_PLAYERLEFT = 0x5C,	// DWORD playerID (sizeof = 5) - a player has just left the game
	D2SPACKET_QUESTITEMSTATE=0x5D,	// BYTE unknown, DWORD unknown (sizeof = 6) - a quest item was just picked up
	D2SPACKET_UNKNOWN5E = 0x5E,		// (sizeof = 38)
	D2SPACKET_UNKNOWN5F = 0x5F,		// (sizeof = 5)
	D2SPACKET_TPSTATE = 0x60,		// BYTE state, BYTE areaID, DWORD actorID (sizeof = 7) - a townportal was just created
	D2SPACKET_UNKNOWN61 = 0x61,		// (sizeof = 2)
	D2SPACKET_UNKNOWN62 = 0x62,		// (sizeof = 7)
	D2SPACKET_WPMENU = 0x63,		// DWORD actorID, BYTE* availableWaypoints (sizeof = variable) - the waypoint menu was just opened; send across a list of available waypoints
	D2SPACKET_UNUSED64 = 0x64,
	D2SPACKET_PLAYERKILLS = 0x65,	// DWORD actorID, DWORD count (sizeof = 7) - an apparently unused packet which logs how many kills the player has made
	D2SPACKET_UNKNOWN66 = 0x66,		// (sizeof = 7)
	D2SPACKET_NPCMOVECOORD = 0x67,	// DWORD actorID, BYTE moveType (0x01 = walk, 0x17 = run), WORD x, WORD y, WORD unknown, BYTE unknown, WORD unknown, BYTE unknown (sizeof = 16) - an NPC or monster has just moved
	D2SPACKET_NPCMOVEACTOR = 0x68,	// DWORD actorID, BYTE moveType (0x00 = walk, 0x18 = run), WORD x, WORD y, BYTE targetActorType, DWORD targetActorID, WORD unknown, BYTE unknown, WORD unknown, BYTE unknown (sizeof = 21) - an NPC or monster has just moved
	D2SPACKET_NPCSTATE = 0x69,		// DWORD actorID, BYTE state, WORD x, WORD y, BYTE life, BYTE unknown (sizeof = 12)
	D2SPACKET_UNKNOWN6A = 0x6A,		// (sizeof = 12)
	D2SPACKET_NPCACTION = 0x6B,		// DWORD actorID, BYTE action, 00 00 00 00 00 00, WORD x, WORD y (sizeof = 16) - have the NPC perform an action
	D2SPACKET_NPCATTACK = 0x6C,		// DWORD actorID, WORD attackType, DWORD targetID, BYTE targetType, WORD x, WORD y (sizeof = 16) - have the NPC attack the target
	D2SPACKET_NPCSTOP = 0x6D,		// DWORD actorID, WORD x, WORD y, BYTE life (sizeof = 10) - have the NPC stop moving
	D2SPACKET_UNKNOWN6E = 0x6E,		// (sizeof = 1)
	D2SPACKET_UNKNOWN6F = 0x6F,		// (sizeof = 1)
	D2SPACKET_UNKNOWN70 = 0x70,		// (sizeof = 1)
	D2SPACKET_UNKNOWN71 = 0x71,		// (sizeof = 1)
	D2SPACKET_UNKNOWN72 = 0x72,		// (sizeof = 1)
	D2SPACKET_UNKNOWN73 = 0x73,		// (sizeof = 32)
	D2SPACKET_CORPSEASSIGN = 0x74,	// BYTE assign (0x00 = false, 0x01 = true), DWORD ownerID, DWORD corpseID (sizeof = 10) - assign a corpse item to a particular player
	D2SPACKET_PARTYINFO = 0x75,		// DWORD actorID, WORD partyID, WORD charLevel, WORD relationship, WORD inYourParty (boolean) (sizeof = 13) - update party info for a particular player
	D2SPACKET_PROXPLAYER = 0x76,	// BYTE actorType, DWORD actorID (sizeof = 6) - used to notify the player that another player is in their proximity (?)
	D2SPACKET_BUTTONACTION = 0x77,	// BYTE action (sizeof = 2)
	D2SPACKET_TRADEACCEPT = 0x78,	// BYTE[16] charName, DWORD actorID (sizeof = 21) - the trade that this player is in has been accepted
	D2SPACKET_GOLDINTRADE = 0x79,	// BYTE owner, DWORD amount (sizeof = 6) - the amount of gold in the trade window has changed
	D2SPACKET_PETACTION = 0x7A,		// BYTE reason (0x00 = unsummoned/lost sight, 0x01 = summoned), BYTE skill, WORD petType, DWORD ownerID, DWORD petID (sizeof = 13) - a pet has just been summoned or unsummoned. deal with it
	D2SPACKET_ASSIGNHOTKEY = 0x7B,	// BYTE slot, BYTE skill, BYTE leftOrRight (00 = right, 80 = left), FF FF FF FF (sizeof = 8) - tell the client that we acknowledged their hotkey change
	D2SPACKET_USEITEM = 0x7C,		// BYTE type, DWORD itemID (sizeof = 6) - use an item
	D2SPACKET_SETITEMSTATE = 0x7D,	// (sizeof = 18)
	D2SPACKET_UNKNOWN7E = 0x7E,		// (sizeof = 5)
	D2SPACKET_ALLYPARTYINFO = 0x7F,	// BYTE actorType, WORD life, DWORD actorID, DWORD areaID
	D2SPACKET_UNUSED80 = 0x80,
	D2SPACKET_ASSIGNMERC = 0x81,	// BYTE unknown, WORD mercType, DWORD ownerID, DWORD mercID, DWORD unknown, DWORD unknown (sizeof = 20) - assign a merc to a player
	D2SPACKET_ASSIGNPORTAL = 0x82,	// DWORD ownerID, BYTE[16] ownerName, DWORD localID, DWORD remoteID (sizeof = 29) - Assign a town portal to an owner
	D2SPACKET_UNUSED83 = 0x83,
	D2SPACKET_UNUSED84 = 0x84,
	D2SPACKET_UNUSED85 = 0x85,
	D2SPACKET_UNUSED86 = 0x86,
	D2SPACKET_UNUSED87 = 0x87,
	D2SPACKET_UNUSED88 = 0x88,
	D2SPACKET_SPECIALQEVENT = 0x89,	// BYTE eventID (sizeof = 2) - triggers a special quest event
	D2SPACKET_NPCWANTSYOU = 0x8A,	// BYTE actorType, DWORD actorID (sizeof = 6) - an NPC wants to interact with the player.
	D2SPACKET_SETPARTYSTATE=0x8B,	// DWORD actorID, BYTE status (0 = no party, 1 = in party, 2 = desires party) (sizeof = 6) - communicates the relationship status
	D2SPACKET_SETRELATIONSHIP=0x8C,	// DWORD player1ID, DWORD player2ID, WORD relationState (sizeof = 11) - set the relationship between two players
	D2SPACKET_SETPLAYERPARTY=0x8D,	// DWORD playerID, WORD partyID (sizeof = 7) - assign a player to a party
	D2SPACKET_CORPSEASSIGN2 = 0x8E,	// BYTE assignIt, DWORD ownerID, DWORD corpseID (sizeof = 10) - assign a player to a corpse
	D2SPACKET_PONG = 0x8F,			// BYTE[32] (sizeof = 33) - reply to ping packet
	D2SPACKET_PARTYAUTOMAP = 0x90,	// DWORD playerID, DWORD playerX, DWORD playerY (sizeof = 13) - relay position of party member for the automap
	D2SPACKET_UNKNOWN91 = 0x91,		// (sizeof = 26)
	D2SPACKET_RESETTRADE = 0x92,	// BYTE actorType, DWORD actorID (sizeof = 6) - resets the trade window
	D2SPACKET_UNKNOWN93 = 0x93,		// (sizeof = 8)
	D2SPACKET_BASESLVLS = 0x94,		// BYTE numSkills, DWORD playerID, struct{WORD skill, BYTE level}*skills (sizeof = variable) - send a list of all skills and their skill levels
	D2SPACKET_LIFEMANA = 0x95,		// WORD life, WORD mana, WORD stamina, WORD x, WORD y, WORD unknown (sizeof = 13)
	D2SPACKET_WALKVERIFY = 0x96,	// WORD stamina, WORD x, WORD y, WORD state (sizeof = 9) - set player stamina and current X as of walking
	D2SPACKET_SWITCHWEAPONS = 0x97,	// (sizeof = 1) - Switch between weapon sets
	D2SPACKET_UNKNOWN98 = 0x98,		// (sizeof = 7)
	D2SPACKET_SKILLTRIGGERED=0x99,	// (sizeof = 16)
	D2SPACKET_UNKNOWN9A = 0x9A,		// (sizeof = 17)
	D2SPACKET_UNKNOWN9B = 0x9B,		// (sizeof = 7)
	D2SPACKET_ITEMACTION = 0x9C,	// (sizeof = variable)
	D2SPACKET_OWNEDACTION = 0x9D,	// (sizeof = variable)
	D2SPACKET_MERCBYTEATTR = 0x9E,	// BYTE attribute, DWORD mercID, BYTE amount (sizeof = 7) - change an attribute on a mercenary
	D2SPACKET_MERCWORDATTR = 0x9F,	// BYTE attribute, DWORD mercID, WORD amount (sizeof = 8) - change an attribute on a mercenary
	D2SPACKET_MERCDWORDATTR = 0xA0,	// BYTE attribute, DWORD mercID, DWORD amount (sizeof = 10) - change an attribute on a mercenary
	D2SPACKET_MERCADDEXPBYTE=0xA1,	// (sizeof = 7)
	D2SPACKET_MERCADDEXPWORD=0xA2,	// (sizeof = 8)
	D2SPACKET_UNKNOWNA3 = 0xA3,		// (sizeof = 24)
	D2SPACKET_UNKNOWNA4 = 0xA4,		// (sizeof = 3)
	D2SPACKET_UNKNOWNA5 = 0xA5,		// (sizeof = 8)
	D2SPACKET_UNKNOWNA6 = 0xA6,		// (sizeof = variable)
	D2SPACKET_DELAYEDSTATE = 0xA7,	// BYTE actorType, DWORD actorID, BYTE state (sizeof = 7)
	D2SPACKET_SETSTATE = 0xA8,		// BYTE actorType, DWORD actorID, BYTE packetLength, BYTE state, VOID stateEffects (sizeof = variable) - set a state on a unit
	D2SPACKET_ENDSTATE = 0xA9,		// BYTE actorType, DWORD actorID, BYTE state (sizeof = 7)
	D2SPACKET_ADDACTOR = 0xAA,		// BYTE actorType, DWORD actorID, BYTE packetLength, VOID stateInfo (sizeof = variable) - add a unit to the clientside info
	D2SPACKET_NPCHEAL = 0xAB,		// BYTE actorType, DWORD actorID, BYTE actorLife (sizeof = 7)
	D2SPACKET_ASSIGNNPC = 0xAC,		// DWORD actorID, WORD actorCode, WORD x, WORD y, BYTE actorLife, BYTE packetLength, VOID stateInfo (sizeof = variable)
	D2SPACKET_UNKNOWNAD = 0xAD,		// (sizeof = 9)
#if GAME_MINOR_VERSION >= 11
	D2SPACKET_WARDEN = 0xAE,		// warden code, do not use
	D2SPACKET_COMPRESSIONINFO=0xAF,	// BYTE protocolVer (sizeof = 2) - tell the client what kind of compression we are using
	D2SPACKET_CONNECTIONTERM=0xB0,	// (sizeof = 1) - Connection was forcibly terminated
	D2SPACKET_UNKNOWNB1 = 0xB1,		// (sizeof = 53)
	D2SPACKET_UNKNOWNB2 = 0xB2,		// (sizeof = variable)
	D2SPACKET_UNKNOWNB3 = 0xB3,		// (sizeof = 5)
#else
	D2SPACKET_COMPRESSIONINFO=0xAE,	// BYTE rejectMessage (sizeof = 2) - tell the client what form of compression is to be used
	D2SPACKET_UNCOMPRESSED=0xAF,	// (sizeof = 1) - tell the client that subsequent packets will NOT be compressed
	D2SPACKET_CONNECTIONTERM=0xB1,	// (sizeof = 1) - tell the client to take a hike!
	D2SPACKET_SAVELOAD=0xB2,		// (sizeof = ?) - tell the client to load a particular D2S file
	D2SPACKET_SAVESTATUS=0xB3,		// BYTE saveStatus (sizeof = 2) - tell the client that we are in need of their savegame
#endif
	D2SPACKET_MAX,
};

// Packets that are sent from the client
enum D2ClientPacketTypes
{
	D2CPACKET_UNUSED00 = 0x00,
	D2CPACKET_WALKCOORD = 0x01,		// WORD x, WORD y (sizeof = 5)
	D2CPACKET_WALKACTOR = 0x02,		// DWORD type, DWORD id (sizeof = 9)
	D2CPACKET_RUNCOORD = 0x03,		// WORD x, WORD y (sizeof = 5)
	D2CPACKET_RUNACTOR = 0x04,		// DWORD type, DWORD id (sizeof = 9)
	D2CPACKET_SLCLICKCOORD = 0x05,	// WORD x, WORD y (sizeof = 9) - shift left click skill
	D2CPACKET_LCLICKACTOR = 0x06,	// DWORD type, DWORD id (sizeof = 9) - left click skill
	D2CPACKET_SLCLICKACTOR = 0x07,	// DWORD type, DWORD id (sizeof = 9) - shift left click skill
	D2CPACKET_SLHCLICKCOORD = 0x08,	// WORD x, WORD y (sizeof = 5) - shift left click skill (hold)
	D2CPACKET_LHCLICKACTOR = 0x09,	// DWORD type, DWORD id (sizeof = 9) - left click skill (hold)
	D2CPACKET_SLHCLICKACTOR = 0x0A,	// DWORD type, DWORD id (sizeof = 9) - shift left click skill (hold)
	D2CPACKET_UNUSED0B = 0x0B,
	D2CPACKET_RCLICKCOORD = 0x0C,	// WORD x, WORD y (sizeof = 5) - right click skill
	D2CPACKET_RCLICKACTOR = 0x0D,	// DWORD type, DWORD id (sizeof = 9) - right click skill
	D2CPACKET_SRCLICKACTOR = 0x0E,	// DWORD type, DWORD id (sizeof = 9) - shift right click skill
	D2CPACKET_RHCLICKCOORD = 0x0F,	// WORD x, WORD y (sizeof = 5) - right click skill (hold)
	D2CPACKET_RHCLICKACTOR = 0x10,	// DWORD type, DWORD id (sizeof = 9) - right click skill (hold)
	D2CPACKET_SRHCLICKACTOR = 0x11,	// DWORD type, DWORD id (sizeof = 9) - shift right click skill (hold)
	D2CPACKET_UNUSED12 = 0x12,
	D2CPACKET_INTERACT = 0x13,		// DWORD type, DWORD id (sizeof = 9) - interact with actor
	D2CPACKET_OCHAT = 0x14,			// 00 00 (message) 00 00 00 (unknown size) - overhead chat
	D2CPACKET_CHAT = 0x15,			// 01 00 (message) 00 00 00 (unknown size) - chat
	D2CPACKET_GROUNDITEM = 0x16,	// 04 00 00 00 DWORD id, DWORD inventory (sizeof = 13) - pick up item from ground
	D2CPACKET_DROPITEM = 0x17,		// DWORD id (sizeof = 5) - drop item onto the ground
	D2CPACKET_INSITEMBUFFER = 0x18,	// DWORD id, DWORD xpos, DWORD ypos, DWORD buffer (sizeof = 17) - insert item into a buffer (cube, stash, etc)
	D2CPACKET_REMITEMBUFFER = 0x19,	// DWORD id (sizeof = 5) - remove an item from its buffer
	D2CPACKET_EQUIPITEM = 0x1A,		// DWORD id, WORD slot, 00 00 (sizeof = 9) - equip an item
	D2CPACKET_SWAP2HITEM = 0x1B,	// DWORD id, WORD slot, 00 00 (sizeof = 9) - equip a one handed weapon while we have a two handed weapon equipped
	D2CPACKET_UNEQUIPITEM = 0x1C,	// WORD slot (sizeof = 3) - unequip an item
	D2CPACKET_CURSOREQUIP = 0x1D,	// DWORD id, WORD slot, 00 00 (sizeof = 9) - equip the item that is currently in the cursor
	D2CPACKET_SWAP1HITEM = 0x1E,	// DWORD unknown, DWORD unknown (sizeof = 9) - equip a two handed weapon while we have two one handed weapons equipped
	D2CPACKET_SWAPBUFFER = 0x1F,	// DWORD cursorItem, DWORD bufferItem, DWORD xpos, DWORD ypos (sizeof = 17) - swap cursor/buffer items
	D2CPACKET_USEBUFFERITEM = 0x20,	// DWORD id, WORD x, 00 00, WORD y, 00 00 (sizeof = 13) - use an item that is in a buffer
	D2CPACKET_STACKITEM = 0x21,		// DWORD itemToStackID, DWORD itemWhereStackID (sizeof = 9) - stack one item onto another
	D2CPACKET_UNSTACKITEMS = 0x22,	// DWORD unknown (sizeof = 5) - unstack items
	D2CPACKET_BELTITEM = 0x23,		// DWORD id, DWORD position (sizeof = 9) - put an item onto the belt
	D2CPACKET_UNBELTITEM = 0x24,	// DWORD id (sizeof = 5) - remove an item from the belt
	D2CPACKET_SWAPBELTITEM = 0x25,	// DWORD cursorItem, DWORD beltItem (sizeof = 9) - swap an item on the belt with the one on the cursor
	D2CPACKET_USEBELTITEM = 0x26,	// DWORD id, DWORD option, 00 00 00 00 (sizeof = 13) - use an item on the belt
	D2CPACKET_IDENTIFYITEM = 0x27,	// DWORD itemID, DWORD scrollID (sizeof = 9) - identify an item using a scroll
	D2CPACKET_SOCKETITEM = 0x28,	// DWORD itemToSocket, DWORD socketedItem (sizeof = 9) - put an item into another item's socket
	D2CPACKET_SCROLLINBOOK = 0x29,	// DWORD scrollID, DWORD bookID (sizeof = 9) - put a scroll into a book
	D2CPACKET_ITEMTOCUBE = 0x2A,	// DWORD itemID, DWORD cubeID (sizeof = 9) - put an item into the Cube
	D2CPACKET_UNUSED2B = 0x2B,
	D2CPACKET_DEPRECATED2C = 0x2C,	// DO NOT send this across the net, the server will think that you are hacking!!
	D2CPACKET_DEPRECATED2D = 0x2D,	// DO NOT send this across the net, the server will think that you are hacking!!
	D2CPACKET_UNUSED2E = 0x2E,
	D2CPACKET_TALKTOACTOR = 0x2F,	// DWORD type, DWORD id (sizeof = 9) - talk to an NPC
	D2CPACKET_ENDTALKING = 0x30,	// DWORD type, DWORD id (sizeof = 9) - stop talking to an NPC
	D2CPACKET_QUESTMESSAGE = 0x31,	// DWORD id, DWORD message (sizeof = 9) - quest related
	D2CPACKET_BUYITEM = 0x32,		// DWORD actor, DWORD item, DWORD tab, DWORD cost (sizeof = 17) - buy an item from an NPC
	D2CPACKET_SELLITEM = 0x33,		// DWORD actor, DWORD item, DWORD tab, DWORD cost (sizeof = 17) - sell an item to an NPC
	D2CPACKET_NPCIDENTIFY = 0x34,	// DWORD actor (sizeof = 5) - identify all items from identifier NPC (Deckard Cain)
	D2CPACKET_REPAIRITEM = 0x35,	// DWORD actor, DWORD item, DWORD tab, DWORD cost (sizeof = 17) - repair an item by means of NPC
	D2CPACKET_HIREMERC = 0x36,		// DWORD actor, DWORD merc (sizeof = 9) - hire a mercenary from an actor
	D2CPACKET_GAMBLEITEM = 0x37,	// DWORD itemID (sizeof = 5) - identifies an item after it has been gambled for
	D2CPACKET_ACTORACTION = 0x38,	// DWORD action, DWORD actor, DWORD complement (sizeof = 13) - performs an action from an actor (?)
	D2CPACKET_ASKFORHEALING = 0x39,	// DWORD actor (sizeof = 5) - request healing from a healer NPC
	D2CPACKET_ALLOCATESTAT = 0x3A,	// WORD stat (sizeof = 3) - put a point in a stat
	D2CPACKET_ALLOCATESKILL = 0x3B,	// WORD skill (sizeof = 3) - put a point in a skill
	D2CPACKET_SELECTSKILL = 0x3C,	// WORD skill, 00, BYTE(80 = left, 00 = right), FF FF (sizeof = 9) - select a skill to use
	D2CPACKET_HIGHLIGHTDOOR = 0x3D,	// DWORD objectID (sizeof = 5) - highlights a door. Can cause a crash.
	D2CPACKET_USEINIFUSS = 0x3E,	// DWORD scrollItemID (sizeof = 5) - use the Scroll of Inifuss
	D2CPACKET_PLAYSOUND = 0x3F,		// WORD sound (sizeof = 3) - play a sound oriented at the player
	D2CPACKET_QUESTDATA = 0x40,		// (sizeof = 1) - Request quest data from the server
	D2CPACKET_RESURRECT = 0x41,		// (sizeof = 1) - Request a player resurrection
	D2CPACKET_UNUSED42 = 0x42,
	D2CPACKET_UNUSED43 = 0x43,
	D2CPACKET_STAFFORIFICE = 0x44,	// DWORD orificeType, DWORD orificeID, DWORD staffID, DWORD state (sizeof = 17) - place the Horadric Staff in the Horadric Orifice
	D2CPACKET_PORTALLOC = 0x45,		// DWORD actorID, WORD location, 00 00 (sizeof = 9) - Change the location of the player's town portal. NOTE: DO NOT SEND THIS! The player will be flagged as a hacker!!
	D2CPACKET_MERCINTERACT = 0x46,	// DWORD mercID, DWORD actorID, DWORD actorType (sizeof = 13) - Have the player's mercenary interact with something
	D2CPACKET_MERCMOVE = 0x47,		// DWORD mercID, DWORD x, DWORD y (sizeof = 13) - have the player's mercenary move somewhere
	D2CPACKET_BUSYOFF = 0x48,		// (sizeof = 1) - Turn off the player's "busy state" (ie, allow them to trade)
	D2CPACKET_TAKEWAYPOINT = 0x49,	// DWORD waypointID, DWORD destination (sizeof = 9) - take the waypoint to the destination
	D2CPACKET_UNUSED4A = 0x4A,
	D2CPACKET_ACTORUPDATE = 0x4B,	// DWORD actorType, DWORD actorID (sizeof = 9) - request an update on an actor
	D2CPACKET_TRANSMOGRIFY = 0x4C,	// DWORD itemID (sizeof = 5) - transmogrify an item
	D2CPACKET_PLAYNPCMESSAGE = 0x4D,// WORD npcSoundMessage (sizeof = 3) - request an NPC sound effect
	D2CPACKET_UNUSED4E = 0x4E,
	D2CPACKET_CLICKBUTTON = 0x4F,	// DWORD buttonId, WORD complement (sizeof = 7) - click a button on the interface (?)
	D2CPACKET_DROPGOLD = 0x50,		// DWORD playerID, DWORD goldAmount (sizeof = 9) - drops some gold
	D2CPACKET_BINDSKILL = 0x51,		// BYTE skill, BYTE (80 = left, 00 = right), FF FF FF FF - binds a hotkey to a skill
	D2CPACKET_UNUSED52 = 0x52,		// DWORD unknown (sizeof = 5)
	D2CPACKET_STAMINAON = 0x53,		// (sizeof = 1) - Turn stamina on (unused)
	D2CPACKET_STAMINAOFF = 0x54,	// (sizeof = 1) - Turn stamina off (unused)
	D2CPACKET_UNUSED55 = 0x55,
	D2CPACKET_UNUSED56 = 0x56,
	D2CPACKET_UNUSED57 = 0x57,
	D2CPACKET_QUESTCOMPLETE = 0x58,	// WORD questID (sizeof = 3) - mark a quest as being complete
	D2CPACKET_MOVEACTOR = 0x59,		// DWORD actorType, DWORD actorId, DWORD x, DWORD y (sizeof = 17) - make an actor move
	D2CPACKET_UNUSED5A = 0x5A,
	D2CPACKET_UNUSED5B = 0x5B,
	D2CPACKET_UNUSED5C = 0x5C,
	D2CPACKET_HOSTILE = 0x5D,		// BYTE button, BYTE onoff, DWORD playerID (sizeof = 7) - either squelch a player or go hostile with them
	D2CPACKET_INVITEPARTY = 0x5E,	// BYTE button, DWORD playerID (sizeof = 6) - invite a player to your party
	D2CPACKET_UPDATEPOS = 0x5F,		// WORD x, WORD y (sizeof = 5) - update your position manually to keep things in sync
	D2CPACKET_SWAPWEAPONS = 0x60,	// (sizeof = 1) - swap weapon sets
	D2CPACKET_MERCITEM = 0x61,		// WORD position (00 to drop) (sizeof = 3) - drop/pickup merc item
	D2CPACKET_MERCRESURRECT = 0x62,	// DWORD npcID (sizeof = 5) - resurrect mercenary
	D2CPACKET_BELTQUICK = 0x63,		// DWORD item (sizeof = 5) - shift-click item to belt
	D2CPACKET_HACKDETECT64 = 0x64,	// (sizeof = 9) - hack detection
	D2CPACKET_HACKDETECT65 = 0x65,	// hack detection
#if GAME_MINOR_VERSION >= 11
	D2CPACKET_WARDEN = 0x66,		// (sizeof = variable) - warden/hack detection related. Do not use.
	D2CPACKET_HACKDETECT67 = 0x67,	// hack detection
	D2CPACKET_JOINREQUEST = 0x68,	// DWORD serverHash, WORD serverToken, BYTE characterType, DWORD version, DWORD unknown, DWORD unknown, 00, char[15] characterName, 00 (sizeof = 37) - try to join this server
	D2CPACKET_LEAVEGAME = 0x69,		// (sizeof = 1) - leave the server
	D2CPACKET_JOINGAME = 0x6B,		// (sizeof = 1) - join the game
	D2CPACKET_PING = 0x6D,			// DWORD tickCount, DWORD delay, 00 00 00 00 (sizeof = 13) - ping the server to keep the connection alive
#else
	D2CPACKET_JOINLOCAL = 0x66,		// (sizeof = 46) - join a local game. (singleplayer, or TCP/IP as host)
	D2CPACKET_JOINREMOTE = 0x67,	// (sizeof = 29) - join a remotely-hosted game (TCP/IP as client, or battle.net)
	D2CPACKET_LEAVEGAME = 0x68,		// (sizeof = 1) - leave the server
	D2CPACKET_SAVEEND = 0x6A,		// (sizeof = 1) - the savefile is finished sending
	D2CPACKET_SAVECHUNK = 0x6B,		// BYTE chunkSize, DWORD saveSize, BYTE[256] chunk (sizeof = variable)
	D2CPACKET_PING = 0x6C,			// DWORD tickCount, DWORD lastTickCount (sizeof = 9) - ping the server to keep the connection alive
#endif
	D2CPACKET_UNUSED6E = 0x6E,
	D2CPACKET_UNUSED6F = 0x6F,
	D2CPACKET_MAX,
};

// D2SPACKET_SAVESTATUS return codes
enum D2SaveStatus
{	// default to 9
	SAVESTATUS_0,	// not used?
	SAVESTATUS_1,	// maps to 0
	SAVESTATUS_2,	// maps to 1
	SAVESTATUS_3,	// maps to 2
	SAVESTATUS_4,	// maps to 3
	SAVESTATUS_5,	// maps to 4
	SAVESTATUS_6,	// maps to 5
	SAVESTATUS_7,	// maps to 10
	SAVESTATUS_8,	// maps to 11
	SAVESTATUS_9,	// maps to 12
	SAVESTATUS_10,	// maps to 13
	SAVESTATUS_11,	// maps to 14
	SAVESTATUS_12,	// maps to 15
	SAVESTATUS_13,	// maps to 16
	SAVESTATUS_14,	// maps to 17
	SAVESTATUS_15,	// maps to 18
	SAVESTATUS_16,	// maps to 19
	SAVESTATUS_17,	// maps to 20
	SAVESTATUS_18,	// maps to 21
	SAVESTATUS_19,	// maps to 22
	SAVESTATUS_20,	// maps to 23
	SAVESTATUS_21,	// maps to 24
	SAVESTATUS_22,	// not used?
	SAVESTATUS_23,	// maps to 25
	SAVESTATUS_24,	// maps to 26
	SAVESTATUS_25,	// maps to 27
	SAVESTATUS_26,	// maps to 28
};

// Base packet type
struct D2Packet
{
	// Each packet contains a single byte header, followed by variable data.
	BYTE nPacketType;

	union
	{
		struct
		{	// packet 0x6C on C->S (1.10)
			DWORD dwTickCount;
#if GAME_MINOR_VERSION >= 11
			DWORD dwDelay;
#endif 
			DWORD dwUnknown;
		} Ping;

		struct
		{	// packet 0x01 (1.10)
			BYTE nDifficulty;
			DWORD unk1;
			BYTE nExpansion;
			BYTE nUnk4;
			WORD unk2;
		} ServerMetaData;

		struct
		{	// packet 0xAE (1.10)
			BYTE nCompressionType;
		} ServerCompressionInfo;

		struct
		{	// packet 0xB2 (1.10)
			BYTE nUnk1;
			BYTE nUnk2; // boolean value
			DWORD nUnk3;
		} ServerSaveFile;

		struct
		{	// packet 0xB3 (1.10)
			BYTE nSaveStatus;
		} ServerSaveStatus;

		struct
		{
#if GAME_MINOR_VERSION >= 11
#else
			char szGameName[24];	// in battle.net games, this is the room name. it is a single byte (0x01) in others.
			BYTE nGameMode;			// 1 in TCP, 2 in singleplayer, 0 in other contexts
			BYTE nClass;			// character class
			BYTE nTemplate;			// unused Character Template system remnants
			BYTE nDifficulty;		// normal/nightmare/hell
			char szCharName[16];	// character name
			WORD nUnknown1;
			DWORD nUnknown2;
			BYTE nUnknown3;
			BYTE nUnknown4;
			BYTE nLocale;			// the language. for localization purposes maybe?
#endif
		} ClientLocalJoinRequest;

		struct
		{
#if GAME_MINOR_VERSION >= 11
#else
			DWORD unk1;	// edx (0x00) ? // +00
			WORD unk2; // cx (0x01) ?	// +04
			BYTE nCharClass; // eax (0x0A) ?	// +06
			DWORD dwVersion;					// +07
			BYTE nLocale; // language	// +0B
			char szCharName[16];		// +0C
#endif
		} ClientRemoteJoinRequest;

		struct
		{
			BYTE nChunkSize;		// how big the chunk to send is, in bytes
			DWORD dwSaveSize;		// how big the save file is, in bytes
			BYTE nChunkBytes[256];	// the chunk bytes themselves
		} ClientSendSaveChunk;
	} packetData;

	// Methods to read and write
	size_t ReadServer(char* buffer, size_t bufferSize);
	size_t WriteServer(char* buffer, size_t bufferSize);

	size_t ReadClient(char* buffer, size_t bufferSize);
	size_t WriteClient(char* buffer, size_t bufferSize);
};