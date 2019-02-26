#pragma once

//protobuf

//#include "query.pb.h"
#include "CustomForwardDeclare.h"

class PBMsgCallBackInterface
{
public:
	virtual void onMessage(google::protobuf::Message* msg) const = 0;
	virtual ~PBMsgCallBackInterface() {};
};