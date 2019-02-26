#pragma once
/************************************************************************/
/* 

*/
/************************************************************************/

#include "PBMesssageForwardDeclare.h"

class PBMessageCallBacks
{
public:
	static void onQuery(muduo::Query* query);
public:
	PBMessageCallBacks();
	~PBMessageCallBacks();
};

