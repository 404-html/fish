#include "Logger.h"
#include "network/Connector.h"
#include "util/format.h"
#include <iostream>
#include <assert.h>

template <> 
Logger * Singleton<Logger>::singleton_ = 0;

Logger::Loglevel Logger::level_ = Trace;

Logger::Logger(Network::Addr& addr, ServerApp* app):addr_(addr) {
	app_ = app;
	
	Network::Connector connector(app_->Poller());
	int fd = connector.Connect(addr_, false);
	assert(fd > 0);

	channel_ = new LoggerChannel(app_->Poller(), fd);
	channel_->SetCloseCallback(std::bind(&Logger::OnChannelClose, this, std::placeholders::_1));

	timer_ = new Timer();
	timer_->SetCallback(std::bind(&Logger::OnUpdate, this, std::placeholders::_1, std::placeholders::_2));
	timer_->Start(app->Poller(), 1, 1);
}

Logger::~Logger(void) {
	if (channel_) {
		delete channel_;
	}
	delete timer_;
}

void Logger::Log(const char* file,int line,Loglevel level,const char* content) {
	if (level < Logger::level_) {
		return;
	}
	std::lock_guard<std::mutex> guard(mutex_);
	std::string log = fmt::format("@{}:{}: {}\n", file, line, content);
	if (channel_) {
		channel_->Write(log);
	} else {
		cached_.push_back(log);
	}
}

void Logger::LuaLog(const char* content) {
	std::lock_guard<std::mutex> guard(mutex_);
	std::string log = fmt::format("@lua: {}\n",content);
	if (channel_) {
		channel_->Write(log);
	} else {
		cached_.push_back(log);
	}
}

void Logger::OnChannelClose(void* userdata) {
	delete channel_;
	channel_ = NULL;
}

void Logger::OnUpdate(Timer* timer, void* userdata) {
	if (!channel_) {
		return;
	}

	Network::Connector connector(app_->Poller());
	int fd = connector.Connect((const Network::Addr&)addr_, false);
	if (fd < 0) {
		return;
	}
	
	channel_ = new LoggerChannel(app_->Poller(), fd);
	channel_->SetCloseCallback(std::bind(&Logger::OnChannelClose, this, std::placeholders::_1));

	std::vector<std::string>::iterator iter = cached_.begin();
	for(;iter != cached_.end();iter++) {
		std::string log = *iter;
		channel_->Write(log);
	}
	cached_.clear();
}

void Logger::SetLogLevel(Loglevel level) {
	Logger::level_ = level;
}

Logger::Loglevel Logger::LogLevel() {
	return Logger::level_;
}
