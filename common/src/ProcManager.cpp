#include "CPUIntensiveThread.h"
#include "WebBaseInterface.h"

#include "ProcManager.h"
#include "ProcessMessageImpl.h"

#include <algorithm>
#include <cstring>

ProcManager::ProcManager()
{
}


ProcManager::~ProcManager()
{
}

void ProcManager::InitProcManager(WebBaseInterface* pGateWay, unsigned int cpuTCount /*= 4*/)
{
	//_webGateWayInterface = pGateWay;
	for (unsigned int i = 0; i < cpuTCount; ++i)
	{
		_cpuThreads.push_back(std::make_shared<CPUIntensiveThread>(pGateWay, std::make_shared<ProcessMessageImpl>()));
	}
	_threadMaxSize = static_cast<unsigned int>(_cpuThreads.size());
	_ioThread = std::make_shared<std::thread>(&ProcManager::IOIntensiveProcThread, this);
}

void ProcManager::StartProcManager()
{
	for (auto& onecpu : _cpuThreads)
	{
		onecpu->WaitJoin();
	}
	_ioThread->join();
}

void ProcManager::IOIntensiveProcThread()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		int i = 0;
	}
}

void ProcManager::DispatchClientMessage(std::shared_ptr<SessionRequestMessage> msg, PackSourceRouteType type /*= PackSourceRouteType::invalid_*/)
{
	char chEPackType = msg->msg[0];
	EPackType packType = static_cast<EPackType>(chEPackType-'0');
	std::shared_ptr<SessionRequestMessageBody> body = std::shared_ptr<SessionRequestMessageBody>(
		new SessionRequestMessageBody(msg->sessionid, { &(msg->msg)[1], msg->msg.length() - 1 }));
	auto _tempIndex = (_threadSelectIndex++) % _threadMaxSize;
	switch (type)
	{
	case PackSourceRouteType::invalid_:
		break;
	case PackSourceRouteType::http:
	{
		_cpuThreads[_tempIndex]->PushHttpRequestMessage(body, packType);
	}break;
	case PackSourceRouteType::https:
	{
		_cpuThreads[_tempIndex]->PushHttpsRequestMessage(body, packType);
	}break;
	case PackSourceRouteType::websocket:
	{
		_cpuThreads[_tempIndex]->PushWebSocketRequestMessage(body, packType);
	}break;
	case PackSourceRouteType::tcpsocket:
	{
	}break;
	default:
		break;
	}
}

void ProcManager::DispathCpuIntensiveMessage(std::shared_ptr<SessionRequestMessage> msg)
{

}

void ProcManager::DispatchIoIntensiveMessage(std::shared_ptr<SessionRequestMessage> msg)
{

}

