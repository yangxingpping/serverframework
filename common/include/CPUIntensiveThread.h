#pragma once

/************************************************************************/
/* 

*/
/************************************************************************/

#include "CustomForwardDeclare.h"
#include "PrimitiveDef.h"
#include "share_def.h"

#include <thread>
#include <memory>
#include <string_view>

class CPUIntensiveThread
{
public:
	static void SClientRequestComeCallback(uv_async_t* handle);
public:
	CPUIntensiveThread(WebBaseInterface* pgw, std::shared_ptr<ProcessMessageInterface> pimpl);
	~CPUIntensiveThread();
	void WaitJoin();
	void CpuIntensiveProcThread();	
	void ProcJsonMessage(SessionType sid, std::string_view strmsg);	
	void ProcPBMessage(SessionType sid, std::string_view strmsg);	

	void PushJsonRequestMessage(std::shared_ptr<SessionRequestMessageBody> msg);
	void PushPBRequestMessage(std::shared_ptr<SessionRequestMessageBody> msg);

	void PushHttpRequestMessage(std::shared_ptr<SessionRequestMessageBody> msg, EPackType type);
	void PushWebSocketRequestMessage(std::shared_ptr<SessionRequestMessageBody> msg, EPackType type);

	void PushHttpsRequestMessage(std::shared_ptr<SessionRequestMessageBody> msg, EPackType type);

	void MemClientRequestComeCallback();

private:
	std::shared_ptr<std::thread> _cpuThreads;
	std::shared_ptr<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>> _recvJsonMessages;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>> _recvPBMessages;

	std::shared_ptr<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>> _recvHttpJsonMessages;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>> _recvHttpPBMessages;

	std::shared_ptr<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>> _recvHttpsJsonMessages;
	std::shared_ptr < moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>> _recvHttpsPBMessages;

	std::shared_ptr<ProcessMessageInterface> _procMessageImpl;

	WebBaseInterface* _webGateWay = nullptr;
	uv_async_t* _asyncClientRequest;
	bool _run = false;
	std::shared_ptr<DBManager> _dbConnection;
};

