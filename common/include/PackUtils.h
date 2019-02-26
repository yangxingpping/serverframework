#pragma once


#include "PrimitiveDef.h"
#include "share_def.h"

#include <string_view>
#include <tuple>
#include <array>
#include <string>

constexpr size_t SGetMainLength()
{
	return 1;// sizeof(MainCmdType);
}

constexpr size_t SGetAssLength()
{
	return 1;// sizeof(AssCMDType);
}

constexpr size_t SGetPacketHeaderLength()
{
	return SGetMainLength() + SGetAssLength();
}

class PackUtils
{
public:
	static std::tuple<MainClassify, UndefineAssist> ExtractMainAssCmd(std::string_view msg);
	static std::string PackMainAssCmd(MainCmdType maincmd, UndefineAssist asscmd);
public:
	PackUtils();
	~PackUtils();
 
};

