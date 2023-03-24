#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"
#include "Managers.h"

ServerSession::~ServerSession()
{
	cout << "~ServerSession" << endl;
}

void ServerSession::OnConnected()
{
	GManagers.SessionManager.Add(static_pointer_cast<ServerSession>(shared_from_this()));
	cout << "OnConnected" << endl;

	Protocol::C_LOGIN pkt;
	wstring wname;
	while (true)
	{
		wcout.imbue(locale("kor"));
		wcin.imbue(locale("kor"));
		wcout << L"이름을 입력하세요 : ";
		wcin >> wname;
		// TODO : 이름 컨벤션
		if (wname.empty() == false)
			break;
	}

	string name = Utils::ConvertWStringToString(wname);
	pkt.set_name(name);
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Send(sendBuffer);
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void ServerSession::OnSend(int32 len)
{

}

void ServerSession::OnDisconnected()
{
	GManagers.SessionManager.Remove(static_pointer_cast<ServerSession>(shared_from_this()));
}
