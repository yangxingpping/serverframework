#pragma once
/************************************************************************/
/* 

*/
/************************************************************************/
#include "ProcessMessageInterface.h"
#include "share_def.h"
#include "CustomForwardDeclare.h"

#include <tuple>
#include <map>
#include <memory>

class ProcessMessageImpl :
	public ProcessMessageInterface
{
public:
	ProcessMessageImpl();
	virtual ~ProcessMessageImpl();
	
	virtual void ProcJsonMessage(SessionType sid, std::string_view msg) override;
	
	virtual void ProcPbMessage(SessionType sid, std::string_view msg) override;

protected:
	void _ProcMainNodefinedMessage(SessionType sid, int ass, std::string_view msg);
	void _ProcHallMessage(SessionType sid, int ass, std::string_view msg);
	void _ProcRoomMessage(SessionType sid, int ass, std::string_view msg);
	
protected:
	void _ProcUserHallLogin(UserHallLoginRequest* ur);

private:
	using ProcMainType = void (ProcessMessageImpl::*)(SessionType, int, std::string_view);
	std::map<MainClassify, ProcMainType> _mainProcMap;

	std::shared_ptr<PBMsgCallBackDispatcher> _pBDispatcher;
};

