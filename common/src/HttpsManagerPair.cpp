
#include "ThirdPartyuWS.h"
#include "libusockets_new.h"

#include "spdlog/spdlog.h"
#include "SessionManager.h"
#include "HttpsManagerPair.h"
#include "WebBaseInterface.h"
#include "MessageQueueManager.h"
#include "DBManager.h"

#include "comm_def.h"

#include <functional>


HttpsManagerPair* HttpsManagerPair::defaultHttpsPair()
{
	if (_sDefaultHttpsPair)
	{
		return _sDefaultHttpsPair;
	}
	else
	{
		assert(false);
		return nullptr;
	}
}

HttpsManagerPair* HttpsManagerPair::_sDefaultHttpsPair = nullptr;

void HttpsManagerPair::NotifyClientResponse()
{
	_wssLoop->defer([this]() {
		_sDefaultHttpsPair->SendClientResponse();
		});
}

void HttpsManagerPair::PushWebSocketClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len)
{
	if (sid == INVALID_SESSION_ID || !msg)
	{ //record some error message
		return;
	}
	_msgQueueP->AddWebResponseMessage(sid, msg, len, type);
}

void HttpsManagerPair::PushHttpClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len)
{
	if (sid == INVALID_SESSION_ID || !msg)
	{ //record some error message
		return;
	}
	_msgQueueP->AddHttpResponseMessage(sid, msg, len, type);
}

void HttpsManagerPair::PushHttpsClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len)
{
	if (sid == INVALID_SESSION_ID || !msg)
	{ //record some error message
		return;
	}
	_msgQueueP->AddHttpsResponseMessage(sid, msg, len, type);
}

HttpsManagerPair::HttpsManagerPair()
{
	_threadSafeMutex = std::make_shared<std::recursive_mutex>();
	_wsDbManager = std::make_shared<DBManager>();
	_wsDbManager->startDBManager();
}

bool HttpsManagerPair::InitHttpManagerPair(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue)
{
	bool bret = false;
	
	_httpIP = ip;
	_httpPort = port;
	_msgQueueP = msgqueue;
	_logger = spdlog::get(LOGGER_NAME);
	_recvHttpThread = std::make_shared<std::thread>(&HttpsManagerPair::RecvHttpDataThreadFunc, this);
	if (_logger)
	{
		bret = true;
	}
	_sDefaultHttpsPair = this;
	
	return bret;
}

bool HttpsManagerPair::InitHttpsManagerPair(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue)
{
	bool bret = false;
	_httpsIP = ip;
	_httpsPort = port;
	_msgQueueP = msgqueue;
	_logger = spdlog::get(LOGGER_NAME);
	_recvHttpsThread = std::make_shared<std::thread>(&HttpsManagerPair::RecvHttpsDataThreadFunc, this);
	if (_logger)
	{
		bret = true;
	}
	_sDefaultHttpsPair = this;
	return bret;
}

bool HttpsManagerPair::InitWsManagerPair(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue)
{
	bool bret = false;
	_wsIP = ip;
	_wsPort = port;
	_msgQueueP = msgqueue;
	_logger = spdlog::get(LOGGER_NAME);
	_recvWsThread = std::make_shared<std::thread>(&HttpsManagerPair::RecvWsDataThreadFunc, this);
	if (_logger)
	{
		bret = true;
	}
	_sDefaultHttpsPair = this;
	return bret;
}

bool HttpsManagerPair::InitWssManagerPair(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue)
{
	bool bret = false;
	_wssIP = ip;
	_wssPort = port;
	_msgQueueP = msgqueue;
	_logger = spdlog::get(LOGGER_NAME);
	_recvWssThread = std::make_shared<std::thread>(&HttpsManagerPair::RecvWssDataThreadFunc, this);
	if (_logger)
	{
		bret = true;
	}
	_sDefaultHttpsPair = this;
	return bret;
}

void HttpsManagerPair::StartWebManagerPair()
{
	
	if (_recvHttpThread && _recvHttpThread->joinable())
	{
		_recvHttpThread->join();
	}
	if (_recvHttpsThread && _recvHttpsThread->joinable())
	{
		_recvHttpsThread->join();
	}
	if (_recvWsThread && _recvWsThread->joinable())
	{
		_recvWsThread->join();
	}
	if (_recvWssThread && _recvWssThread->joinable())
	{
		_recvWssThread->join();
	}
}

HttpsManagerPair::~HttpsManagerPair()
{
}

void HttpsManagerPair::RecvHttpDataThreadFunc()
{
	using WebSocketBehavior = typename uWS::TemplatedApp<false>::WebSocketBehavior;
	using WebSocket = uWS::WebSocket<false, true>;
	using HttpRequest = uWS::HttpRequest;
	using HttpResponse = uWS::HttpResponse<false>;

	{
		std::lock_guard<std::recursive_mutex> lock(*_threadSafeMutex);
		_httpLoop = uWS::Loop::defaultLoop();
	}
	
	uWS::App app;
	app.get("/*", [this](HttpResponse* resp, HttpRequest* req)
		{
			auto url = req->getUrl();
			auto method = req->getMethod();
			resp->onAborted([]() {
				std::cout << "aborted" << std::endl;
				});
			if (method == "get") //get method
			{
				auto query = req->getQuery();
				if (query.length() <= 2)
				{
					resp->end("invalid parameters");
					return;
				}
				auto sessionid = static_cast<SessionManager<uWS::HttpResponse<false>*>*>(this)->AddSession(resp);
				_msgQueueP->AddHttpRequestMessage(sessionid, query.data() + 1, query.length() - 1);
			}
		});
	
	app.listen(_httpPort, [](auto* token) {
		});
	app.run();

}


void HttpsManagerPair::RecvHttpsDataThreadFunc()
{
	using namespace uWS;
	using WebSocketBehavior = typename uWS::TemplatedApp<false>::WebSocketBehavior;
	using WebSocket = uWS::WebSocket<false, true>;
	using HttpRequest = uWS::HttpRequest;
	using HttpResponse = uWS::HttpResponse<true>;

	{
		std::lock_guard<std::recursive_mutex> lock(*_threadSafeMutex);
		_httpsLoop = uWS::Loop::defaultLoop();
	}
	struct us_new_socket_context_options_t ssl_options {"server.key", "server.pem", "123456789",nullptr};
	uWS::SSLApp* app = nullptr;
	{
		std::lock_guard<std::recursive_mutex> lock(*_threadSafeMutex);
		app = new uWS::SSLApp(ssl_options);
	}
	app->get("/*", [this](HttpResponse* resp, HttpRequest* req)
		{
			auto url = req->getUrl();
			auto method = req->getMethod();
			resp->onAborted([]() {
				std::cout << "aborted" << std::endl;
				});
			if (method == "get") //get method
			{
				auto query = req->getQuery();
				if (query.length() <= 2)
				{
					resp->end("invalid parameters");
					return;
				}
				auto sessionid = static_cast<SessionManager<struct uWS::HttpResponse<true>*>*>(this)->AddSession(resp);
				_msgQueueP->AddHttpsRequestMessage(sessionid, query.data() + 1, query.length() - 1);
			}
		});
	app->listen(_httpsPort, [](auto* token) {
		});
	app->run();
}

void HttpsManagerPair::RecvWsDataThreadFunc()
{
	using WebSocketBehavior = typename uWS::TemplatedApp<false>::WebSocketBehavior;
	using WebSocket = uWS::WebSocket<false, true>;
	using HttpRequest = uWS::HttpRequest;
	
	auto funcOpens = [this](WebSocket* ws, HttpRequest* req)
	{
		static_cast<SessionManager<uWS::WebSocket<false, true>*>*>(this)->AddSession(ws);
	};
	auto funcMessages = [this](WebSocket* ws, std::string_view message, uWS::OpCode opcode)
	{
		auto sessionid = static_cast<SessionManager<uWS::WebSocket<false, true>*>*>(this)->GetSessionIDByConnectionPt(ws);
		_msgQueueP->AddWebRequestMessage(sessionid, &message[0], message.length());
	};
	auto funcDrains = [this](WebSocket* ws)
	{

	};
	auto funcPings = [this](WebSocket* ws)
	{

	};
	auto funcPongs = [this](WebSocket* ws)
	{

	};
	auto funcCloses = [this](WebSocket* ws, int code, std::string_view message)
	{

	};

	WebSocketBehavior behavior;
	behavior.compression = uWS::DISABLED;
	behavior.maxPayloadLength = 16 * 1024;
	behavior.idleTimeout = 120;

	behavior.open = funcOpens;
	behavior.message = funcMessages;
	behavior.drain = funcDrains;
	behavior.ping = funcPings;
	behavior.pong = funcPongs;
	behavior.close = funcCloses;
	{
		std::lock_guard<std::recursive_mutex> lock(*_threadSafeMutex);
		_wsLoop = uWS::Loop::defaultLoop();
	}
	uWS::App app;
	app.ws<WebSocketDataPerson>("/*", std::move(behavior));
	app.listen(_wsPort, [](auto* token) {
		}).run();
}

void HttpsManagerPair::RecvWssDataThreadFunc()
{
	using WebSocketBehavior = typename uWS::TemplatedApp<true>::WebSocketBehavior;
	using WebSocket = uWS::WebSocket<true, true>;
	using HttpRequest = uWS::HttpRequest;
	using HttpResponse = uWS::HttpResponse<false>;

	{
		std::lock_guard<std::recursive_mutex> lock(*_threadSafeMutex);
		_wssLoop = uWS::Loop::defaultLoop();
	}

	auto funcOpen = [this](WebSocket* ws, HttpRequest* req)
	{
		static_cast<SessionManager<uWS::WebSocket<true, true>*>*>(this)->AddSession(ws);
	};
	auto funcMessage = [this](WebSocket* ws, std::string_view message, uWS::OpCode opcode)
	{
		auto sessionid = static_cast<SessionManager<uWS::WebSocket<true, true>*>*>(this)->GetSessionIDByConnectionPt(ws);
		_msgQueueP->AddWebRequestMessage(sessionid, &message[0], message.length());
	};
	auto funcDrain = [this](WebSocket* ws)
	{

	};
	auto funcPing = [this](WebSocket* ws)
	{

	};
	auto funcPong = [this](WebSocket* ws)
	{

	};
	auto funcClose = [this](WebSocket* ws, int code, std::string_view message)
	{

	};

	WebSocketBehavior behavior;
	behavior.compression = uWS::DISABLED;
	behavior.maxPayloadLength = 16 * 1024;
	behavior.idleTimeout = 120;

	behavior.open = funcOpen;
	behavior.message = funcMessage;
	behavior.drain = funcDrain;
	behavior.ping = funcPing;
	behavior.pong = funcPong;
	behavior.close = funcClose;

	uWS::SSLApp* app = nullptr;
	struct us_new_socket_context_options_t ssl_options { "server.key", "server.pem", "123456789", nullptr };
	{
		std::lock_guard<std::recursive_mutex> lock(*_threadSafeMutex);
	    app = new uWS::SSLApp(ssl_options);
	}
	app->ws<WebSocketDataPerson>("/*", std::move(behavior));
	app->listen(_wssPort, [](auto* token) {
		}).run();
}

void HttpsManagerPair::SendClientResponse()
{
	auto msgResp = _msgQueueP->GetWebMessageResp();
	while (msgResp->sessionid != INVALID_SESSION_ID)
	{
		switch (msgResp->respType)
		{
		case ResponseType::broadcast: //
		{
			auto conns = static_cast<SessionManager<uWS::WebSocket<false,true>*>*>(this)->GetAllConnections();
			for (const auto& conn : conns)
			{
				conn->send(msgResp->msg.c_str());
			}
		}break;
		case ResponseType::multicast: //
		{
			MultibroadSessionType groupid = static_cast<MultibroadSessionType>(msgResp->sessionid);
			auto conns = static_cast<SessionManager<uWS::WebSocket<false, true>*>*>(this)->GetConnectionsByGroupID(groupid);
			for (const auto& conn : conns)
			{
				conn->send(msgResp->msg.c_str());
			}
		}break;
		case ResponseType::unicast: //
		{
			auto respSession = static_cast<SessionManager<uWS::WebSocket<false, true>*>*>(this)->GetConnectionPtByUnicastSessionID(msgResp->sessionid);
			if (respSession)
			{
				respSession->send(msgResp->msg.c_str());
			}
		}break;
		default:
			break;
		}
		msgResp = _msgQueueP->GetWebMessageResp();
	}
	msgResp = _msgQueueP->GetHttpMessageResp();
	while (msgResp->sessionid != INVALID_SESSION_ID)
	{
		switch (msgResp->respType)
		{
		case ResponseType::broadcast: //
		{
			auto conns = static_cast<SessionManager<uWS::HttpResponse<false>*>*>(this)->GetAllConnections();
			for (const auto& conn : conns)
			{
				conn->end(msgResp->msg);
				static_cast<SessionManager<uWS::HttpResponse<false>*>*>(this)->RemoveSession(conn);
			}
		}break;
		case ResponseType::multicast: //
		{
			MultibroadSessionType groupid = static_cast<MultibroadSessionType>(msgResp->sessionid);
			auto conns = static_cast<SessionManager<uWS::HttpResponse<false>*>*>(this)->GetConnectionsByGroupID(groupid);
			for (const auto& conn : conns)
			{
				conn->end(msgResp->msg);
				static_cast<SessionManager<uWS::HttpResponse<false>*>*>(this)->RemoveSession(conn);
			}
		}break;
		case ResponseType::unicast: //
		{
			auto conn = static_cast<SessionManager<uWS::HttpResponse<false>*>*>(this)->GetConnectionPtByUnicastSessionID(msgResp->sessionid);
			if (conn)
			{
				conn->end(msgResp->msg);
				static_cast<SessionManager<uWS::HttpResponse<false>*>*>(this)->RemoveSession(conn);
			}
		}break;
		default:
			break;
		}
		msgResp = _msgQueueP->GetHttpMessageResp();
	}

	//https
	msgResp = _msgQueueP->GetHttpsMessageResp();
	while (msgResp->sessionid != INVALID_SESSION_ID)
	{
		switch (msgResp->respType)
		{
		case ResponseType::broadcast: //
		{
			auto conns = static_cast<SessionManager<uWS::HttpResponse<true>*>*>(this)->GetAllConnections();
			for (const auto& conn : conns)
			{
				conn->end(msgResp->msg);
				static_cast<SessionManager<uWS::HttpResponse<true>*>*>(this)->RemoveSession(conn);
			}
		}break;
		case ResponseType::multicast: //
		{
			MultibroadSessionType groupid = static_cast<MultibroadSessionType>(msgResp->sessionid);
			auto conns = static_cast<SessionManager<uWS::HttpResponse<true>*>*>(this)->GetConnectionsByGroupID(groupid);
			for (const auto& conn : conns)
			{
				conn->end(msgResp->msg);
				static_cast<SessionManager<uWS::HttpResponse<true>*>*>(this)->RemoveSession(conn);
			}
		}break;
		case ResponseType::unicast: //
		{
			auto conn = static_cast<SessionManager<uWS::HttpResponse<true>*>*>(this)->GetConnectionPtByUnicastSessionID(msgResp->sessionid);
			if (conn)
			{
				conn->end(msgResp->msg);
				static_cast<SessionManager<uWS::HttpResponse<true>*>*>(this)->RemoveSession(conn);
			}
		}break;
		default:
			break;
		}
		msgResp = _msgQueueP->GetHttpsMessageResp();
	}
}


