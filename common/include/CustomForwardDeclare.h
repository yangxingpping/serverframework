#pragma once

class WebBaseInterface;
class otl_connect;

class SessionRequestMessageBody;
class ProcManager;
class CPUIntensiveThread;
class WebManagerPair;
class SocketManagerPair;
class MessageQueueManager;
class ProcessMessageInterface;
class ProcessMessageImpl;
class PBMsgCallBackDispatcher;
class DBManager;
class NNOtherServersManager;


struct UserHallLoginRequest;


namespace cpp_redis
{
	class client;
}

namespace uS
{
	struct Async;
}

struct us_loop;

namespace uWS
{
	template<bool SSL, bool isServer>
	struct WebSocket;
	template<bool SSL>
	struct HttpResponse;
	struct Loop;
}

namespace spdlog
{
	class logger;
}


namespace google
{
	namespace protobuf
	{
		class Message;
		class Descriptor;
	}
}


namespace moodycamel
{
	struct ConcurrentQueueDefaultTraits;
	template<typename T, typename Traits>
	class ConcurrentQueue;
}

struct uv_async_s;
typedef struct uv_async_s uv_async_t;

enum class MultiBussinessType;

template<typename UserSpaceSessionType, MultiBussinessType BussID>
class MulticastBusinessBase;

//
//struct uv_tcp_s;
//typedef struct uv_tcp_s uv_tcp_t;
//
//struct uv_stream_s;
//typedef struct uv_stream_s uv_stream_t;
//
//struct uv_tcp_s;
//typedef struct uv_tcp_s uv_tcp_t;
//
//struct uv_handle_s;
//typedef struct uv_handle_s uv_handle_t;
//
//
//struct  uv_buf_t;
//
struct uv_loop_s;
typedef struct uv_loop_s uv_loop_t;
//
//typedef ssize_t ssize_t;
