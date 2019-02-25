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
	virtual void NotifyHttpResponse() = 0;
	virtual void NotifyHttpsResponse() = 0;
	virtual void NotifyWsResponse() = 0;
	virtual void NotifyWssResponse() = 0;
	virtual void PushWebSocketClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len) = 0; //
	virtual void PushHttpClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len) = 0;
	virtual void PushHttpsClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len) = 0;
};