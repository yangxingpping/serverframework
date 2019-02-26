
#include "concurrentqueue.h"

#include "MessageQueueManager.h"
#include "uv.h"
#include "SessionManager.h"

MessageQueueManager::MessageQueueManager()
{
	_webRecvQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>>>();
	_webSendQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>>>();
	_sockRecvQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>>>();
	_sockSendQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>>>();
	_httpRecvQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>>>();
	_httpSendQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>>>();
	_httpsRecvQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>>>();
	_httpsSendQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>>>();
	_swebRecvQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>>>();
	_swebSendQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>>>();
}


MessageQueueManager::~MessageQueueManager()
{
}

void MessageQueueManager::InitMessageQueueManager(std::shared_ptr<ProcManagerInterface> proc)
{
	_procPt = proc;
	_dispathcThread = std::make_shared<std::thread>(&MessageQueueManager::DispathMessageThreadFunc, this);
}

void MessageQueueManager::AddWebRequestMessage(SessionType sessionid, const char* msg, size_t len, WebBaseInterface* pfront)
{
	auto msg2 = std::shared_ptr<SessionRequestMessage>(new SessionRequestMessage{sessionid, {msg, len}, PackSourceHostType::client, pfront });
	_webRecvQueue->enqueue(msg2);
	if (_run)
	{
		uv_async_send(_asyncClientConneResponse);
	}
}

void MessageQueueManager::AddWebResponseMessage(SessionType sessionid, const char* msg, size_t len, ResponseType broadType)
{
	auto msg2 = std::shared_ptr<SessionResponseMessage>(new SessionResponseMessage({ ResponseType::unicast ,sessionid, {msg, len} }));
	_webSendQueue->enqueue(msg2);
}


void MessageQueueManager::AddsWebRequestMessage(SessionType sessionid, const char* msg, size_t len, WebBaseInterface* pfront)
{
	auto msg2 = std::shared_ptr<SessionRequestMessage>(new SessionRequestMessage{ sessionid, {msg, len}, PackSourceHostType::client, pfront });
	_swebRecvQueue->enqueue(msg2);
	if (_run)
	{
		uv_async_send(_asyncClientConneResponse);
	}
}

void MessageQueueManager::AddsWebResponseMessage(SessionType sessionid, const char* msg, size_t len, ResponseType broadType)
{
	auto msg2 = std::shared_ptr<SessionResponseMessage>(new SessionResponseMessage({ ResponseType::unicast ,sessionid, {msg, len} }));
	_swebSendQueue->enqueue(msg2);
}

void MessageQueueManager::AddHttpRequestMessage(SessionType sessionid, const char* msg, size_t len, WebBaseInterface* pfront)
{
	auto msg2 = std::shared_ptr<SessionRequestMessage>(new SessionRequestMessage{ sessionid, {msg, len}, PackSourceHostType::client, pfront});
	_httpRecvQueue->enqueue(msg2);
	if (_run)
	{
		uv_async_send(_asyncClientConneResponse);
	}
}


void MessageQueueManager::AddHttpResponseMessage(SessionType sessionid, const char* msg, size_t len, ResponseType broadType /*= ResponseType::unicast*/)
{
	auto msg2 = std::shared_ptr<SessionResponseMessage>(new SessionResponseMessage({ ResponseType::unicast ,sessionid, {msg, len} }));
	_httpSendQueue->enqueue(msg2);
}

void MessageQueueManager::AddHttpsRequestMessage(SessionType sessionid, const char* msg, size_t len, WebBaseInterface* pfront)
{
	auto msg2 = std::shared_ptr<SessionRequestMessage>(new SessionRequestMessage{ sessionid, {msg, len}, PackSourceHostType::client, pfront });
	_httpsRecvQueue->enqueue(msg2);
	if (_run)
	{
		uv_async_send(_asyncClientConneResponse);
	}
}

void MessageQueueManager::AddHttpsResponseMessage(SessionType sessionid, const char* msg, size_t len, ResponseType broadType /*= ResponseType::unicast*/)
{
	auto msg2 = std::shared_ptr<SessionResponseMessage>(new SessionResponseMessage({ ResponseType::unicast ,sessionid, {msg, len} }));
	_httpsSendQueue->enqueue(msg2);
}

std::shared_ptr<SessionResponseMessage> MessageQueueManager::GetWebMessageResp()
{
	std::shared_ptr<SessionResponseMessage> msgRet = std::shared_ptr<SessionResponseMessage>(new SessionResponseMessage({ ResponseType::unicast, INVALID_SESSION_ID, {""} }));
	_webSendQueue->try_dequeue(msgRet);
	return msgRet;
}


std::shared_ptr<SessionResponseMessage> MessageQueueManager::GetSWebMessageResp()
{
	std::shared_ptr<SessionResponseMessage> msgRet = std::shared_ptr<SessionResponseMessage>(new SessionResponseMessage({ ResponseType::unicast, INVALID_SESSION_ID, {""} }));
	_swebSendQueue->try_dequeue(msgRet);
	return msgRet;
}

std::shared_ptr<SessionResponseMessage> MessageQueueManager::GetHttpMessageResp()
{
	std::shared_ptr<SessionResponseMessage> msgRet = std::shared_ptr<SessionResponseMessage>(new SessionResponseMessage({ ResponseType::unicast, INVALID_SESSION_ID, {""} }));
	_httpSendQueue->try_dequeue(msgRet);
	return msgRet;
}

std::shared_ptr<SessionResponseMessage> MessageQueueManager::GetHttpsMessageResp()
{
	std::shared_ptr<SessionResponseMessage> msgRet = std::shared_ptr<SessionResponseMessage>(new SessionResponseMessage({ ResponseType::unicast, INVALID_SESSION_ID, {""} }));
	_httpsSendQueue->try_dequeue(msgRet);
	return msgRet;
}

void MessageQueueManager::AddSocketMessage(SessionType sessionid, char* msg, size_t len)
{

}

void MessageQueueManager::DispathMessageThreadFunc()
{
	uv_loop_t*_loop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
	uv_loop_init(_loop);
	_asyncClientConneResponse = (uv_async_t*)malloc(sizeof(uv_async_t));
	_asyncClientConneResponse->data = this;
	auto _sasyncCallback = [](uv_async_t* handle) 
	{
		MessageQueueManager* mq = static_cast<MessageQueueManager*>(handle->data);
		mq->MemAsyncRecvDataCallback();
	};
	uv_async_init(_loop, _asyncClientConneResponse, _sasyncCallback);
	_run = true;
	uv_run(_loop, UV_RUN_DEFAULT);
	
	_run = false;
	
}

void MessageQueueManager::MemAsyncRecvDataCallback()
{
	
	size_t maxSize{ 10 };
	std::vector<std::shared_ptr<SessionRequestMessage>> vecMsgs;
	vecMsgs.reserve(maxSize);
	auto webmsgsize = _webRecvQueue->try_dequeue_bulk(std::back_inserter(vecMsgs), maxSize);
	while (webmsgsize > 0)
	{
		for (auto& msg : vecMsgs)
		{
			_procPt->DispatchClientMessage(msg, PackSourceRouteType::websocket);
		}
		vecMsgs.clear();
		webmsgsize = _webRecvQueue->try_dequeue_bulk(std::back_inserter(vecMsgs), maxSize);
	}

	vecMsgs.clear();
	auto httpmsgsize = _httpRecvQueue->try_dequeue_bulk(std::back_inserter(vecMsgs), maxSize);
	while (httpmsgsize > 0)
	{
		for (auto& msg : vecMsgs)
		{
			_procPt->DispatchClientMessage(msg,  PackSourceRouteType::http);
		}
		vecMsgs.clear();
		httpmsgsize = _httpRecvQueue->try_dequeue_bulk(std::back_inserter(vecMsgs), maxSize);
	}

	vecMsgs.clear();
	auto httpsmsgsize = _httpsRecvQueue->try_dequeue_bulk(std::back_inserter(vecMsgs), maxSize);
	while (httpsmsgsize > 0)
	{
		for (auto& msg : vecMsgs)
		{
			_procPt->DispatchClientMessage(msg, PackSourceRouteType::https);
		}
		vecMsgs.clear();
		httpsmsgsize = _httpsRecvQueue->try_dequeue_bulk(std::back_inserter(vecMsgs), maxSize);
	}

	vecMsgs.clear();
	auto sockmsgsize = _sockRecvQueue->try_dequeue_bulk(std::back_inserter(vecMsgs), maxSize);
	while (webmsgsize > 0)
	{
		for (auto& msg : vecMsgs)
		{
			_procPt->DispatchClientMessage(msg, PackSourceRouteType::tcpsocket);
		}
		vecMsgs.clear();
		webmsgsize = _sockRecvQueue->try_dequeue_bulk(std::back_inserter(vecMsgs), maxSize);
	}

}
