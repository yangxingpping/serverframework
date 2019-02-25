#pragma once
/**************
 redis, mysql, sql server
**/
#include "CustomForwardDeclare.h"
#include <memory>

class DBManager
{
public:
	DBManager();
	virtual ~DBManager();

private:
	//std::shared_ptr<otl_connect> _dbConnection;
	otl_connect* _dbConnection=nullptr;
	std::shared_ptr<cpp_redis::client> _rdConnection;
};

