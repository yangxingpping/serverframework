#pragma once

/************************************************************************/

/************************************************************************/

#define LOGGER_NAME		"filelog"	//global logger name
#define LOG_FILE_NAME	"a.log"		//

#include "CustomForwardDeclare.h"
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
		PackSourceHostType type, WebBaseInterface* pfront):sessionid(sid),hostType(type), pSource(pfront)
	{
		msg = m;
	}
public:
	SessionType sessionid;
	PackSourceHostType hostType;
	std::string msg;
	WebBaseInterface* pSource = nullptr;
};

class SessionRequestMessageBody
{
public:
	SessionRequestMessageBody(SessionType sid, std::string m, WebBaseInterface* pfront):sessionid(sid), pSource(pfront)
	{
		msg = m;
	}
	SessionType sessionid;
	std::string msg;
	WebBaseInterface* pSource = nullptr;
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
	SessionResponseMessage(ResponseType type, SessionType sid, std::string msg) :SessionMessage(sid, msg, PackSourceHostType::client,nullptr)
	{
		respType = type;
	}
public:
	ResponseType respType;
};


struct WebSocketDataPerson
{

};

struct SqlDBConfig
{
	std::string		_user;
	std::string		_pass;
	std::string		_odbc;
};

struct RedisConfig
{
	std::string		_ip;
	uint16_t		_port;
	std::string		_pass;
};