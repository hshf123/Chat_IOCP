#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "Managers.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	if (pkt.success() == false)
		return false;

	if (pkt.roominfos().size() == 0)
	{
		// 뭔가 문제있는 상황
		return false;
	}

	if (pkt.isnewid())
	{
		cout << "새롭게 계정을 생성했습니다." << endl;
		wcout.imbue(locale("kor"));
		wcout << L"안녕하세요 " << Utils::ConvertStringToWString(pkt.player().name()) << L"님" << endl;
	}
	else
	{
		cout << "계정을 불러왔습니다." << endl;
		wcout.imbue(locale("kor"));
		wcout << L"안녕하세요 " << Utils::ConvertStringToWString(pkt.player().name()) << L"님" << endl;
	}

	auto roomInfos = pkt.roominfos();
	for (Protocol::RoomInfo roomInfo : roomInfos)
	{
		cout << roomInfo.id() << "번방 |" << " Player Count : " << roomInfo.playercount() << endl;
	}

	Protocol::C_ENTER_GAME enterGamePkt;
	while (true)
	{
		cout << "들어갈 방을 선택하세요." << endl;
		uint32 roomId;
		cin >> roomId;
		if (roomId < 1 || roomId > pkt.roominfos().size())
		{
			cout << "잘못된 방 번호입니다. 다시 입력하세요." << endl;
			continue;
		}

		enterGamePkt.set_roomid(roomId);
		break;
	}
	
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	GManagers.Broadcast(sendBuffer);

	GManagers.MyPlayer->playerId = pkt.player().id();
	GManagers.MyPlayer->name = Utils::ConvertStringToWString(pkt.player().name());

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	if (pkt.success())
	{
		// 내가 속한 방의 정보를 저장
		GManagers.MyPlayer->roomId = pkt.roominfo().id();
		GManagers.MyPlayer->State = PLAYER_STATE::WATINGROOM;
		GManagers.WaitingForGame(pkt.roominfo());
	}
	else
	{
		::system("cls");
		cout << "방을 다시 선택해주세요." << endl;
		auto roomInfos = pkt.roominfos();
		for (Protocol::RoomInfo roomInfo : roomInfos)
		{
			cout << roomInfo.id() << "번방 |" << " Player Count : " << roomInfo.playercount() << endl;
		}

		Protocol::C_ENTER_GAME enterGamePkt;
		while (true)
		{
			cout << "들어갈 방을 선택하세요." << endl;
			uint32 roomId;
			cin >> roomId;
			if (roomId < 1 || roomId > pkt.roominfos().size())
			{
				cout << "잘못된 방 번호입니다. 다시 입력하세요." << endl;
				continue;
			}

			enterGamePkt.set_roomid(roomId);
			break;
		}
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
		session->Send(sendBuffer);
	}

	return true;
}

bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
{
	if (pkt.roominfos().size() == 0)
		return false;

	GManagers.MyPlayer->roomId = 0;

	::system("cls");
	auto roomInfos = pkt.roominfos();
	for (Protocol::RoomInfo roomInfo : roomInfos)
	{
		cout << roomInfo.id() << "번방 |" << " Player Count : " << roomInfo.playercount() << endl;
	}

	Protocol::C_ENTER_GAME enterGamePkt;
	while (true)
	{
		cout << "들어갈 방을 선택하세요." << endl;
		uint32 roomId;
		cin >> roomId;
		if (roomId < 1 || roomId > pkt.roominfos().size())
		{
			cout << "잘못된 방 번호입니다. 다시 입력하세요." << endl;
			continue;
		}

		enterGamePkt.set_roomid(roomId);
		break;
	}

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
	GManagers.Broadcast(sendBuffer);
	return true;
}

bool Handle_S_ROOM_UPDATE(PacketSessionRef& session, Protocol::S_ROOM_UPDATE& pkt)
{
	if (pkt.roominfo().id() != GManagers.MyPlayer->roomId)
		return false;

	GManagers.WaitingForGame(pkt.roominfo());
	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	if (pkt.roominfo().id() != GManagers.MyPlayer->roomId)
		return false;

	GManagers.RecvChat(pkt);
	return true;
}
