#include "query.pb.h"
#include "PBMsgCallBackDispatcher.h"



PBMsgCallBackDispatcher::PBMsgCallBackDispatcher()
{
}


PBMsgCallBackDispatcher::~PBMsgCallBackDispatcher()
{
}

google::protobuf::Message* PBMsgCallBackDispatcher::GenerateMessageFromString(std::string_view strmsg)
{
	using namespace google::protobuf;
	auto fullname = muduo::Query::descriptor()->full_name();
	google::protobuf::Message* ret = nullptr;
	size_t pos1 = strmsg.find_first_of("|");
	if (pos1 == std::string::npos)
	{
		//log error info (protobuf message error)
		return ret;
	}
	const std::string typeName{ &strmsg[0], pos1  };
	const auto pbdescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
	if (!pbdescriptor)
	{
		//log error info (can not find protobuf object by typename)
		return ret;
	}
	const auto* prototype = MessageFactory::generated_factory()->GetPrototype(pbdescriptor);
	if (!prototype)
	{
		//log error info
		return ret;
	}
	ret = prototype->New();
	bool bb = ret->ParseFromString(std::string(&strmsg[pos1 + 1]));
	if (!bb)
	{
		//log error info
	}
	return ret;
}

void PBMsgCallBackDispatcher::DispatchCustomMessage(google::protobuf::Message* msg)
{
	MapPBDescriptorToCallBack::iterator it = _mapCallbacks.find(msg->GetDescriptor());
	if (it != _mapCallbacks.end())
	{
		
		it->second->onMessage(msg);
	}
}
