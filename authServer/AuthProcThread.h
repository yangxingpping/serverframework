#pragma once

#include "CustomForwardDeclare.h"
#include "CPUIntensiveThread.h"

#include <memory>

class AuthProcThread :
	public CPUIntensiveThread
{
public:
	AuthProcThread(std::shared_ptr<ProcessMessageInterface> pimpl);
	virtual ~AuthProcThread();

	virtual bool MemClientRequestComeCallback() override;
};

