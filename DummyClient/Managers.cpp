#include "pch.h"
#include "Managers.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"

Managers GManagers;

void Managers::Init()
{
	MyPlayer = MakeShared<Player>();
	MyPlayer->State = PLAYER_STATE::INLOBBY;
}

void Managers::Update()
{
	switch (MyPlayer->State)
	{
	case PLAYER_STATE::INLOBBY:
		InLobby();
		break;
	case PLAYER_STATE::WATINGROOM:
	case PLAYER_STATE::INGAME:
		Chat();
		break;
	default:
		break;
	}
}

void Managers::Broadcast(SendBufferRef sendBuffer)
{
	SessionManager.Broadcast(sendBuffer);
}

void Managers::InLobby()
{
	if (MyPlayer->State != PLAYER_STATE::INLOBBY)
		return;
}

void Managers::WaitingForGame(const Protocol::RoomInfo& pkt)
{
	if (MyPlayer->State != PLAYER_STATE::WATINGROOM)
		return;

	WRITE_LOCK;
	::system("cls");
	cout << "현재 방은 " << pkt.id() << "번 방 입니다." << " 인원 : " << pkt.playercount() << " / 3" << endl;
	cout << "| 명단 | ";
	for (auto p : pkt.players())
	{
		wcout.imbue(locale("kor"));
		wstring name = Utils::ConvertStringToWString(p.name());
		wcout << "| ";
		if (name == MyPlayer->name)
		{
			cout << "나 : ";
		}
		wcout << Utils::ConvertStringToWString(p.name()) << " | ";
	}
	cout << endl;
	cout << "방을 나가시려면 \'/exit\'를 입력하세요." << endl;


	wcout.imbue(locale("kor"));
	for (auto& p : _chatData)
	{
		wcout << p.first << L" : " << p.second << endl;
	}
}

void Managers::Chat()
{
	if (MyPlayer->State == PLAYER_STATE::WATINGROOM || MyPlayer->State == PLAYER_STATE::INGAME)
	{
		wstring msg;
		wcin.imbue(locale("kor"));
		::getline(wcin, msg);

		if (msg.empty())
			return;

		if (msg == L"/exit")
		{
			LeaveRoom();
		}

		Protocol::C_CHAT chatPkt;
		chatPkt.set_msg(Utils::ConvertWStringToString(msg));
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(chatPkt);
		Broadcast(sendBuffer);
	}
}

void Managers::RecvChat(const Protocol::S_CHAT& pkt)
{
	{
		WRITE_LOCK;
		wstring msg = Utils::ConvertStringToWString(pkt.msg());
		wstring name = Utils::ConvertStringToWString(pkt.player().name());
		_chatData.push_back({ name, msg });
	}

	GManagers.WaitingForGame(pkt.roominfo());
}

void Managers::LeaveRoom()
{
	MyPlayer->State = PLAYER_STATE::INLOBBY;
	Protocol::C_LEAVE_GAME leavePkt;
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(leavePkt);
	Broadcast(sendBuffer);
}
