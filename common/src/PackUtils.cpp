#include "PackUtils.h"
#include "ThirdPartyuWS.h"

#ifndef C_WINDOWS
#include <arpa/inet.h>
#endif

std::tuple<MainClassify, UndefineAssist> PackUtils::ExtractMainAssCmd(std::string_view msg)
{
	//MainCmdType* netMain = (MainCmdType*)(&msg[0]);
	char temp = msg[0];
	MainCmdType localMain = MainCmdType((int)(temp-'0'));// ntohl(*netMain);
	//UndefineAssist* netAss = (UndefineAssist*)(&msg[0 + sizeof(MainCmdType)]);
	temp = msg[1];
	UndefineAssist localAss = UndefineAssist((int)(temp-'0'));// static_cast<UndefineAssist>(ntohl(static_cast<uint32_t>(*netAss)));
	return std::make_tuple(static_cast<MainClassify>(localMain), static_cast<UndefineAssist>(localAss));
}

std::string PackUtils::PackMainAssCmd(MainCmdType maincmd, UndefineAssist asscmd)
{
	std::string ret;
	//MainCmdType netMain = htonl(maincmd);
	//uint32_t netass = htonl(static_cast<uint32_t>(asscmd));
	//memcpy(&ret[0], (char*)&netMain, SGetMainLength());
	//memcpy(&ret[SGetMainLength()], (char*)&netass, SGetAssLength());
	ret += std::to_string((int)(maincmd));
	ret += std::to_string((int)(asscmd));
	return ret;
}

PackUtils::PackUtils()
{
}


PackUtils::~PackUtils()
{
}
