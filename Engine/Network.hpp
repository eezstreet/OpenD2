#pragma once
#include "../Shared/D2Shared.hpp"

// Network.cpp
namespace Network
{
	void SendServerPacket(int nClientMask, D2Packet* pPacket);
	void SendClientPacket(D2Packet* pPacket);
	void SetMaxPlayerCount(DWORD dwNewPlayerCount);
	DWORD ReadClientPackets(DWORD dwTimeout);
	DWORD ReadServerPackets(DWORD dwTimeout);
	bool ConnectToServer(char* szServerAddress, DWORD dwPort);
	void DisconnectFromServer();
	void StartListen(DWORD dwPort);
	void StopListening();
	void Init();
	void Shutdown();
};