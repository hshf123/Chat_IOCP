#include "pch.h"
#include "CorePch.h"
#include "ThreadManager.h"

#include "Service.h"
#include "ClientSession.h"
#include "SendBuffer.h"
#include "ClientSessionManager.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "RoomManager.h"

int main()
{
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 18 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));
	
	{
		auto query = L"									\
			DROP TABLE IF EXISTS [dbo].[Player];		\
			CREATE TABLE [dbo].[Player]					\
			(											\
				[id] INT NOT NULL PRIMARY KEY,			\
				[name] NVARCHAR(50) NULL,				\
			);";
	
		DBConnection* dbConn = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		GDBConnectionPool->Push(dbConn);
	}

	for (int32 i = 1; i <= 5; i++)
	{
		GRoomManager.Add();
	}

	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>,
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}