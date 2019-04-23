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
	DBManager(RedisConfig redisconf = { "192.168.5.138", 6379 }, SqlDBConfig sqlconf = { "root", "123456", "test" });
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
};

