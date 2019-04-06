
#include "uv.h"

#include "concurrentqueue/concurrentqueue.h"
#include "comm_def.h"
#include "CPUIntensiveThread.h"
#include "WebBaseInterface.h"
#include "otlv4_h2/otlv4.h"
#include "CustomMacro.h"
#include "WrapperCaller.h"
#include "ProcessMessageInterface.h"
#include "DBManager.h"

void CPUIntensiveThread::SClientRequestComeCallback(uv_async_t* handle)
{
	if (handle)
	{
		CPUIntensiveThread* px = (CPUIntensiveThread*)(handle->data);
		if (px)
		{
			px->MemClientRequestComeCallback();
		}
	}
}

CPUIntensiveThread::CPUIntensiveThread(std::shared_ptr<ProcessMessageInterface> pimpl)
	
{
	_procMessageImpl = pimpl;
	_dbConnection = std::make_shared<DBManager>();
	_cpuThreads = std::make_shared<std::thread>(&CPUIntensiveThread::CpuIntensiveProcThread, this);
	_recvJsonMessages = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>>();
	_recvPBMessages = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>>();
	_recvHttpJsonMessages = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>>();
	_recvHttpPBMessages = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>>();
	_recvHttpsJsonMessages = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>>();
	_recvHttpsPBMessages = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<SessionRequestMessageBody>, moodycamel::ConcurrentQueueDefaultTraits>>();
	bool bFound = false;
}


CPUIntensiveThread::~CPUIntensiveThread()
{
}

void CPUIntensiveThread::WaitJoin()
{
	if (_cpuThreads->joinable())
	{
		_cpuThreads->join();
	}
}

void CPUIntensiveThread::CpuIntensiveProcThread()
{
	uv_loop_t loop;
	_dbConnection->startDBManager();
	uv_loop_init(&loop);
	_asyncClientRequest = new uv_async_t();
	_asyncClientRequest->data = this;
	uv_async_init(&loop, _asyncClientRequest, &CPUIntensiveThread::SClientRequestComeCallback);
	_run = true;
	uv_run(&loop, UV_RUN_DEFAULT);
	uv_loop_close(&loop);
}

void CPUIntensiveThread::ProcJsonMessage(SessionType sid, std::string_view strmsg)
{
	_procMessageImpl->ProcJsonMessage(sid, strmsg);
}

void CPUIntensiveThread::ProcPBMessage(SessionType sid, std::string_view strmsg)
{
	_procMessageImpl->ProcPbMessage(sid, strmsg);
}

void CPUIntensiveThread::PushJsonRequestMessage(std::shared_ptr<SessionRequestMessageBody> msg)
{
	_recvJsonMessages->enqueue(msg);
	if (_run)
	{
		uv_async_send(_asyncClientRequest);
	}
}

void CPUIntensiveThread::PushPBRequestMessage(std::shared_ptr<SessionRequestMessageBody> msg)
{
	_recvPBMessages->enqueue(msg);
	if (_run)
	{
		uv_async_send(_asyncClientRequest);
	}
}


void CPUIntensiveThread::PushHttpRequestMessage(std::shared_ptr<SessionRequestMessageBody> msg, EPackType type)
{
	switch (type)
	{
	case EPackType::JSONPACK:
	{
		_recvHttpJsonMessages->enqueue(msg);
		if (_run)
		{
			uv_async_send(_asyncClientRequest);
		}
	}break;
	case EPackType::PBPACK:
	{
		_recvHttpPBMessages->enqueue(msg);
		if (_run)
		{
			uv_async_send(_asyncClientRequest);
		}
	}break;
	default:
		break;
	}
}


void CPUIntensiveThread::PushWebSocketRequestMessage(std::shared_ptr<SessionRequestMessageBody> msg, EPackType type)
{
	switch (type)
	{
	case EPackType::JSONPACK:
	{
		_recvJsonMessages->enqueue(msg);
		if (_run)
		{
			uv_async_send(_asyncClientRequest);
		}
	}break;
	case EPackType::PBPACK:
	{
		_recvPBMessages->enqueue(msg);
		if (_run)
		{
			uv_async_send(_asyncClientRequest);
		}
	}break;
	default:
		break;
	}
}

void CPUIntensiveThread::PushHttpsRequestMessage(std::shared_ptr<SessionRequestMessageBody> msg, EPackType type)
{
	switch (type)
	{
	case EPackType::JSONPACK:
	{
		_recvHttpsJsonMessages->enqueue(msg);
		if (_run)
		{
			uv_async_send(_asyncClientRequest);
		}
	}break;
	case EPackType::PBPACK:
	{
		_recvHttpsPBMessages->enqueue(msg);
		if (_run)
		{
			uv_async_send(_asyncClientRequest);
		}
	}break;
	default:
		break;
	}
}

bool CPUIntensiveThread::MemClientRequestComeCallback()
{
	static_assert(std::is_integral<MainCmdType>::value, "main command must be integral");
	static_assert(std::is_integral<AssCMDType>::value, "assist command must be integral");
	assert(sizeof(MainCmdType) % 4 == 0);
	assert(sizeof(AssCMDType) % 4 == 0);

	std::shared_ptr<SessionRequestMessageBody> msg;
	bool bHasMsg = _recvJsonMessages->try_dequeue(msg);

	while (bHasMsg) //has json message
	{
		std::string str{ "hello.world" };
		WrapStatCostTimeVoid(CCALL(&CPUIntensiveThread::ProcJsonMessage), this, msg->sessionid, std::string_view{ msg->msg });
		if (msg->pSource)
		{
			msg->pSource->PushClientResponse(ResponseType::unicast, msg->sessionid, str.c_str(), str.length());
			msg->pSource->NotifyClientResponse();
		}
		else {}
		bHasMsg = _recvJsonMessages->try_dequeue(msg);
	}

	bHasMsg = _recvHttpJsonMessages->try_dequeue(msg);
	while (bHasMsg) //has json message
	{
		std::string str{ "hello.world" };
		WrapStatCostTimeVoid(CCALL(&CPUIntensiveThread::ProcJsonMessage), this, msg->sessionid, std::string_view{ msg->msg });
		if (msg->pSource)
		{
			msg->pSource->PushClientResponse(ResponseType::unicast, msg->sessionid, str.c_str(), str.length());
			msg->pSource->NotifyClientResponse();
		}
		else{}
		bHasMsg = _recvHttpJsonMessages->try_dequeue(msg);
	}

	bHasMsg = _recvHttpsJsonMessages->try_dequeue(msg);
	while (bHasMsg) //has json message
	{
		std::string str{ "hello.world" };
		WrapStatCostTimeVoid(CCALL(&CPUIntensiveThread::ProcJsonMessage), this, msg->sessionid, std::string_view{ msg->msg });
		if (msg->pSource)
		{
			msg->pSource->PushClientResponse(ResponseType::unicast, msg->sessionid, str.c_str(), str.length());
			msg->pSource->NotifyClientResponse();
		}
		else //something was wrong
		{

		}
		bHasMsg = _recvHttpsJsonMessages->try_dequeue(msg);
	}

	bHasMsg = _recvPBMessages->try_dequeue(msg);
	while (bHasMsg) //has protobuf message
	{
		std::string str{ "world.hello" };
		WrapStatCostTimeVoid(CCALL(&CPUIntensiveThread::ProcPBMessage), this, msg->sessionid ,std::string_view{ msg->msg });
		if (msg->pSource)
		{
			msg->pSource->PushClientResponse(ResponseType::unicast, msg->sessionid, str.c_str(), str.length());
			msg->pSource->NotifyClientResponse();
		}
		else
		{ }
		bHasMsg = _recvPBMessages->try_dequeue(msg);
	}

	return true;
}
