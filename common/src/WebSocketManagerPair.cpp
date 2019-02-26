
#include "ThirdPartyuWS.h"
#include "libusockets_new.h"

#include "spdlog/spdlog.h"
#include "SessionManager.h"
#include "WebSocketManagerPair.h"
#include "WebBaseInterface.h"
#include "MessageQueueManager.h"
#include "DBManager.h"

#include "comm_def.h"

#include <functional>


WebSocketManagerPair* WebSocketManagerPair::_sDefaultWsPair = nullptr;


WebSocketManagerPair* WebSocketManagerPair::defaultWebSocketPair()
{
	if (_sDefaultWsPair)
	{
		return _sDefaultWsPair;
	}
	else
	{
		assert(false);
		return nullptr;
	}
}




void WebSocketManagerPair::NotifyClientResponse()
{
	_wsLoop->defer([this]() {
		_sDefaultWsPair->SendClientResponse();
		});
}



void WebSocketManagerPair::PushClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len)
{
	if (sid == INVALID_SESSION_ID || !msg)
	{ //record some error message
		return;
	}
	_msgQueueP->AddWebResponseMessage(sid, msg, len, type);
}

WebSocketManagerPair::WebSocketManagerPair()
{
	_threadSafeMutex = std::make_shared<std::recursive_mutex>();
	_wsDbManager = std::make_shared<DBManager>();
	_wsDbManager->startDBManager();
}

bool WebSocketManagerPair::InitWsManagerPair(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue)
{
	bool bret = false;
	_wsIP = ip;
	_wsPort = port;
	_msgQueueP = msgqueue;
	_logger = spdlog::get(LOGGER_NAME);
	_recvWsThread = std::make_shared<std::thread>(&WebSocketManagerPair::RecvWsDataThreadFunc, this);
	if (_logger)
	{
		bret = true;
	}
	_sDefaultWsPair = this;
	return bret;
}

void WebSocketManagerPair::StartWebManagerPair()
{
	if (_recvWsThread && _recvWsThread->joinable())
	{
		_recvWsThread->join();
	}
}

WebSocketManagerPair::~WebSocketManagerPair()
{
}

void WebSocketManagerPair::RecvWsDataThreadFunc()
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

void WebSocketManagerPair::SendClientResponse()
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
}


