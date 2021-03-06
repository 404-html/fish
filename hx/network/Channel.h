﻿#ifndef CHANNEL_H
#define CHANNEL_H
#include <stdint.h>
#include <vector>
#include <map>
#include <queue>

#include "ServerApp.h"
#include "EventPoller.h"
#include "Reader.h"
#include "Writer.h"
#include "util/MemoryStream.h"

class ServerApp;

namespace Network {
	class Channel {	
	public:
		enum eChannelState {eAlive,eClosed,eInvalid};

	public:
		Channel(Network::EventPoller* poller,int fd);
		virtual ~Channel();

		virtual bool IsAlive();

		virtual void Close(bool rightnow = false);

		virtual void EnableRead();
		virtual void DisableRead();

		virtual void EnableWrite();
		virtual void DisableWrite();

		virtual void SetReader(Reader* reader);
		virtual void SetWriter(Writer* writer);

		virtual void HandleRead();
		virtual void HandleWrite();
		virtual void HandleClose();
		virtual void HandleError();

		virtual int Write(char* data,int size, uint32_t* reference);
		virtual int Write(std::string& data, uint32_t* reference);

	private:
		virtual void Clean();

		virtual void OnRead(ev::io &rio, int revents);
		virtual void OnWrite(ev::io &wio, int revents);

	protected:
		eChannelState state_;

		Network::EventPoller* poller_;
		ev::io rio_;
		ev::io wio_;
		int fd_;

		Reader* reader_;
		Writer* writer_;
	};
}


#endif
