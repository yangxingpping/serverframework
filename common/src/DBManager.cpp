
#include "DBManager.h"
#include "CustomMacro.h"

#include "cpp_redis/cpp_redis"
#include "otlv4_h2/otlv4.h"



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
	//BEGIN_DB_ACCESS
	_dbConnection->rlogon(_sqlUser.c_str(), _sqlPass.c_str(), _sqlodbc.c_str());
	//END_DB_ACCESS;
}
