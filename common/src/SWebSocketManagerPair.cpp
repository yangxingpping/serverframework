
#include "ThirdPartyuWS.h"
#include "libusockets_new.h"

#include "spdlog/spdlog.h"
#include "SessionManager.h"
#include "SWebSocketManagerPair.h"
#include "WebBaseInterface.h"
#include "MessageQueueManager.h"
#include "DBManager.h"

#include "comm_def.h"

#include <functional>

SWebSocketManagerPair* SWebSocketManagerPair::defaultSWebSocketPair()
{
	if (_sDefaultWssPair)
	{
		return _sDefaultWssPair;
	}
	else
	{
		assert(false);
		return nullptr;
	}
}

SWebSocketManagerPair* SWebSocketManagerPair::_sDefaultWssPair = nullptr;






void SWebSocketManagerPair::NotifyClientResponse()
{
	_wsLoop->defer([this]() {
		_sDefaultWssPair->SendClientResponse();
		});
}



void SWebSocketManagerPair::PushClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len)
{
	if (sid == INVALID_SESSION_ID || !msg)
	{ //record some error message
		return;
	}
	_msgQueueP->AddWebResponseMessage(sid, msg, len, type);
}

bool SWebSocketManagerPair::InitWebManager(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue)
{
	bool bret = false;
	_wssIP = ip;
	_wssPort = port;
	_msgQueueP = msgqueue;
	_logger = spdlog::get(LOGGER_NAME);
	_recvWssThread = std::make_shared<std::thread>(&SWebSocketManagerPair::RecvWssDataThreadFunc, this);
	if (_logger)
	{
		bret = true;
	}
	_sDefaultWssPair = this;
	return bret;
}

SWebSocketManagerPair::SWebSocketManagerPair()
{
	_threadSafeMutex = std::make_shared<std::recursive_mutex>();
	_wsDbManager = std::make_shared<DBManager>();
	_wsDbManager->startDBManager();
}

void SWebSocketManagerPair::StartWebManagerPair()
{
	if (_recvWssThread && _recvWssThread->joinable())
	{
		_recvWssThread->join();
	}
}

SWebSocketManagerPair::~SWebSocketManagerPair()
{
}

void SWebSocketManagerPair::RecvWssDataThreadFunc()
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

void SWebSocketManagerPair::SendClientResponse()
{
	auto msgResp = _msgQueueP->GetSWebMessageResp();
	while (msgResp->sessionid != INVALID_SESSION_ID)
	{
		switch (msgResp->respType)
		{
		case ResponseType::broadcast: //
		{
			auto conns = static_cast<SessionManager<uWS::WebSocket<true,true>*>*>(this)->GetAllConnections();
			for (const auto& conn : conns)
			{
				conn->send(msgResp->msg.c_str());
			}
		}break;
		case ResponseType::multicast: //
		{
			MultibroadSessionType groupid = static_cast<MultibroadSessionType>(msgResp->sessionid);
			auto conns = static_cast<SessionManager<uWS::WebSocket<true, true>*>*>(this)->GetConnectionsByGroupID(groupid);
			for (const auto& conn : conns)
			{
				conn->send(msgResp->msg.c_str());
			}
		}break;
		case ResponseType::unicast: //
		{
			auto respSession = static_cast<SessionManager<uWS::WebSocket<true, true>*>*>(this)->GetConnectionPtByUnicastSessionID(msgResp->sessionid);
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
}


