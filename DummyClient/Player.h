#pragma once

enum class PLAYER_STATE : uint8
{
	NONE,
	INLOBBY,
	WATINGROOM,
	INGAME,
};

class Player
{
public:
	uint64 playerId = 0;
	wstring name;

	uint32 roomId;

	PLAYER_STATE State = PLAYER_STATE::NONE;
};
