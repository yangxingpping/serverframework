#include "Utils.h"
#include "comm_def.h"


#include "xxhash.h"
#include "spdlog/spdlog.h"


SessionType Utils::_SsessionIndex = 0;
std::mutex Utils::_SSessionMutex;

SessionType Utils::SGetSessionIndex()
{
	std::lock_guard<std::mutex> guard(_SSessionMutex);
	_SsessionIndex++;
	if (_SsessionIndex == INVALID_SESSION_ID)
	{
		_SsessionIndex++;
	}
	return _SsessionIndex;
}


std::shared_ptr<spdlog::logger> Utils::SGetLogger()
{
	static std::shared_ptr<spdlog::logger> s_logger;
	if (!s_logger)
	{
		s_logger = spdlog::get(LOGGER_NAME);
	}
	return s_logger;
}


MD564Type Utils::SCalculHash(const std::string str, MD564Type seed/*=0*/)
{
	return XXH64(str.c_str(), str.length(), seed);
}

Utils::Utils()
{
}


Utils::~Utils()
{
}
