#pragma once

#include "CustomForwardDeclare.h"
#include "comm_def.h"
#include <memory>


class ProcManagerInterface
{
public:
	virtual void DispatchClientMessage(std::shared_ptr<SessionRequestMessage> msg, PackSourceRouteType type = PackSourceRouteType::invalid_) = 0;
	virtual void InitProcManager(unsigned int cpuTCount = 4) = 0;
	virtual void AddBackendUnit(CPUIntensiveThread* backendUnit) = 0;
};