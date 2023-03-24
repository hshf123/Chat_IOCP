#pragma once

class Player
{
public:
	void GetPlayerInfoFromDB(const string& str, OUT int32& outId);
	void InsertPlayerInfo(int64 id, const string& str);

public:
	uint64 playerId = 0;
	wstring name;

	uint32 roomId;
	ClientSessionRef ownerSession;

private:
	USE_LOCK;
};

