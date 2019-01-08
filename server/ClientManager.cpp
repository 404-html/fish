﻿#include "ClientManager.h"
#include "ClientChannel.h"
#include "network/Network.h"
#include <stdlib.h>
#include <string.h>

template <>
ClientManager * Singleton<ClientManager>::singleton_ = 0;

ClientManager::ClientManager(Network::EventPoller* poller, uint32_t maxClient, uint8_t serverId) {
	serverId_ = serverId;
	maxClient_ = maxClient;
	allocStep_ = 0;
	size_ = 0;

	clientMgr_ = (ClientChannel**)malloc(sizeof( *clientMgr_ ) * maxClient);
	memset(clientMgr_, 0, sizeof( *clientMgr_ ) * maxClient);

	poller = poller_;

	using namespace std::placeholders;

	acceptor_ = new Network::Acceptor(poller_);
	acceptor_->SetCallback(std::bind(&ClientManager::OnClientAccept, this, _1, _2));

	timer_ = new Timer();
	timer_->SetCallback(std::bind(&ClientManager::OnUpate, this, _1, _2));
	timer_->Start(poller_, 1, 1);

	check_.set(poller_->GetLoop());
	check_.set<ClientManager, &ClientManager::OnCheck>(this);
	check_.start();
}


ClientManager::~ClientManager() {
	free(clientMgr_);
	delete acceptor_;
	delete timer_;
	check_.stop();
}

void ClientManager::OnClientAccept(int fd, Network::Addr& addr) {
	if (size_ >= maxClient_) {
		Network::SocketClose(fd);
		return;
	}

	ClientChannel* channel = new ClientChannel(poller_, fd);
	channel->SetId(AllocVfd());
	BindClient(channel->GetId(), channel);
	channel->EnableRead();
}

void ClientManager::MarkClientDead(ClientChannel* channel) {
	deadClients_.push_back(channel);
}

void ClientManager::OnUpate(Timer* timer, void* userdata) {

}

void ClientManager::OnCheck() {
	for(int i = 0; i < deadClients_.size();i++) {
		ClientChannel* channel = deadClients_[i];
		delete channel;
	}
	deadClients_.clear();
}

int ClientManager::AllocVfd() {
	if ( size_ >= maxClient_ ) {
		return -1;
	}

	for ( ;; ) {
		int vfd = (allocStep_++) % maxClient_;
		if ( !clientMgr_[vfd] ) {
			return vfd << 8 | serverId_;
		}
	}
	assert(0);
	return -1;
}

void ClientManager::BindClient(int id, ClientChannel* channel) {
	int serverId = id & 0xff;
	if ( serverId != serverId_) {
		return;
	}
	int vfd = id >> 8;
	assert(clientMgr_[vfd] == NULL);
	clientMgr_[vfd] = channel;
}

void ClientManager::DeleteClient(int id) {
	int serverId = id & 0xff;
	if ( serverId != serverId_ ) {
		return;
	}
	int vfd = id >> 8;
	assert(clientMgr_[vfd] != NULL);
	clientMgr_[vfd] = NULL;
}

ClientChannel* ClientManager::GetClient(int id) {
	int serverId = id & 0xff;
	if ( serverId != serverId_ ) {
		return NULL;
	}
	int vfd = id >> 8;
	ClientChannel* channel = clientMgr_[vfd];
	if ( !channel ) {
		return NULL;
	}
	return channel;
}

int ClientManager::SendClient(int id, char* data, size_t size) {
	ClientChannel* channel = GetClient(id);
	if ( !channel ) {
		return -1;
	}
	return channel->Write(data, size);
}

int ClientManager::BroadClient(std::vector<int>& ids, char* data, size_t size) {
	for ( int i = 0; i < ids.size();i++ ) {
		int id = ids[i];
		char* message = (char*)malloc(size);
		memcpy(message, data, size);
		SendClient(id, message, size);
	}
	free(data);
	return 0;
}

int ClientManager::CloseClient(int id) {
	ClientChannel* channel = GetClient(id);
	if ( !channel ) {
		return -1;
	}
	if (!channel->IsAlive()) {
		return -1;
	}
	channel->Close(true);
	return 0;
}