#pragma once

#include "CustomForwardDeclare.h"
#include "share_def.h"

#include <mutex>
#include <memory>
#include <string>

class Utils
{
public:
	static SessionType _SsessionIndex;
	static std::mutex _SSessionMutex;
	static SessionType SGetSessionIndex();
	static std::shared_ptr<spdlog::logger> SGetLogger();
	static MD564Type SCalculHash(const std::string str, MD564Type seed=0);
	static std::string DecodeBase64(const char* pmsg, size_t len);
public:
	Utils();
	~Utils();
};

#define LOG_ALQAZ Utils::SGetLogger()



