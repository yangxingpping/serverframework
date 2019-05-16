#include "Utils.h"
#include "comm_def.h"


#include "xxhash.h"
#include "spdlog/spdlog.h"
#include "cppcodec/base64_rfc4648.hpp"
#include "cppcodec/parse_error.hpp"


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
	return  XXH64(str.c_str(), str.length(), seed);
}

std::string Utils::DecodeBase64(const char* pmsg, size_t len)
{
	static thread_local std::string ret;
	ret.reserve(10 * 1024);
	if (!pmsg || len == 0)
	{
		return ret;
	}
	
	using base64 = cppcodec::base64_rfc4648;
	try
	{
		std::vector<uint8_t> strmsg = base64::decode(pmsg, len);
		ret.assign({ strmsg.begin(),strmsg.end() });
	}
	catch (cppcodec::parse_error e)
	{
		std::string errInfo = "data is not encode in base64:";
		errInfo += e.what();
		LOG_ALQAZ->warn(errInfo);
	}
	return ret;
}

Utils::Utils()
{
}


Utils::~Utils()
{
}
