#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "SendBuffer.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"
#include "Managers.h"

char sendData[] = "Hello World";

int main()
{
	ServerPacketHandler::Init();

	this_thread::sleep_for(1s);

	ClientServiceRef clientService = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		1);

	ASSERT_CRASH(clientService->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					clientService->GetIocpCore()->Dispatch();
				}
			});
	}

	GManagers.Init();

	while (true)
	{
		GManagers.Update();
	}

	GThreadManager->Join();
}
