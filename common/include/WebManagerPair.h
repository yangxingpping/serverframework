#pragma once

//web(http,websocket)

#include "CustomForwardDeclare.h"

#include "WebBaseInterface.h"
#include "SessionManager.h"

#include <thread>
#include <mutex>


class WebManagerPair : public WebBaseInterface, public SessionManager<uWS::WebSocket<false,true>*>, SessionManager<uWS::WebSocket<true, true>*>,
	SessionManager<uWS::HttpResponse<false>*>, SessionManager<uWS::HttpResponse<true>*>
{
public: //statck function
	static WebManagerPair* defaultHttpPair();
	static WebManagerPair* defaultWebSocketPair();
	static WebManagerPair* _sDefaultHttpPair;
	static WebManagerPair* _sDefaultHttpsPair;
	static WebManagerPair* _sDefaultWsPair;
	static WebManagerPair* _sDefaultWssPair;
public: //override functions
	virtual void NotifyHttpResponse() override;
	virtual void NotifyHttpsResponse() override;
	virtual void NotifyWsResponse() override;
	virtual void NotifyWssResponse() override;
	virtual void PushWebSocketClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len) override;
	virtual void PushHttpClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len) override;
	virtual void PushHttpsClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len) override;

public:
	WebManagerPair();
	bool InitHttpManagerPair(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue);
	bool InitHttpsManagerPair(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue);
	bool InitWsManagerPair(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue);
	bool InitWssManagerPair(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue);
	void StartWebManagerPair();
	virtual ~WebManagerPair();
	void RecvHttpDataThreadFunc();
	void RecvHttpsDataThreadFunc();
	void RecvWsDataThreadFunc();
	void RecvWssDataThreadFunc();

	

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
	std::shared_ptr<std::thread> _recvHttpsThread;
	std::shared_ptr<std::thread> _recvWsThread;
	std::shared_ptr<std::thread> _recvWssThread;

	uWS::Loop* _httpLoop = nullptr;
	uWS::Loop* _httpsLoop = nullptr;
	uWS::Loop* _wsLoop = nullptr;
	uWS::Loop* _wssLoop = nullptr;

	std::shared_ptr<MessageQueueManager> _msgQueueP;
	uWS::Loop* _curLoop = nullptr;

	std::shared_ptr<std::recursive_mutex> _threadSafeMutex;
};

