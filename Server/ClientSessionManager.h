#pragma once

using ClientSessionRef = shared_ptr<class ClientSession>;

class ClientSessionManager
{
public:
	void Add(ClientSessionRef session);
	void Remove(ClientSessionRef session);

	bool DuplicationCheck(const uint64& id);
	
private:
	USE_LOCK;
	Set<ClientSessionRef> _sessions;
};

extern ClientSessionManager GSessionManager;