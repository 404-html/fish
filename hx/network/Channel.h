﻿#ifndef CHANNEL_H
#define CHANNEL_H
#include <vector>
#include <map>
#include <queue>


#include "EventPoller.h"
#include "TcpReader.h"
#include "../ServerApp.h"
#include "../util/MemoryStream.h"

class ServerApp;

namespace Network
{
	class Channel
	{	
	public:
		enum eChannelState {Alive,Closed,Error,Invalid};

	public:
		Channel(Network::EventPoller* poller,int fd);
		virtual ~Channel();

		virtual void Close(bool rightnow = false);

		virtual void EnableRead();

		virtual void DisableRead();

		virtual void EnableWrite();

		virtual void DisableWrite();

		virtual void HandleRead();

		virtual void HandleWrite();

		virtual void HandleError();

		virtual int Write(char* data,int size);

		virtual int Write(MemoryStream* ms);

		virtual void SetReader(Reader * reader);

		virtual bool IsAlive();

		virtual int Forward(const char * ptr,int size) = 0;

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
