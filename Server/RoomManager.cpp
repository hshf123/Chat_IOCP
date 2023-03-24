#include "pch.h"
#include "RoomManager.h"
#include "Room.h"

RoomManager GRoomManager;

RoomRef RoomManager::Add()
{
	RoomRef room = MakeShared<Room>();

	{
		WRITE_LOCK;
		room->roomId = _roomId;
		_rooms.insert({ _roomId, room });
		_roomId++;
	}

	return room;
}

bool RoomManager::Remove(uint32 roomId)
{
	WRITE_LOCK;
	uint32 count = _rooms.erase(roomId);
	if (count == 1)
		return true;

	return false;
}

RoomRef RoomManager::Find(uint32 roomId)
{
	WRITE_LOCK;
	auto it = _rooms.find(roomId);
	if (it == _rooms.end())
		return nullptr;

	return it->second;
}
