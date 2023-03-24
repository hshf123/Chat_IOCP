#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"
#include "Player.h"
#include "RoomManager.h"
#include "Room.h"
#include "Protocol.pb.h"
#include "ClientSessionManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	static Atomic<uint64> idGenerator = 1;
	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);

	int32 outId = 0;
	{
		clientSession->MyPlayer = MakeShared<Player>();
		clientSession->MyPlayer->GetPlayerInfoFromDB(pkt.name(), outId);
	}

	if (outId == 0)
	{
		// 새로 만들기
		loginPkt.set_isnewid(true);
		{
			Protocol::Player* player = new Protocol::Player();
			player->set_id(idGenerator);
			player->set_name(pkt.name());
			loginPkt.set_allocated_player(player);
		}
		{
			clientSession->MyPlayer = MakeShared<Player>();
			PlayerRef& player = clientSession->MyPlayer;
			player->playerId = idGenerator++;
			player->name = Utils::ConvertStringToWString(pkt.name());
			player->ownerSession = clientSession;
		}
	
		// DB에 추가
		{
			clientSession->MyPlayer->InsertPlayerInfo(clientSession->MyPlayer->playerId, pkt.name());
		}
	}
	else
	{
		// DB 데이터 채워넣기
		if (GSessionManager.DuplicationCheck(outId) == false)
			session->Disconnect(L"이미 접속중인 이름");

		loginPkt.set_isnewid(false);
		{
			Protocol::Player* player = new Protocol::Player();
			player->set_id(outId);
			player->set_name(pkt.name());
			loginPkt.set_allocated_player(player);
		}
		{
			clientSession->MyPlayer = MakeShared<Player>();
			PlayerRef& player = clientSession->MyPlayer;
			player->playerId = outId;
			player->name = Utils::ConvertStringToWString(pkt.name());
			player->ownerSession = clientSession;
		}
	}

	// 모든 방의 인원수를 체크해서 표시
	for (uint32 i = 1; i <= GRoomManager.GetRoomCount(); i++)
	{
		RoomRef room = GRoomManager.Find(i);
		Protocol::RoomInfo* roomInfo = loginPkt.add_roominfos();
		room->FillRoomInfo(roomInfo);
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	Protocol::S_ENTER_GAME enterPkt;

	RoomRef room = GRoomManager.Find(pkt.roomid());
	if (room->Enter(clientSession->MyPlayer) == false)
	{
		enterPkt.set_success(false);
		for (uint32 i = 1; i <= GRoomManager.GetRoomCount(); i++)
		{
			RoomRef room = GRoomManager.Find(i);
			Protocol::RoomInfo* roomInfo = enterPkt.add_roominfos();
			roomInfo->set_id(i);
			roomInfo->set_playercount(room->GetPlayerCount());
		}
	}
	else
	{
		enterPkt.set_success(true);
		clientSession->MyPlayer->roomId = pkt.roomid();

		Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();
		room->FillRoomInfo(roomInfo);
		enterPkt.set_allocated_roominfo(roomInfo);

		// 새로운 참여자 브로드캐스트.
		Protocol::S_ROOM_UPDATE updatePkt;
		Protocol::RoomInfo* roomInfo2 = new Protocol::RoomInfo();
		room->FillRoomInfo(roomInfo2);
		updatePkt.set_allocated_roominfo(roomInfo2);
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(updatePkt);
		room->Broadcast(sendBuffer, clientSession);
	}

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(enterPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt)
{
	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	if (clientSession->MyPlayer == nullptr)
		return false;

	RoomRef room = GRoomManager.Find(clientSession->MyPlayer->roomId);
	if (room == nullptr)
		return false;

	// 방에서 제거
	room->Leave(clientSession->MyPlayer);

	{
		// 제거된 걸 원래 방에 있던 사람들에게 전송
		Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();
		room->FillRoomInfo(roomInfo);
		Protocol::S_ROOM_UPDATE updatePkt;
		updatePkt.set_allocated_roominfo(roomInfo);
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(updatePkt);
		room->Broadcast(sendBuffer);
	}

	{
		// 제거 된 유저를 로비로
		Protocol::S_LEAVE_GAME leavePkt;
		for (uint32 i = 1; i <= GRoomManager.GetRoomCount(); i++)
		{
			RoomRef room = GRoomManager.Find(i);
			Protocol::RoomInfo* roomInfo = leavePkt.add_roominfos();
			room->FillRoomInfo(roomInfo);
		}
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(leavePkt);
		clientSession->Send(sendBuffer);
	}
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	RoomRef room = GRoomManager.Find(clientSession->MyPlayer->roomId);
	if (room == nullptr)
		return false;

	Protocol::S_CHAT chatPkt;
	Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();
	room->FillRoomInfo(roomInfo);
	chatPkt.set_allocated_roominfo(roomInfo);
	Protocol::Player* player = new Protocol::Player();
	player->set_id(clientSession->MyPlayer->playerId);
	player->set_name(Utils::ConvertWStringToString(clientSession->MyPlayer->name));
	player->set_roomid(clientSession->MyPlayer->roomId);
	chatPkt.set_allocated_player(player);
	chatPkt.set_msg(pkt.msg());
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);
	room->Broadcast(sendBuffer);

	return true;
}
