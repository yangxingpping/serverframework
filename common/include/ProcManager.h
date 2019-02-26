#pragma once

#include "CustomForwardDeclare.h"

#include "comm_def.h"
#include "ProcManagerInterface.h"


#include <thread>
#include <memory>
#include <vector>


class ProcManager : public std::enable_shared_from_this<ProcManager>, public ProcManagerInterface
{
public:
	ProcManager();
	virtual ~ProcManager();
	virtual void InitProcManager(WebBaseInterface* pGateWay, unsigned int cpuTCount = 4) override;

	void StartProcManager();
	
	
	void IOIntensiveProcThread();	//
	virtual void DispatchClientMessage(std::shared_ptr<SessionRequestMessage> msg, PackSourceRouteType type = PackSourceRouteType::invalid_) override;
	void DispathCpuIntensiveMessage(std::shared_ptr<SessionRequestMessage> msg);
	void DispatchIoIntensiveMessage(std::shared_ptr<SessionRequestMessage> msg);

private:
	std::vector<std::shared_ptr<CPUIntensiveThread>> _cpuThreads;
	std::shared_ptr<std::thread> _ioThread;
	unsigned int	_threadSelectIndex = 0;
	unsigned int	_threadMaxSize = 0;
	WebBaseInterface* _webGateWayInterface = nullptr;
};

