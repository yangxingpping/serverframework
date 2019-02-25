#pragma once

#include "comm_def.h"
#include <memory>


class ProcManagerInterface
{
public:
	virtual void DispatchClientMessage(std::shared_ptr<SessionRequestMessage> msg, PackSourceRouteType type = PackSourceRouteType::invalid_) = 0;
};