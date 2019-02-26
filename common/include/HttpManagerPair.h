#pragma once

//web(http,websocket)

#include "CustomForwardDeclare.h"

#include "WebBaseInterface.h"
#include "SessionManager.h"

#include <thread>
#include <mutex>


class HttpManagerPair : public WebBaseInterface, SessionManager<uWS::HttpResponse<false>*>
{
public: //statck function
	static HttpManagerPair* defaultHttpPair();
	static HttpManagerPair* _sDefaultHttpPair;
public: //override functions
	virtual void NotifyClientResponse() override;
	virtual void PushClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len) override;
	virtual bool InitWebManager(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue) override;
public:
	HttpManagerPair();
	void StartWebManagerPair();
	virtual ~HttpManagerPair();
	void RecvHttpDataThreadFunc();

	

	void SendClientResponse();

private:
	
private:

	std::string _httpIP="";
	uint16_t	_httpPort=0;
	std::string _httpsIP="";
	uint16_t	_httpsPort = 0;
	std::string _wsIP = "";
	uint16_t	_wsPort = 0;
	std::string _wssIP = "";
	uint16_t	_wssPort = 0;
	std::shared_ptr<spdlog::logger> _logger;
	std::shared_ptr<std::thread> _recvHttpThread;

	uWS::Loop* _httpLoop = nullptr;

	std::shared_ptr<MessageQueueManager> _msgQueueP;
	uWS::Loop* _curLoop = nullptr;
	std::shared_ptr<std::recursive_mutex> _threadSafeMutex;
	std::shared_ptr<DBManager> _wsDbManager = nullptr;
};

