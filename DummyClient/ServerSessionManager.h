#pragma once

class ServerSessionManager
{
public:
	void Add(ServerSessionRef session);
	void Remove(ServerSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	Set<ServerSessionRef> _sessions;
};
