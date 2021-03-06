﻿#ifndef CLIENT_CHANNEL_H
#define CLIENT_CHANNEL_H

#include "time/Timer.h"
#include "network/Channel.h"

namespace Network {
	class ClientChannel : public Channel {
	public:
		typedef Channel Super;

	public:
		ClientChannel(EventPoller* poller, int fd, int vid);

		~ClientChannel();

		virtual void HandleRead();

		virtual void HandleClose();

		virtual void HandleError();

		void OnUpdate(Timer* timer, void* userdata);

		void OnClientError(bool close = false);

		int GetVid();

	private:
		int vid_;
		uint32_t freq_;
		uint32_t need_;
		uint16_t seed_;
		uint64_t lastMsgTime_;
		Timer* timer_;
	};
}


#endif