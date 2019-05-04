
#include <string.h>
#include "NngRelateManager.h"
#include "DBManager.h"

#include "concurrentqueue/concurrentqueue.h"
#include "nng/nng.h"
#include "nng/protocol/pubsub0/pub.h"
#include "nng/protocol/pipeline0/pull.h"

NngRelateManager::NngRelateManager(std::string pubaddr /*= "tcp://127.0.0.1:100"*/, std::string pulladdr /*= "tcp://127.0.0.1:101"*/)
	:_pubAddress(pubaddr), _pullAddress(pulladdr)
{
	_pubQueue = std::make_shared<moodycamel::ConcurrentQueue<std::shared_ptr<std::string>, moodycamel::ConcurrentQueueDefaultTraits>>();
}



bool NngRelateManager::startNngRelateManager()
{
	_dbManager = std::make_shared<DBManager>();
	_pubThread = std::make_shared<std::thread>(&NngRelateManager::pubThreadFunc, this);
	_pullThread = std::make_shared<std::thread>(&NngRelateManager::pullThreadFunc, this);
	_respThread = std::make_shared<std::thread>(&NngRelateManager::respThreadFunc, this);
	return true;
}

void NngRelateManager::stopNngRelateManager()
{
	//send posion message to pub message queue and pull socket and then join
	_pubThread->join();
	_pullThread->join();
}

void NngRelateManager::addPubMessage(std::shared_ptr<std::string> msg)
{
	_pubQueue->enqueue(msg);
}

NngRelateManager::~NngRelateManager()
{
}

void NngRelateManager::pubThreadFunc()
{
	
	nng_socket _pubsocket;
	int nn_result = 0;
	nn_result = nng_pub_open(&_pubsocket);
	nn_result = nng_listen(_pubsocket, _pubAddress.c_str(), NULL, 0);
	while (true)
	{
		std::shared_ptr<std::string> msg = std::make_shared<std::string>("");
		if (_pubQueue->try_dequeue(msg) && !(msg->empty()))
		{
			nn_result = nng_send(_pubsocket, &(*msg)[0], msg->length(), 0);
		}
	}
}

void NngRelateManager::pullThreadFunc()
{
	nng_socket _pullsocket;
	char msgbuf[10 * 1024]; //max nng message size is 10*1024
	size_t real_size = 0;
	int nn_result = 0;
	nn_result = nng_pull_open(&_pullsocket);
	nn_result = nng_listen(_pullsocket, _pullAddress.c_str(), NULL, 0);
	while (true)
	{
		memset(&msgbuf[0], 0, sizeof(msgbuf[0]));
		real_size = 0;
		nn_result = nng_recv(_pullsocket, &msgbuf[0], &real_size, 0);
	}
}

void NngRelateManager::respThreadFunc()
{
	nng_socket _respsocket;
	char msgbuf[10 * 1024]; //max nng message size is 10*1024
	size_t real_size = 0;
	int nn_result = 0;
	nn_result = nng_pull_open(&_respsocket);
	nn_result = nng_listen(_respsocket, _respAddr.c_str(), NULL, 0);
	while (true)
	{
		memset(&msgbuf[0], 0, sizeof(msgbuf[0]));
		real_size = 0;
		nn_result = nng_recv(_respsocket, &msgbuf[0], &real_size, 0);
	}
}
