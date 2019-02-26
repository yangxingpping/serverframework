
#include "ThirdPartyuWS.h"
#include "libusockets_new.h"

#include "spdlog/spdlog.h"
#include "SessionManager.h"
#include "HttpManagerPair.h"
#include "WebBaseInterface.h"
#include "MessageQueueManager.h"
#include "DBManager.h"

#include "comm_def.h"

#include <functional>

HttpManagerPair* HttpManagerPair::_sDefaultHttpPair = nullptr;

HttpManagerPair* HttpManagerPair::defaultHttpPair()
{
	if (_sDefaultHttpPair)
	{
		return _sDefaultHttpPair;
	}
	else
	{
		assert(false);
		return nullptr;
	}
}



void HttpManagerPair::NotifyClientResponse()
{
	_httpLoop->defer([this]() {
		_sDefaultHttpPair->SendClientResponse();
		});
}


void HttpManagerPair::PushClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len)
{
	if (sid == INVALID_SESSION_ID || !msg)
	{ //record some error message
		return;
	}
	_msgQueueP->AddWebResponseMessage(sid, msg, len, type);
}


bool HttpManagerPair::InitWebManager(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue)
{
	bool bret = false;

	_httpIP = ip;
	_httpPort = port;
	_msgQueueP = msgqueue;
	_logger = spdlog::get(LOGGER_NAME);
	_recvHttpThread = std::make_shared<std::thread>(&HttpManagerPair::RecvHttpDataThreadFunc, this);
	if (_logger)
	{
		bret = true;
	}
	_sDefaultHttpPair = this;
	return bret;
}

HttpManagerPair::HttpManagerPair()
{
	_threadSafeMutex = std::make_shared<std::recursive_mutex>();
	_wsDbManager = std::make_shared<DBManager>();
	_wsDbManager->startDBManager();
}


void HttpManagerPair::StartWebManagerPair()
{
	
	if (_recvHttpThread && _recvHttpThread->joinable())
	{
		_recvHttpThread->join();
	}
}

HttpManagerPair::~HttpManagerPair()
{
}

void HttpManagerPair::RecvHttpDataThreadFunc()
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
				_msgQueueP->AddHttpRequestMessage(sessionid, query.data() + 1, query.length() - 1, this);
			}
		});
	
	app.listen(_httpPort, [](auto* token) {
		});
	app.run();

}


void HttpManagerPair::SendClientResponse()
{
	auto msgResp = _msgQueueP->GetHttpMessageResp();
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
}


