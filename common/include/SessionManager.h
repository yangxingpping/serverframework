#pragma once

#include "share_def.h"
#include "Utils.h"

#include "boost/bimap.hpp"

#include <map>
#include <set>
#include <vector>
#include <typeinfo>
#include <iostream>

template<typename SCTP>
class SessionManager
{
public:
	SessionManager() {};
	~SessionManager() {};

	SessionType AddSession(SCTP pt);
	SessionType RemoveSession(SCTP pt);
	void AddToGroupSession(SessionType sid, MultibroadSessionType group);

	SessionType GetSessionIDByConnectionPt(SCTP pt);
	SCTP GetConnectionPtByUnicastSessionID(SessionType sessiond);
	std::vector< SCTP> GetConnectionsByGroupID(MultibroadSessionType groupid);
	std::vector< SCTP> GetAllConnections();

private:
	boost::bimap<SessionType, SCTP> _connectionPt2Sessions;
	std::map< MultibroadSessionType, std::set<SessionType>> _multibroad2Sessions;
	std::map<SessionType, std::multiset<MultibroadSessionType>>	_session2Multibroad;
};

template<typename SCTP>
std::vector< SCTP> SessionManager<SCTP>::GetAllConnections()
{
	std::vector<SCTP> vret;
	for (auto it = _connectionPt2Sessions.left.begin(); it != _connectionPt2Sessions.left.end(); ++it)
	{
		vret.push_back(it->second);
	}
	return vret;
}

template<typename SCTP>
std::vector< SCTP> SessionManager<SCTP>::GetConnectionsByGroupID(MultibroadSessionType groupid)
{
	std::vector< SCTP> vret;
	auto it = _multibroad2Sessions.find(groupid);
	if (it != _multibroad2Sessions.end())
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			if (*it2 == INVALID_SESSION_ID)
			{
				continue;
			}
			auto it3 = _connectionPt2Sessions.left.find(*it2);
			if (it3 != _connectionPt2Sessions.left.end())
			{
				vret.push_back(it3->second);
			}
		}
	}
	return vret;
}

template<typename SCTP>
void SessionManager<SCTP>::AddToGroupSession(SessionType sid, MultibroadSessionType group)
{
	_session2Multibroad[sid].insert(group);
	_multibroad2Sessions[group].insert(sid);
}

template<typename SCTP>
SCTP SessionManager<SCTP>::GetConnectionPtByUnicastSessionID(SessionType sessiond)
{
	static_assert(std::is_pointer< SCTP>::value);
	SCTP pRet = nullptr;
	auto it = _connectionPt2Sessions.left.find(sessiond);
	if (it != _connectionPt2Sessions.left.end())
	{
		pRet = it->second;
	}
	return pRet;
}

template<typename SCTP>
SessionType SessionManager<SCTP>::GetSessionIDByConnectionPt(SCTP pt)
{
	SessionType sRet{ INVALID_SESSION_ID };
	auto it = _connectionPt2Sessions.right.find(pt);
	sRet = (it == _connectionPt2Sessions.right.end()) ? INVALID_SESSION_ID : it->second;
	return sRet;
}

template<typename SCTP>
SessionType SessionManager<SCTP>::RemoveSession(SCTP pt)
{
	static_assert(std::is_pointer< SCTP>::value);
	if (!pt || _connectionPt2Sessions.right.find(pt) == _connectionPt2Sessions.right.end())
	{
		return INVALID_SESSION_ID;
	}
	auto it = _connectionPt2Sessions.right.find(pt);
	auto sRet = it->second;
	_connectionPt2Sessions.right.erase(it);
	//remove multicast group
	auto itsid2groupid = _session2Multibroad.find(sRet);
	if (itsid2groupid != _session2Multibroad.end())
	{
		for (auto itgroup = itsid2groupid->second.begin(); itgroup != itsid2groupid->second.end(); ++itgroup)
		{
			_multibroad2Sessions[*itgroup].erase(sRet);
		}
	}
	return sRet;
}

template<typename SCTP>
SessionType SessionManager<SCTP>::AddSession(SCTP pt)
{
	static_assert(std::is_pointer< SCTP>::value);
	if (!pt || _connectionPt2Sessions.right.find(pt) != _connectionPt2Sessions.right.end())
	{
		return INVALID_SESSION_ID;
	}
	auto newRet = Utils::SGetSessionIndex();
	_connectionPt2Sessions.left.insert({ newRet, pt });
	return newRet;
}

