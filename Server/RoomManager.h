#pragma once

class RoomManager
{
public:
	RoomRef Add();
	bool Remove(uint32 roomId);
	RoomRef Find(uint32 roomId);
	uint32 GetRoomCount() { return _rooms.size(); }

private:
	USE_LOCK;
	HashMap<uint32, RoomRef> _rooms;
	uint32 _roomId = 1;
};

extern RoomManager GRoomManager;