#include "cpp_redis/cpp_redis"
#include "otlv4_h2/otlv4.h"
#include "DBManager.h"



DBManager::DBManager()
{
	_dbConnection = new otl_connect();
	_rdConnection = std::make_shared<cpp_redis::client>();
}


DBManager::~DBManager()
{
	if(_dbConnection)
	{
		delete _dbConnection;
		_dbConnection=nullptr;
	}
}
