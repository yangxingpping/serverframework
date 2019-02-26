#pragma once

/************************************************************************/
/* 
web
*/
/************************************************************************/
#include "comm_def.h"

#include <string>
#include <memory>

class WebBaseInterface
{
public:
	virtual void NotifyClientResponse() = 0;
	virtual void PushClientResponse(ResponseType type, SessionType sid, const char* msg, size_t len) = 0;
	virtual bool InitWebManager(std::string ip, uint16_t port, std::shared_ptr<MessageQueueManager> msgqueue) = 0;
};