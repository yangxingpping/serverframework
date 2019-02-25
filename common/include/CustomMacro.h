#pragma once



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
