#pragma once

/************************************************************************/

/************************************************************************/

#include "CustomForwardDeclare.h"

#include "comm_def.h"
#include "ProcManagerInterface.h"


#include <memory>
#include <thread>

class MessageQueueManager
{
public:
	MessageQueueManager();
	~MessageQueueManager();
	void InitMessageQueueManager(std::shared_ptr<ProcManagerInterface> proc);

	void AddWebRequestMessage(SessionType sessionid, const char* msg, size_t len);
	void AddWebResponseMessage(SessionType sessionid, const char* msg, size_t len, ResponseType broadType);

	void AddsWebRequestMessage(SessionType sessionid, const char* msg, size_t len);
	void AddsWebResponseMessage(SessionType sessionid, const char* msg, size_t len, ResponseType broadType);

	void AddHttpRequestMessage(SessionType sessionid, const char* msg, size_t len);
	void AddHttpResponseMessage(SessionType sessionid, const char* msg, size_t len, ResponseType broadType = ResponseType::unicast);

	void AddHttpsRequestMessage(SessionType sessionid, const char* msg, size_t len);
	void AddHttpsResponseMessage(SessionType sessionid, const char* msg, size_t len, ResponseType broadType = ResponseType::unicast);


	std::shared_ptr<SessionResponseMessage> GetWebMessageResp();
	std::shared_ptr<SessionResponseMessage> GetSWebMessageResp();
	std::shared_ptr<SessionResponseMessage> GetHttpMessageResp();
	std::shared_ptr<SessionResponseMessage> GetHttpsMessageResp();
	void AddSocketMessage(SessionType sessionid, char* msg, size_t len);

	void DispathMessageThreadFunc();
	void MemAsyncRecvDataCallback();

private:
	std::shared_ptr<std::thread> _dispathcThread;

	std::shared_ptr<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _sockRecvQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _sockSendQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _swebRecvQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _swebSendQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _webRecvQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _webSendQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _httpRecvQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _httpSendQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _httpsRecvQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _httpsSendQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _othersRecvQueue;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionResponseMessage>, moodycamel::ConcurrentQueueDefaultTraits>> _othersendQueue;

	std::shared_ptr<ProcManagerInterface> _procPt;
	uv_async_t* _asyncClientConneResponse;
	uv_loop_t* _loop;
	
	bool _run = false;
};

