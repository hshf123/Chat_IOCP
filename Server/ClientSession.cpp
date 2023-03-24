#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "SendBuffer.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "RoomManager.h"
#include "Room.h"
#include "Protocol.pb.h"

void ClientSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
	if (MyPlayer == nullptr)
		return;

	RoomRef room = GRoomManager.Find((MyPlayer->roomId));
	if (room != nullptr)
	{
		room->Leave(MyPlayer);
		Protocol::RoomInfo* roomInfo = new Protocol::RoomInfo();
		room->FillRoomInfo(roomInfo);
		Protocol::S_ROOM_UPDATE updatePkt;
		updatePkt.set_allocated_roominfo(roomInfo);
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(updatePkt);
		room->Broadcast(sendBuffer);
	}

	MyPlayer->ownerSession = nullptr;
	MyPlayer = nullptr;
}


void ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader header = *reinterpret_cast<PacketHeader*>(buffer);

	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void ClientSession::OnSend(int32 len)
{
}
