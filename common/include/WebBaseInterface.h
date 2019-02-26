#pragma once

/************************************************************************/
/* 
web
*/
/************************************************************************/
#include "comm_def.h"


class WebBaseInterface
{
public:
	virtual void NotifyClientResponse() = 0;
	virtual void PushClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len) = 0;
};