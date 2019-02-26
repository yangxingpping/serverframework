#pragma once
/**************
 redis, mysql, sql server
**/
#include "CustomForwardDeclare.h"
#include "comm_def.h"

#include <memory>
#include <thread>

class DBManager
{
public:
	DBManager(RedisConfig redisconf = { "127.0.0.1", 6379 }, SqlDBConfig sqlconf = { "root", "123456", "_odbcmysql" });
	virtual ~DBManager();
	void startDBManager();
	void procPullThread();

	void procPullMessage(std::shared_ptr<std::string> msg);

private:
	otl_connect* _dbConnection=nullptr;
	std::shared_ptr<cpp_redis::client> _rdConnection=nullptr;
	std::shared_ptr<std::thread> _procPullMsgThread = nullptr;
	std::shared_ptr<moodycamel::ConcurrentQueue<std::shared_ptr<std::string>, moodycamel::ConcurrentQueueDefaultTraits>> _pullQueue;

	RedisConfig		_redisConfig;
	SqlDBConfig		_sqlConfig;
	//std::string		_sqlUser = "root";			// mysql server ip;
	//std::string		_sqlPass = "123456";		// mysql server port;
	//std::string		_sqlodbc = "_odbcmysql";	//msyql server odbc connection
	//std::string		_redisIp = "127.0.0.1"; // redis server ip;
	//uint16_t		_redisPort = 6379;		// redis server port; 
};

