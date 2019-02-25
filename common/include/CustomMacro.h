#pragma once

#include "spdlog/spdlog.h"
#include "comm_def.h"

#include <string>

#define C_LOGGER spdlog::get(LOGGER_NAME)

#define BEGIN_TRY try{

#define END_OTL_ACCESS }\
	catch(const otl_exception&  e)\
	{\
		C_LOGGER->error("otl error with {}", e.msg); \
	}\
	catch(...)\
	{\
		C_LOGGER->error("otl error with undefined error"); \
	}

#define END_JSON_PARSE }\
	catch(...)\
	{\
		C_LOGGER->error("parse json with undefined error"); \
	}


#define BEGIN_DB_ACCESS try \
	{

#define END_DB_ACCESS }\
	catch(otl_exception& e)\
	{\
		std::string strerr{"ִ��sql:" + strsql + ":�����쳣:" + e.msg};\
		C_LOGGER->warn(strerr);\
	}\
	catch(...)\
	{\
		std::string strerr{"ִ��sql:" + strsql + ":�����쳣:" + e.msg};\
		C_LOGGER->warn(strerr);\
	}