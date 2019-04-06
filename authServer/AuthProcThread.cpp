#include "AuthProcThread.h"

#include "WrapperCaller.h"
#include "WebBaseInterface.h"
#include "CPUIntensiveThread.h"
#include "comm_def.h"

#include <assert.h>

#include "concurrentqueue/concurrentqueue.h"

AuthProcThread::AuthProcThread(std::shared_ptr<ProcessMessageInterface> pimpl):CPUIntensiveThread(pimpl)
{
}


AuthProcThread::~AuthProcThread()
{
}

bool AuthProcThread::MemClientRequestComeCallback()
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
		ProcJsonMessage(msg->sessionid, std::string_view{ msg->msg });
		//WrapStatCostTimeVoid(CCALL(&CPUIntensiveThread::ProcJsonMessage), this, msg->sessionid, std::string_view{ msg->msg });
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
		ProcJsonMessage(msg->sessionid, std::string_view{ msg->msg });
		//WrapStatCostTimeVoid(CCALL(&CPUIntensiveThread::ProcJsonMessage), this, msg->sessionid, std::string_view{ msg->msg });
		if (msg->pSource)
		{
			msg->pSource->PushClientResponse(ResponseType::unicast, msg->sessionid, str.c_str(), str.length());
			msg->pSource->NotifyClientResponse();
		}
		else {}
		bHasMsg = _recvHttpJsonMessages->try_dequeue(msg);
	}

	bHasMsg = _recvHttpsJsonMessages->try_dequeue(msg);
	while (bHasMsg) //has json message
	{
		std::string str{ "hello.world" };
		ProcJsonMessage(msg->sessionid, std::string_view{ msg->msg });
		//WrapStatCostTimeVoid(CCALL(&CPUIntensiveThread::ProcJsonMessage), this, msg->sessionid, std::string_view{ msg->msg });
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
		ProcPBMessage(msg->sessionid, std::string_view{ msg->msg });
		//WrapStatCostTimeVoid(CCALL(&CPUIntensiveThread::ProcPBMessage), this, msg->sessionid, std::string_view{ msg->msg });
		if (msg->pSource)
		{
			msg->pSource->PushClientResponse(ResponseType::unicast, msg->sessionid, str.c_str(), str.length());
			msg->pSource->NotifyClientResponse();
		}
		else
		{
		}
		bHasMsg = _recvPBMessages->try_dequeue(msg);
	}

	return true;
}
