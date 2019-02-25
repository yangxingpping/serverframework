#pragma once

/************************************************************************/

/************************************************************************/

#define LOGGER_NAME		"filelog"	//global logger name
#define LOG_FILE_NAME	"a.log"		//




#include "share_def.h"

#include <string>

enum class PackSourceHostType
{
	client,	//this package is from client
	server,	//this package is from other servers
};

enum class PackSourceRouteType
{
	invalid_,
	http,
	https,
	websocket,
	tcpsocket,
};

class SessionMessage
{
public:
	SessionMessage(SessionType sid, std::string m,
		PackSourceHostType type=PackSourceHostType::client):sessionid(sid),hostType(type)
	{
		msg = m;
	}
public:
	SessionType sessionid;
	PackSourceHostType hostType;
	std::string msg;
};

class SessionRequestMessageBody
{
public:
	SessionRequestMessageBody(SessionType sid, std::string m):sessionid(sid)
	{
		msg = m;
	}
	SessionType sessionid;
	std::string msg;
};

enum class ResponseType
{
	unicast,
	multicast,
	broadcast,
};

using SessionRequestMessage = SessionMessage;


class SessionResponseMessage : public SessionMessage
{
public:
	SessionResponseMessage(ResponseType type, SessionType sid, std::string msg) :SessionMessage(sid, msg)
	{
		respType = type;
	}
public:
	ResponseType respType;
};


struct WebSocketDataPerson
{

};