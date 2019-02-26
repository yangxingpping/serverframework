
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

void HttpsManagerPair::PushClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len)
{
	if (sid == INVALID_SESSION_ID || !msg)
	{ //record some error message
		return;
	}
	_msgQueueP->AddWebResponseMessage(sid, msg, len, type);
}

bool HttpsManagerPair::InitWebManager(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue)
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

HttpsManagerPair::HttpsManagerPair()
{
	_threadSafeMutex = std::make_shared<std::recursive_mutex>();
	_wsDbManager = std::make_shared<DBManager>();
	_wsDbManager->startDBManager();
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


void HttpsManagerPair::SendClientResponse()
{
	auto msgResp = _msgQueueP->GetHttpsMessageResp();
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


