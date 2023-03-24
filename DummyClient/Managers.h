#pragma once
#include "Player.h"
#include "Protocol.pb.h"
#include "ServerSessionManager.h"

using PlayerRef = shared_ptr<Player>;

class Managers
{
public:
	void Init();
	void Update();
	void Broadcast(SendBufferRef sendBuffer);

	void InLobby();
	void WaitingForGame(const Protocol::RoomInfo& pkt);
	void Chat();
	void RecvChat(const Protocol::S_CHAT& pkt);
	void LeaveRoom();

public:
	PlayerRef MyPlayer;
	ServerSessionManager SessionManager;

private:
	USE_LOCK;
	Vector<pair<wstring, wstring>> _chatData;
};

extern Managers GManagers;