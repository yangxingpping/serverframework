#pragma once



#include "PBMsgCallBackInterface.h"

#include <functional>

template<class T>
class PBMsgCallBackImpl : public PBMsgCallBackInterface
{
public:
	typedef std::function<void(T* msg)> PBMessageCallBackType;	

public:
	PBMsgCallBackImpl(const PBMessageCallBackType cb):_callback(cb)
	{
		//_callback = cb;
	}
	virtual void onMessage(google::protobuf::Message* msg) const override
	{
		T* acutalmsg = dynamic_cast<T*>(msg);
		if (acutalmsg)
		{
			_callback(acutalmsg);
		}
	}

private:
	PBMessageCallBackType _callback;
};