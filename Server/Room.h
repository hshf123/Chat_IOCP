#pragma once

class Room
{
public:
	bool Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer, ClientSessionRef session = nullptr);
	PlayerRef FindPlayer(uint64 playerId);
	uint32 GetPlayerCount() { return _players.size(); }
	void FillRoomInfo(Protocol::RoomInfo* pkt);

public:
	uint32 roomId = 1;

private:
	USE_LOCK;
	HashMap<uint64, PlayerRef> _players;
};
