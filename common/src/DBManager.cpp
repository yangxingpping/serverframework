
#include "DBManager.h"
#include "CustomMacro.h"

#include "cpp_redis/cpp_redis"
#include "otlv4_h2/otlv4.h"
#include "concurrentqueue/concurrentqueue.h"

#include <string>



DBManager::DBManager(RedisConfig redisconf, SqlDBConfig sqlconf)
	:_redisConfig(redisconf), _sqlConfig(sqlconf)
{

}

DBManager::~DBManager()
{
	if(_dbConnection)
	{
		delete _dbConnection;
		_dbConnection=nullptr;
	}
}

void DBManager::startDBManager()
{
	std::string strsql{ "connect failed with sql user=" + _sqlConfig._user + ",pass=" + _sqlConfig._pass + ",odbc=" + _sqlConfig._odbc };
	try
	{
		_dbConnection = new otl_connect();
		_rdConnection = std::make_shared<cpp_redis::client>();
		_rdConnection->connect(_redisConfig._ip, _redisConfig._port);
		_dbConnection->rlogon(_sqlConfig._user.c_str(), _sqlConfig._pass.c_str(), _sqlConfig._odbc.c_str());
		_pullQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<std::string>, moodycamel::ConcurrentQueueDefaultTraits>>();
		_procPullMsgThread = std::make_shared<std::thread>(&DBManager::procPullThread, this);
	}
	catch (otl_exception & e)
	{
		std::cout << "otl error : " << e.msg << std::endl;
	}
	catch (cpp_redis::redis_error& e)
	{
		std::cout << "redis error : " << e.what() << std::endl;
	}
	catch (std::exception & e) {
		std::cout << "std exception : " << e.what() << std::endl;
	}
	catch (...) {
		std::cout << "uncatched exception" << std::endl;
	}
	
}

void DBManager::procPullThread()
{
	while (true)
	{
		std::shared_ptr<std::string> msg;
		if (_pullQueue->try_dequeue(msg))
		{
			procPullMessage(msg);
		}
	}
}

void DBManager::procPullMessage(std::shared_ptr<std::string> msg)
{

}
