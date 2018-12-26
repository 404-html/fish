#include "Channel.h"
#include "http_parser.h"

namespace Network {

class HttpChannel : public Channel {

public:
	HttpChannel(Network::EventPoller* poller,int fd);

	virtual ~HttpChannel();

	virtual void HandleRead();

	virtual void HandleError();

	virtual void SetComplete();

	virtual std::string GetMethod();
	
	virtual void SetUrl(const char* data, size_t size);

	virtual std::string GetUrl();

	virtual void SetStatus(const char* data, size_t size);

	virtual std::string GetStatus();
	
	virtual void SetHeader(std::string& field, std::string& value);

	virtual void SetContent(const char* data, size_t size);

	virtual std::string GetContent();

	virtual void SetReplyHeader(std::string& field, std::string& value);

	virtual void Reply(int code, std::string& content);

	static int OnParseBegin(struct http_parser* parser);
	static int OnParseComplete(struct http_parser* parser);

	static int OnChunkHeader(struct http_parser* parser);
	static int OnChunkComplete(struct http_parser* parser);

	static int OnUrl(struct http_parser* parser,const char* at,size_t length);
	static int OnStatus(struct http_parser* parser,const char* at,size_t length);

	static int OnHeaderField(struct http_parser* parser,const char* at,size_t length);
	static int OnHeaderValue(struct http_parser* parser,const char* at,size_t length);
	static int OnHeaderComplete(struct http_parser* parser);

	static int OnContent(struct http_parser* parser,const char* at,size_t length);

public:
	std::string field_;
	std::string value_;
	int phase_;
private:
	struct http_parser parser_;
	std::map<std::string,std::string> headers_;
	std::string url_;
	std::string content_;
	std::string status_;
	bool completed_;

	std::map<std::string,std::string> replyHeaders_;
};
};
