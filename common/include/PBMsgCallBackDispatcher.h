#pragma once

//protobuf

#include "PBMsgCallBackImpl.h"

#include <map>
#include <string_view>

using MapPBDescriptorToCallBack = std::map<const google::protobuf::Descriptor*, std::shared_ptr<PBMsgCallBackInterface>>;

class PBMsgCallBackDispatcher
{
public:
	PBMsgCallBackDispatcher();
	~PBMsgCallBackDispatcher();
	google::protobuf::Message* GenerateMessageFromString(std::string_view strmsg);
	void DispatchCustomMessage(google::protobuf::Message* msg);

	template<class T>
	void RegisterMessageCallback(const typename PBMsgCallBackImpl<T>::PBMessageCallBackType& cb)
	{
		std::shared_ptr<PBMsgCallBackInterface> pd(new PBMsgCallBackImpl<T>(cb));
		_mapCallbacks[T::descriptor()] = pd;
	}
private:
	MapPBDescriptorToCallBack _mapCallbacks;
};

