#include "ProcessMessageImpl.h"
#include "PackUtils.h"
#include "ThirdPartyuWS.h"
#include "share_def.h"
#include "query.pb.h"
#include "PBMessageCallBacks.h"
#include "PBMsgCallBackDispatcher.h"
#include "Utils.h"
#include "JsonStructs.h"

#include <cassert>


ProcessMessageImpl::ProcessMessageImpl()
{
	_pBDispatcher = std::make_shared<PBMsgCallBackDispatcher>();
	_pBDispatcher->RegisterMessageCallback<muduo::Query>(&PBMessageCallBacks::onQuery);
	_mainProcMap.insert({ MainClassify::UndefinedMainCmdL, &ProcessMessageImpl::_ProcMainNodefinedMessage });
	_mainProcMap.insert({ MainClassify::UndefinedMainCmdH, &ProcessMessageImpl::_ProcMainNodefinedMessage });
	_mainProcMap.insert({ MainClassify::HallLogic, &ProcessMessageImpl::_ProcHallMessage });
	_mainProcMap.insert({ MainClassify::RoomLogic, &ProcessMessageImpl::_ProcRoomMessage });
}


ProcessMessageImpl::~ProcessMessageImpl()
{
}

void ProcessMessageImpl::ProcJsonMessage(SessionType sid, std::string_view msg)
{
	if (SGetPacketHeaderLength() >= msg.length())
	{
		//log error info
		return;
	}
	MainClassify maincmd{ MainClassify::UndefinedMainCmdL };
	UndefineAssist asscmd{ UndefineAssist::UndefinedAssCmd };
	std::tie(maincmd, asscmd) = PackUtils::ExtractMainAssCmd(msg);
	auto it = _mainProcMap.find(maincmd);
	if (it != _mainProcMap.end())
	{
		ProcMainType func = it->second;
		(this->*func)(sid, static_cast<int>(asscmd), std::string_view({ &msg[SGetPacketHeaderLength()], msg.length() - SGetPacketHeaderLength() }));
	}
	else
	{
		//log error info
	}
}

void ProcessMessageImpl::ProcPbMessage(SessionType sid, std::string_view msg)
{
	auto pbmsg = _pBDispatcher->GenerateMessageFromString(msg);
	if (!pbmsg)
	{
		return;
	}
	_pBDispatcher->DispatchCustomMessage(pbmsg);
}


void ProcessMessageImpl::_ProcMainNodefinedMessage(SessionType sid, int ass, std::string_view msg)
{
	//log error info
}

void ProcessMessageImpl::_ProcRoomMessage(SessionType sid, int ass, std::string_view msg)
{
	RoomAssistRequest mass1 = static_cast<RoomAssistRequest>(ass);
	switch (mass1)
	{
	case RoomAssistRequest::UndefinedAssCmd:
	{
	}break;
	case RoomAssistRequest::EnterDesk:
	{
	}break;
	case RoomAssistRequest::LeaveDesk:
		break;
	case RoomAssistRequest::SendEmojiMessage:
	{

	}break;
	case RoomAssistRequest::SendTextMessage:
	{

	}break;
	default:
		break;
	}
}

void ProcessMessageImpl::_ProcUserHallLogin(UserHallLoginRequest* ur)
{

}

void ProcessMessageImpl::_ProcHallMessage(SessionType sid, int ass, std::string_view msg)
{
	HallAssistRequest mass2 = static_cast<HallAssistRequest>(ass);
	bool bSuccess = false;
	switch (mass2)
	{
	case HallAssistRequest::UndefinedAssCmd:
	{
	}break;
	case HallAssistRequest::UserLogin:
	{
		UserHallLoginRequest userrequest;
		bSuccess = iguana::json::from_json0(userrequest, &msg[0]);
		if (bSuccess)
		{
			_ProcUserHallLogin(&userrequest);
		}
		else
		{
			//log error info
		}
	}break;
	case HallAssistRequest::GetAllRooms:
	{
	}break;
	default:
		break;
	}
}

