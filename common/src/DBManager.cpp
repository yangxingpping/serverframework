
#include "DBManager.h"
#include "CustomMacro.h"

#include "cpp_redis/cpp_redis"
#include "otlv4_h2/otlv4.h"
#include "concurrentqueue.h"

#include <string>



DBManager::DBManager(std::string sqluser/*="127.0.0.1"*/, std::string sqlpass /*= "123456"*/, std::string odbc /*= "_odbcmysql"*/, std::string redisip/*="127.0.0.1"*/, uint16_t redisport/*=6379*/)
	:_sqlUser(sqluser), _sqlPass(sqlpass), _redisIp(redisip), _redisPort(redisport), _sqlodbc(odbc)
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
	std::string strsql{ "connect failed with sql user=" + _sqlUser + ",pass=" + _sqlPass + ",odbc=" + _sqlodbc };
	_dbConnection = new otl_connect();
	_rdConnection = std::make_shared<cpp_redis::client>();
	_rdConnection->connect(_redisIp, _redisPort);
	_dbConnection->rlogon(_sqlUser.c_str(), _sqlPass.c_str(), _sqlodbc.c_str());
	_procPullMsgThread = std::make_shared<std::thread>(&DBManager::procPullThread, this);
	_pullQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<std::string>, moodycamel::ConcurrentQueueDefaultTraits>>();
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
