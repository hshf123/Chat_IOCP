#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "ClientSession.h"

bool Room::Enter(PlayerRef player)
{
	WRITE_LOCK;

	if (_players.size() >= 3)
		return false;
	_players[player->playerId] = player;
	return true;
}

void Room::Leave(PlayerRef player)
{
	WRITE_LOCK;
	_players.erase(player->playerId);
	player->roomId = 0;
}

void Room::Broadcast(SendBufferRef sendBuffer, ClientSessionRef session /*= nullptr*/)
{
	WRITE_LOCK;
	for (auto& p : _players)
	{
		if (p.second->ownerSession == session)
			continue;
		p.second->ownerSession->Send(sendBuffer);
	}
}

PlayerRef Room::FindPlayer(uint64 playerId)
{
	WRITE_LOCK;
	auto it = _players.find(playerId);
	if (it == _players.end())
		return nullptr;

	return it->second;
}

void Room::FillRoomInfo(Protocol::RoomInfo* pkt)
{
	WRITE_LOCK;
	pkt->set_id(roomId);
	pkt->set_playercount(_players.size());
	for (auto& h : _players)
	{
		if (roomId == h.second->roomId)
		{
			Protocol::Player* player = pkt->add_players();
			player->set_id(h.second->playerId);
			player->set_name(Utils::ConvertWStringToString(h.second->name));
			player->set_roomid(h.second->roomId);
		}
	}
}
