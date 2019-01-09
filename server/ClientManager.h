﻿#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H
#include <stdint.h>
#include "util/Singleton.h"
#include "time/Timer.h"
#include "network/Acceptor.h"
#include "lua.hpp"
#include <vector>

class ClientChannel;

class ClientManager : public Singleton<ClientManager> {
public:
	ClientManager(uint32_t maxClient, uint8_t serverId);
	~ClientManager();

	void OnClientAccept(int fd, Network::Addr& addr);

	void MarkClientDead(ClientChannel* channel);

	void OnCheck();

	int AllocVid();

	ClientChannel* GetClient(int vid);

	void BindClient(int vid, ClientChannel* channel);

	void DeleteClient(int vid);

	int SendClient(int vid, char* data, size_t size);

	int BroadcastClient(std::vector<int>& vids, char* data, size_t size);

	int CloseClient(int vid);

	int Start(Network::Addr& addr);

	int Stop();

	void SetMaxFreq(uint32_t freq);

	uint32_t GetMaxFreq();

	void SetMaxAlive(uint32_t alive);

	uint32_t GetMaxAlive();

	void SetWarnFlow(uint32_t flow);	

	uint32_t GetWarnFlow();

	static int Register(lua_State* L);

	static int LStop(lua_State* L);

	static int LSendClient(lua_State* L);

	static int LBroadcastClient(lua_State* L);

	static int LCloseClient(lua_State* L);
private:
	uint8_t serverId_;

	uint32_t maxClient_;
	uint32_t allocStep_;
	uint32_t size_;
	
	ClientChannel** clientSlots_;

	Network::Acceptor* acceptor_;
	ev::check check_;

	std::vector<ClientChannel*> deadClients_;

	uint32_t maxFreq_;
	uint32_t maxAlive_;
	uint32_t warnFlow_;
};

#define CLIENT_MGR ClientManager::GetSingleton()

#endif