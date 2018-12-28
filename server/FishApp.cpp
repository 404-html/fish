﻿#include "FishApp.h"
#include "Logger.h"
#include "util/format.h"
#include "network/HttpChannel.h"

FishApp::FishApp(std::string file) : file_(file)
{
	clientAcceptor_ = new Network::Acceptor(poller_);
	httpAcceptor_ = new Network::Acceptor(poller_);
}

FishApp::~FishApp(void)
{
}

int FishApp::Init()
{
	LOG_ERROR(fmt::format("FishApp start:{}", file_));
	ServerApp::Init();

	Network::Acceptor::OnConnection callback = std::bind(&FishApp::OnClientAccept, this, std::placeholders::_1, std::placeholders::_2);
	clientAcceptor_->SetCallback(callback);

	Network::Acceptor::OnConnection httpCallback = std::bind(&FishApp::OnHttpAccept, this, std::placeholders::_1, std::placeholders::_2);

	httpAcceptor_->SetCallback(httpCallback);

	lua_->DoFile(file_);

	return 0;
}

int FishApp::Fina()
{
	ServerApp::Fina();
	return 0;
}

int FishApp::Run()
{
	ServerApp::Run();
	return 0;
}

void FishApp::OnUpate(Timer* timer, void* userdata)
{
	ServerApp::OnUpate(timer, userdata);
}

void FishApp::ListenClient(std::string ip,int port)
{
	clientAcceptor_->Listen(ip.c_str(), port);
}

void FishApp::ListenHttp(std::string ip,int port)
{
	httpAcceptor_->Listen(ip.c_str(), port);
}

void FishApp::OnClientAccept(int fd, Network::Addr& addr)
{
	std::cout << addr.ToStr() << std::endl;
}

void FishApp::OnHttpAccept(int fd, Network::Addr& addr)
{
	Network::HttpChannel* channel = new Network::HttpChannel(poller_,fd);
	channel->SetCallback(std::bind(&FishApp::OnHttpComplete, this, std::placeholders::_1, std::placeholders::_2), NULL);
	channel->EnableRead();
}

void FishApp::OnHttpComplete(Network::HttpChannel* channel, void* userdata) {
	std::cout << "method:" << channel->GetMethod() << std::endl;
	std::cout << "url:" << channel->GetUrl() << std::endl;
	std::cout << "content:" << channel->GetContent() << std::endl;

	std::map<std::string,std::string>& headers = channel->GetHeader();
	std::map<std::string,std::string>::iterator iter = headers.begin();
	for(;iter != headers.end();iter++) {
		std::cout << iter->first << ":" << iter->second << std::endl;
	}
	channel->Close(true);
}
