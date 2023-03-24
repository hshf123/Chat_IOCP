#include "pch.h"
#include "Player.h"

void Player::GetPlayerInfoFromDB(const string& str, OUT int32& outId)
{
	WRITE_LOCK;
	// DB에서 이름 체크 없으면 새로 만들기
	wstring wname = Utils::ConvertStringToWString(str);
	const WCHAR* name = wname.c_str();

	DBConnection* dbConn = GDBConnectionPool->Pop();
	const WCHAR* query = L"SELECT id, name 			\
							FROM [dbo].[Player]		\
							WHERE name = (?)";
	DBBind<1, 2> dbBind(*dbConn, query);

	dbBind.BindParam(0, *name);

	WCHAR outName[50];
	dbBind.BindCol(0, OUT outId);
	dbBind.BindCol(1, OUT outName);

	ASSERT_CRASH(dbBind.Execute());
	dbConn->Fetch();
	GDBConnectionPool->Push(dbConn);
}

void Player::InsertPlayerInfo(int64 id, const string& str)
{
	WRITE_LOCK;
	wstring wname = Utils::ConvertStringToWString(str);
	const WCHAR* name = wname.c_str();

	DBConnection* dbConn = GDBConnectionPool->Pop();
	const WCHAR* query = L"INSERT INTO [dbo].[Player] 		\
							([id], [name])					\
							VALUES (?, ?)";
	DBBind<2, 0> dbBind(*dbConn, query);
	
	dbBind.BindParam(0, id);
	dbBind.BindParam(1, *name);
	 
	ASSERT_CRASH(dbBind.Execute());
	GDBConnectionPool->Push(dbConn);
	return;
}
