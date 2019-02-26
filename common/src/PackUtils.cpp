#include "PackUtils.h"
#include "ThirdPartyuWS.h"

#ifndef C_WINDOWS
#include <arpa/inet.h>
#endif

std::tuple<MainClassify, UndefineAssist> PackUtils::ExtractMainAssCmd(std::string_view msg)
{
	MainCmdType* netMain = (MainCmdType*)(&msg[0]);
	MainCmdType localMain = ntohl(*netMain);
	UndefineAssist* netAss = (UndefineAssist*)(&msg[0 + sizeof(MainCmdType)]);
	UndefineAssist localAss = static_cast<UndefineAssist>(ntohl(static_cast<uint32_t>(*netAss)));
	return std::make_tuple(static_cast<MainClassify>(localMain), static_cast<UndefineAssist>(localAss));
}

std::array<char, SGetPacketHeaderLength()> PackUtils::PackMainAssCmd(MainCmdType maincmd, UndefineAssist asscmd)
{
	std::array<char, SGetPacketHeaderLength()> ret;
	MainCmdType netMain = htonl(maincmd);
	uint32_t netass = htonl(static_cast<uint32_t>(asscmd));
	memcpy(&ret[0], (char*)&netMain, SGetMainLength());
	memcpy(&ret[SGetMainLength()], (char*)&netass, SGetAssLength());
	return ret;
}

PackUtils::PackUtils()
{
}


PackUtils::~PackUtils()
{
}
