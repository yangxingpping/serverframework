#pragma once

#include "PrimitiveDef.h"

#include <string_view>

class ProcessMessageInterface
{
public:
	ProcessMessageInterface() {};
	virtual ~ProcessMessageInterface() {};
	virtual void ProcJsonMessage(SessionType sid, std::string_view msg) = 0;
	virtual void ProcPbMessage(SessionType sid, std::string_view msg) = 0;
};